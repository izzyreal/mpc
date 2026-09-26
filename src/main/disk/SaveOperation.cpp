#include "SaveOperation.hpp"

#include "Mpc.hpp"
#include "Logger.hpp"
#include "StrUtil.hpp"
#include "Util.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/SaveDestination.hpp"
#include "file/kaitai/ApsIo.hpp"
#include "file/kaitai/PgmIo.hpp"
#include "file/kaitai/SndIo.hpp"
#include "file/wav/WavFile.hpp"
#include "lcdgui/screens/LoadScreen.hpp"
#include "lcdgui/screens/window/SaveAProgramScreen.hpp"
#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"
#include "sampler/Sound.hpp"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <streambuf>
#include <algorithm>
#include <thread>

using namespace mpc;
using namespace mpc::disk;
using namespace mpc::lcdgui;

namespace
{
    enum class Step
    {
        Prepare,
        Header,
        Sound,
        Scan
    };

    struct Job
    {
        std::shared_ptr<void> lease;
        std::shared_ptr<AbstractDisk>
            disk; // Pins raw volume through final flush.
        std::unique_ptr<SaveDestination> destination;
        std::string filename;
        std::vector<char> header;
        std::vector<sampler::Sound> sounds;
        bool wav = false, replaceSounds = false, replaceHeader = false;
        // The following fields belong exclusively to the worker.
        size_t nextSound = 0;
        bool prepared = false, headerWritten = false, collision = false,
             failed = false;
    };

    struct Result
    {
        Step step;
        bool collision = false, skipped = false;
        std::string error;
        std::optional<DirectoryListing> listing;
    };

    std::vector<char> soundBytes(sampler::Sound &sound, bool wav)
    {
        if (!wav)
        {
            return file::kaitai::SndIo::saveSound(sound);
        }
        // Encode directly into the write buffer, avoiding full WAV copies.
        struct Buffer : std::streambuf
        {
            std::vector<char> bytes;
            std::streamsize xsputn(const char *data,
                                   std::streamsize size) override
            {
                bytes.insert(bytes.end(), data, data + size);
                return size;
            }
            int_type overflow(int_type value) override
            {
                if (!traits_type::eq_int_type(value, traits_type::eof()))
                {
                    bytes.push_back(traits_type::to_char_type(value));
                }
                return traits_type::not_eof(value);
            }
        } buffer;
        const auto data = sound.getSampleData();
        buffer.bytes.reserve(44 + data->size() * 2);
        auto stream = std::make_shared<std::ostream>(&buffer);
        const auto channels = sound.isMono() ? 1 : 2;
        auto writer = file::wav::WavFile::writeWavStream(
            stream, channels, sound.getFrameCount(), 16, sound.getSampleRate());
        if (sound.isMono())
        {
            writer.writeFrames(*data, sound.getFrameCount());
        }
        else
        {
            constexpr int chunkFrames = 1024;
            std::vector<float> interleaved(chunkFrames * 2);
            const auto frames = sound.getFrameCount();
            for (int start = 0; start < frames; start += chunkFrames)
            {
                const auto count = std::min(chunkFrames, frames - start);
                for (int i = 0; i < count; ++i)
                {
                    interleaved[i * 2] = (*data)[start + i];
                    interleaved[i * 2 + 1] = (*data)[frames + start + i];
                }
                writer.writeFrames(interleaved, count);
            }
        }
        writer.finishWriteChecked();
        if (!*stream)
        {
            throw std::runtime_error("Unable to encode WAV");
        }
        return std::move(buffer.bytes);
    }

