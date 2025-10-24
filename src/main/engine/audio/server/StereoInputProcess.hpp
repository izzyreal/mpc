#include <engine/audio/server/AudioServerProcess.hpp>

#include <engine/audio/core/AudioBuffer.hpp>

namespace mpc::engine::audio::server
{

    class StereoInputProcess
        : public AudioServerProcess
    {

    public:
        int processAudio(mpc::engine::audio::core::AudioBuffer *buffer, int nFrames) override;

        StereoInputProcess(std::string name);
    };

} // namespace mpc::engine::audio::server
