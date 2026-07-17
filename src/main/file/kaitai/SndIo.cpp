#include "file/kaitai/SndIo.hpp"

#include "SampleOps.hpp"
#include "StrUtil.hpp"
#include "disk/MpcFile.hpp"
#include "file/kaitai/KaitaiIoUtil.hpp"
#include "file/kaitai/Mpc60SampleDecoder.hpp"
#include "file/kaitai/generated/mpc2000snd.h"
#include "file/kaitai/generated/mpc3000_snd_v2.h"
#include "file/kaitai/generated/mpc60_snd_v1.h"
#include "sampler/Sound.hpp"

using namespace mpc::file::kaitai;
using namespace mpc::file::kaitai::generated;
using namespace mpc::sampleops;

namespace
{
    std::string trimRightSpaces(std::string value)
    {
        while (!value.empty() && value.back() == ' ')
        {
            value.pop_back();
        }
        return value;
    }

    std::string parsedSoundName(const std::string &parsedName)
    {
        return trimRightSpaces(parsedName);
    }
}

sound_or_error SndIo::loadBytes(const std::vector<char> &bytes,
                                const std::shared_ptr<mpc::sampler::Sound> &sound,
                                const std::string &)
{
    if (bytes.size() < 2)
    {
        return tl::make_unexpected("SND file is too short");
    }

    const auto firstByte = static_cast<unsigned char>(bytes[0]);
    const auto secondByte = static_cast<unsigned char>(bytes[1]);

    std::stringstream parseStream(
        std::string(bytes.begin(), bytes.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    ::kaitai::kstream parseIo(&parseStream);

    if (firstByte == 0x01 && secondByte == 0x04)
    {
        mpc2000snd_t parsed(&parseIo);
        parsed._read();

        auto parsedName = parsed.name();
        const auto nulPos = parsedName.find('\0');
        if (nulPos != std::string::npos)
        {
            parsedName.erase(nulPos);
        }

        sound->setName(parsedSoundName(parsedName));
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

    if (firstByte == 0x01 && secondByte == 0x01)
    {
        mpc60_snd_v1_t parsed(&parseIo);

        sound->setName(parsedSoundName(parsed.name()));
        sound->setMono(true);
        sound->setSampleRate(parsed.sample_rate());
        sound->setLevel(parsed.volume_percent());
        sound->setTune(parsed.tuning());
        sound->setBeatCount(4);
        sound->setLoopEnabled(false);

        auto sampleData = sound->getMutableSampleData();
        sampleData->clear();
        sampleData->reserve(parsed.sample_count());

        Mpc60SampleDecoder decoder;
        size_t decodedCount = 0;
        for (const auto &pair : *parsed.sample_data_pairs())
        {
            if (decodedCount < parsed.sample_count())
            {
                sampleData->push_back(
                    decoder.decodeImportedFloat(pair->sample0_code(), false));
                ++decodedCount;
            }

            if (decodedCount < parsed.sample_count())
            {
                sampleData->push_back(
                    decoder.decodeImportedFloat(pair->sample1_code(), true));
                ++decodedCount;
            }
        }

        sound->setStart(0);
        sound->setEnd(sound->getLastFrameIndex());
        sound->setLoopTo(0);

        return sound;
    }

    if (firstByte == 0x01 && secondByte == 0x02)
    {
        mpc3000_snd_v2_t parsed(&parseIo);
        parsed._read();

        sound->setName(parsedSoundName(parsed.name()));
        sound->setMono(true);
        sound->setSampleRate(parsed.sample_rate());
        sound->setLevel(parsed.level());
        sound->setTune(0);
        sound->setBeatCount(4);
        sound->setLoopEnabled(false);

        auto sampleData = sound->getMutableSampleData();
        sampleData->clear();
        if (parsed.sample_data() != nullptr)
        {
            sampleData->reserve(parsed.sample_data()->size());
            for (const auto sample : *parsed.sample_data())
            {
                sampleData->push_back(short_to_float(sample));
            }
        }

        sound->setStart(parsed.start());
        sound->setEnd(parsed.end());
        sound->setLoopTo(0);

        return sound;
    }

    return tl::make_unexpected("Unsupported SND file format");
}

sound_or_error SndIo::loadSound(const std::shared_ptr<mpc::disk::MpcFile> &file,
                                const std::shared_ptr<mpc::sampler::Sound> &sound,
                                const std::string &nameWithoutExtension)
{
    return loadBytes(file->getBytes(), sound, nameWithoutExtension);
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