    Result execute(Job &job, Step step)
    {
        Result result{step};
        try
        {
            switch (step)
            {
                case Step::Prepare:
                    job.destination->prepare();
                    job.prepared = true;
                    result.collision = job.collision =
                        !job.replaceHeader &&
                        bool(job.destination->find(job.filename));
                    break;
                case Step::Header:
                    job.destination->create(job.filename, job.replaceHeader)
                        ->setFileDataChecked(job.header);
                    job.destination->flush();
                    job.headerWritten = true;
                    break;
                case Step::Sound:
                {
                    auto &sound = job.sounds.at(job.nextSound);
                    const auto extension = job.wav ? ".WAV" : ".SND";
                    const auto lookupName =
                        StrUtil::replaceAll(sound.getName(), ' ', "") +
                        extension;
                    const auto existing = job.destination->find(lookupName);
                    if (existing && !job.replaceSounds)
                    {
                        result.skipped = true;
                    }
                    else
                    {
                        // Preserve the legacy lookup and output naming rules.
                        if (existing)
                        {
                            job.destination->remove(existing);
                        }
                        auto bytes = soundBytes(sound, job.wav);
                        const auto outputName =
                            Util::getFileName(sound.getName() + extension);
                        job.destination->create(outputName, bool(existing))
                            ->setFileDataChecked(bytes);
                        job.destination->flush();
                    }
                    ++job.nextSound;
                    break;
                }
                case Step::Scan:
                    job.destination->flush();
                    result.listing = job.destination->scan();
                    break;
            }
        }
        catch (const std::exception &e)
        {
            job.failed = true;
            result.error = e.what();
        }
        catch (...)
        {
            job.failed = true;
            result.error = "Unknown file I/O error";
        }
        if (!result.error.empty())
        {
            MLOG("Save " + job.filename + ": " + result.error);
        }
        return result;
    }
} // namespace

struct SaveOperation::Impl
{
    Mpc &mpc;
    Now now;
    std::atomic<bool> active{false};
    std::mutex mutex;
    std::condition_variable condition;
    std::shared_ptr<Job> job;
    std::optional<Step> command;
    std::optional<Result> result;
    bool stopping = false;
    std::thread worker;
    std::function<void()> onExists;
    std::optional<Clock::time_point> deadline;
    size_t soundIndex = 0;
    std::string failure;

    Impl(Mpc &mpc, Now now)
        : mpc(mpc), now(std::move(now)), worker(
                                             [this]
                                             {
                                                 run();
                                             })
    {
    }

    void run()
    {
        for (;;)
        {
            std::unique_lock lock(mutex);
            condition.wait(lock,
                           [this]
                           {
                               return stopping || command.has_value();
                           });
            auto current = job;
            if (stopping)
            {
                lock.unlock();
                // No UI callbacks or pacing during teardown. Never authorize an
                // overwrite that still requires a user decision.
                if (current && !current->failed)
                {
                    if (!current->prepared)
                    {
                        execute(*current, Step::Prepare);
                    }
                    if (!current->collision && !current->failed)
                    {
                        if (!current->headerWritten)
                        {
                            execute(*current, Step::Header);
                        }
                        while (!current->failed &&
                               current->nextSound < current->sounds.size())
                        {
                            execute(*current, Step::Sound);
                        }
                        execute(*current, Step::Scan);
                    }
                }
                return;
            }
            const auto step = *command;
            command.reset();
            lock.unlock();
            auto completed = execute(*current, step);
            lock.lock();
            result = std::move(completed);
        }
    }

    void submit(Step step)
    {
        std::lock_guard lock(mutex);
        command = step;
        condition.notify_one();
    }

    bool start(const std::shared_ptr<sampler::Program> &program,
               const std::string &filename, bool replace,
               std::function<void()> exists)
    {
        if (active || stopping)
        {
            return false;
        }
        auto lease = mpc.fileOperationGate.tryAcquire(true);
        if (!lease)
        {
            return false;
        }
        auto next = std::make_shared<Job>();
        next->lease = std::move(lease);
        try
        {
            next->disk = mpc.getDisk();
            next->filename = filename;
            next->replaceHeader = replace;
            next->destination = next->disk->captureSaveDestination(
                mpc.screens->get<ScreenId::LoadScreen>()->view);
            const auto options =
                mpc.screens->get<ScreenId::SaveAProgramScreen>();
            next->wav = options->save == 2;
            next->replaceSounds = options->replaceSameSounds;
            const auto sampler = mpc.getSampler();
            next->header =
                program ? file::kaitai::PgmIo::saveProgram(*program, sampler)
                        : file::kaitai::ApsIo::save(
                              mpc,
                              mpc_fs::path(StrUtil::toUpper(StrUtil::replaceAll(
                                               filename, ' ', "_")))
                                  .stem()
                                  .string());
            if (options->save != 0)
            {
                if (program)
                {
                    for (const auto &note : program->getNotesParameters())
                    {
                        if (note->getSoundIndex() != -1)
                        {
                            const auto sound =
                                sampler->getSound(note->getSoundIndex());
                            if (!sound)
                            {
                                throw std::runtime_error(
                                    "Missing program sound");
                            }
                            next->sounds.push_back(*sound);
                        }
                    }
                }
                else
                {
                    for (const auto &sound : sampler->getSounds())
                    {
                        next->sounds.push_back(*sound);
                    }
                }
            }
        }
        catch (const std::exception &e)
        {
            MLOG(std::string("I/O error: ") + e.what());
            showError();
            return false;
        }
        onExists = std::move(exists);
        deadline.reset();
        failure.clear();
        soundIndex = 0;
        active.store(true);
        {
            std::lock_guard lock(mutex);
            job = std::move(next);
        }
        submit(Step::Prepare);
        return true;
    }

