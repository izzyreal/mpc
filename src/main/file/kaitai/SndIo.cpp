#include "file/kaitai/SndIo.hpp"

#include "SampleOps.hpp"
#include "StrUtil.hpp"
#include "disk/MpcFile.hpp"
#include "file/kaitai/KaitaiIoUtil.hpp"
#include "file/kaitai/generated/mpc2000snd.h"
#include "sampler/Sound.hpp"

using namespace mpc::file::kaitai;
using namespace mpc::sampleops;

sound_or_error SndIo::loadSound(const std::shared_ptr<mpc::disk::MpcFile> &file,
                                const std::shared_ptr<mpc::sampler::Sound> &sound,
                                const std::string &nameWithoutExtension)
{
    const auto bytes = file->getBytes();

    std::stringstream parseStream(
        std::string(bytes.begin(), bytes.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    ::kaitai::kstream parseIo(&parseStream);
    mpc2000snd_t parsed(&parseIo);
    parsed._read();

    auto parsedName = parsed.name();
    const auto nulPos = parsedName.find('\0');
    if (nulPos != std::string::npos)
    {
        parsedName.erase(nulPos);
    }
    while (!parsedName.empty() && parsedName.back() == ' ')
    {
        parsedName.pop_back();
    }

    const auto soundName = StrUtil::eqIgnoreCase(parsedName, nameWithoutExtension)
                               ? parsedName
                               : nameWithoutExtension;

    sound->setName(soundName);
    sound->setMono(!parsed.stereo());
    sound->setSampleRate(parsed.sample_rate());
    sound->setLevel(parsed.level());
    sound->setTune(parsed.tune());
    sound->setBeatCount(parsed.beat_count());
    sound->setLoopEnabled(parsed.loop_enabled());

    auto sampleData = sound->getMutableSampleData();
    sampleData->clear();
    if (parsed.sample_data() != nullptr)
    {
        for (const auto frame : *parsed.sample_data())
        {
            sampleData->push_back(short_to_float(frame));
        }
    }

    sound->setStart(parsed.start());
    sound->setEnd(parsed.end());
    sound->setLoopTo(parsed.end() - parsed.loop_frame_count());

    return sound;
}

std::vector<char> SndIo::saveSound(mpc::sampler::Sound &sound)
{
    mpc2000snd_t parsed(nullptr);
    parsed.set_magic(std::string("\x01\x04", 2));
    parsed.set_name(StrUtil::padRight(sound.getName().substr(0, 16), " ", 16) + '\0');
    parsed.set_level(sound.getSndLevel());
    parsed.set_tune(static_cast<int8_t>(sound.getTune()));
    parsed.set_stereo(!sound.isMono());
    parsed.set_start(sound.getStart());
    parsed.set_end(sound.getEnd());
    parsed.set_frame_count(sound.getFrameCount());
    parsed.set_loop_frame_count(sound.getEnd() - sound.getLoopTo());
    parsed.set_loop_enabled(sound.isLoopEnabled());
    parsed.set_beat_count(sound.getBeatCount());
    parsed.set_sample_rate(sound.getSampleRate());

    auto frames = std::make_unique<std::vector<int16_t>>();
    for (const auto sample : *sound.getSampleData())
    {
        frames->push_back(mean_normalized_float_to_short(sample));
    }
    parsed.set_sample_data(std::move(frames));

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    ::kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto written = writeStream.str();
    return std::vector<char>(written.begin(), written.end());
}
