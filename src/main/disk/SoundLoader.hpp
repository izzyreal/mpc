#pragma once

#include <string>
#include <memory>
#include <vector>

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
    
    // If the sound you're attempting to load has already been
    // loaded, this will be filled with the index of the loaded
    // sound. It will be -1 if the sound hasn't already been
    // loaded. Most consumers treat this value as a boolean,
    // except ProgramLoader. It needs to know what sound index
    // a NoteParameters should have.
    
    int existingIndex = -1;
    
    // The LOAD screen will want to display a popup message,
    // like "not 16bit .WAV file", in case SoundLoader is unable to
    // parse a file with a familiar extension. This is where
    // SoundLoader can store the message exactly like how it will
    // be displayed.
    std::string errorMessage = "";
    
    bool canBeConverted = false;
    
    // This will be set to true when everything prior to and including
    // adding a sound went fine. It is used by error handlers to decide
    // whether to remove an added sound or not. See in vmpc-juce the
    // file PadControl.cpp
    bool soundWasAdded = false;
};

class SoundLoader
{
public:
    SoundLoader(mpc::Mpc& mpc);
    SoundLoader(mpc::Mpc& mpc, std::vector<std::weak_ptr<mpc::sampler::Sound>> sounds, bool replace);

    void setPartOfProgram(bool);
    void loadSound(std::shared_ptr<MpcFile>, SoundLoaderResult&, bool shouldBeConverted);
    void setPreview(bool);

private:
    mpc::Mpc& mpc;
    bool partOfProgram = false;
    std::vector<std::weak_ptr<mpc::sampler::Sound>> sounds;
    bool preview = false;
    bool replace = false;
};
}