    void showError()
    {
        mpc.getLayeredScreen()->showPopupAndThenReturnToLayer(
            "I/O error! See logs for info",
            static_cast<int>(
                mpc.getFileOperationTimings().ioErrorFeedback.count()),
            0);
    }

    void advance()
    {
        deadline.reset();
        if (soundIndex == job->sounds.size())
        {
            submit(Step::Scan);
            return;
        }
        const auto name =
            StrUtil::replaceAll(job->sounds[soundIndex].getName(), ' ', "");
        mpc.getLayeredScreen()->showPopup("Saving " +
                                          StrUtil::padRight(name, " ", 16) +
                                          (job->wav ? ".WAV" : ".SND"));
        submit(Step::Sound);
    }

    void release()
    {
        active.store(false);
        std::lock_guard lock(mutex);
        job.reset();
        onExists = {};
    }

    void tick()
    {
        if (!active)
        {
            return;
        }
        if (deadline)
        {
            if (now() >= *deadline)
            {
                advance();
            }
            return;
        }
        std::optional<Result> completed;
        {
            std::lock_guard lock(mutex);
            completed.swap(result);
        }
        if (!completed)
        {
            return;
        }
        if (!completed->error.empty())
        {
            if (failure.empty())
            {
                failure = completed->error;
            }
            if (completed->step != Step::Scan)
            {
                submit(Step::Scan);
                return;
            }
        }
        if (completed->step == Step::Scan)
        {
            const bool failed = !failure.empty();
            const bool hasSounds = !job->sounds.empty();
            if (completed->listing)
            {
                job->disk->prepareListingForNextRefresh(
                    std::move(*completed->listing));
            }
            release();
            if (failed)
            {
                showError();
            }
            else if (hasSounds)
            {
                mpc.getLayeredScreen()->openScreenById(ScreenId::SaveScreen);
            }
            else
            {
                mpc.getLayeredScreen()->closeRecentScreensUntilReachingLayer(0);
            }
        }
        else if (completed->collision)
        {
            auto callback = std::move(onExists);
            release();
            if (callback)
            {
                callback();
            }
            else
            {
                showError();
            }
        }
        else if (completed->step == Step::Prepare)
        {
            submit(Step::Header);
        }
        else if (completed->step == Step::Header)
        {
            if (job->sounds.empty())
            {
                mpc.getLayeredScreen()->showPopup("Saving " + job->filename);
            }
            deadline = now() + mpc.getFileOperationTimings().saveTransition;
        }
        else
        {
            ++soundIndex;
            if (completed->skipped)
            {
                advance();
            }
            else
            {
                deadline =
                    now() + mpc.getFileOperationTimings().progressDisplay;
            }
        }
    }

    void shutdown()
    {
        {
            std::lock_guard lock(mutex);
            stopping = true;
            condition.notify_one();
        }
        if (worker.joinable())
        {
            worker.join();
        }
        result.reset();
        command.reset();
        release();
    }
};

SaveOperation::SaveOperation(Mpc &mpc, Now now)
    : impl(std::make_unique<Impl>(mpc, std::move(now)))
{
}
SaveOperation::~SaveOperation()
{
    shutdown();
}
bool SaveOperation::startProgram(
    const std::shared_ptr<sampler::Program> &program, const std::string &name,
    bool replace, std::function<void()> exists)
{
    return impl->start(program, name, replace, std::move(exists));
}
bool SaveOperation::startAps(const std::string &name, bool replace,
                             std::function<void()> exists)
{
    return impl->start({}, name, replace, std::move(exists));
}
bool SaveOperation::isActive() const
{
    return impl->active.load();
}
void SaveOperation::tick()
{
    impl->tick();
}
void SaveOperation::shutdown()
{
    impl->shutdown();
}
