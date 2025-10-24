#include "engine/audio/mixer/MainMixProcess.hpp"
#include "engine/audio/mixer/AudioMixer.hpp"
#include "engine/audio/mixer/AudioMixerStrip.hpp"
#include "engine/audio/mixer/MixControls.hpp"
#include "engine/audio/mixer/MainMixControls.hpp"

using namespace mpc::engine::audio::mixer;

MainMixProcess::MainMixProcess(
    const std::shared_ptr<AudioMixerStrip> &strip,
    const std::shared_ptr<MixControls> &mixControls,
    AudioMixer *mixer) : MixProcess(strip, mixControls)
{
    auto mmc = std::dynamic_pointer_cast<MainMixControls>(mixControls);
    routeControl = mmc->getRouteControl();
    if (routeControl != nullptr)
    {
        routedStrip = mixer->getStripImpl(routeControl->getValueString());
    }
}

AudioMixerStrip *MainMixProcess::getRoutedStrip()
{
    if (routeControl != nullptr && nextRoutedStrip != nullptr)
    {
        routedStrip = nextRoutedStrip;
        nextRoutedStrip = std::shared_ptr<AudioMixerStrip>();
    }
    return MixProcess::getRoutedStrip();
}
