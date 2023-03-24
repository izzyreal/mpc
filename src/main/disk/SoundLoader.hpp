#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include "mpc_types.hpp"

namespace moduru::file { class File; }
namespace mpc { class Mpc; }
namespace mpc::file::wav { class WavFile; }
namespace mpc::sampler { class Sound; }
namespace mpc::disk { class MpcFile; }

namespace mpc::disk
{

struct SoundLoaderResult {
    
    // Set to true if the file could not be parsed.
    bool success = false;
    
    // The LOAD screen will want to display a popup message,
    // like "not 16bit .WAV file", in case SoundLoader is unable to
    // parse a file with a familiar extension. This is where
    // SoundLoader can store the message exactly like how it will
    // be displayed.
    std::string errorMessage;
    
    bool canBeConverted = false;
};

class SoundLoader
{
public:
    SoundLoader(mpc::Mpc& mpcToUse, bool replaceToUse);

    void loadSound(std::shared_ptr<MpcFile>, SoundLoaderResult&, std::shared_ptr<mpc::sampler::Sound>, const bool shouldBeConverted);
    void setPreview(bool);

private:
    mpc::Mpc& mpc;
    bool preview = false;
    bool replace = false;

    sound_or_error onReadWavSuccess(
            std::shared_ptr<mpc::file::wav::WavFile>&,
                    std::string filenameWithoutExtension,
                    std::shared_ptr<mpc::sampler::Sound> sound,
                    const bool shouldBeConverted);
};
}
