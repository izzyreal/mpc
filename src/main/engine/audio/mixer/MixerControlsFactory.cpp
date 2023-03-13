#include <engine/audio/mixer/MixerControlsFactory.hpp>

#include <engine/audio/core/ChannelFormat.hpp>
#include <engine/audio/mixer/MainMixControls.hpp>
#include <engine/audio/mixer/MixControls.hpp>
#include <engine/audio/mixer/MixerControlsIds.hpp>
#include <engine/control/CompoundControl.hpp>

using namespace ctoot::audio::mixer;
using namespace ctoot::audio::core;
using namespace ctoot::control;
using namespace std;

void MixerControlsFactory::createBusStrips(shared_ptr<MixerControls> mixerControls, string mainStripName,
                                           shared_ptr<ChannelFormat> mainFormat, int nreturns)
{
    auto lMixerControls = mixerControls;
    lMixerControls->createStripControls(MixerControlsIds::MAIN_STRIP, mainStripName);
    auto auxControls = lMixerControls->getAuxBusControls();
    auto naux = static_cast<int>(auxControls.size());
    for (int i = 0; i < naux; i++)
    {
        auto busControls = auxControls[i];
        lMixerControls->createStripControls(MixerControlsIds::AUX_STRIP, busControls->getName(), false);
    }
}

void MixerControlsFactory::createChannelStrips(shared_ptr<MixerControls> mixerControls, int nchannels)
{

    auto mbc = mixerControls->getMainBusControls();
    auto mainFormat = mbc->getChannelFormat();
    for (int i = 0; i < nchannels; i++)
    {
        mixerControls->createStripControls(MixerControlsIds::CHANNEL_STRIP, to_string(1 + i));
    }
}

void MixerControlsFactory::addMixControls(MixerControls *mixerControls, shared_ptr<AudioControlsChain> controls,
                                          bool hasMixControls)
{
    auto lControls = controls;
    int stripId = lControls->getId();

    if (stripId == MixerControlsIds::AUX_STRIP)
    {
        auto busControls = mixerControls->getBusControls(lControls->getName());
        if (busControls)
        {
            lControls->add(make_shared<MixControls>(mixerControls, stripId, busControls, true));
        }
    }

    auto mainMixControls = make_shared<MainMixControls>(mixerControls, stripId,
                                                        mixerControls->getMainBusControls(),
                                                        stripId == MixerControlsIds::MAIN_STRIP);

    if (mixerControls->getMainBusControls()->getId() == MixerControlsIds::MAIN_BUS)
    {
        auto routeControl = mainMixControls->createRouteControl(stripId);

        if (routeControl != nullptr)
        {
            mainMixControls->add(shared_ptr<Control>(routeControl));
        }
    }

    if (stripId != MixerControlsIds::MAIN_STRIP && stripId != MixerControlsIds::AUX_STRIP)
    {
        for (int i = 0; i < mixerControls->getAuxBusControls().size(); i++)
        {
            auto busControls = mixerControls->getAuxBusControls()[i];
            if (hasMixControls)
            {
                lControls->add(make_shared<MixControls>(mixerControls, stripId, busControls, false));
            }
        }
    }

    if (hasMixControls)
    {
        lControls->add(std::move(mainMixControls));
    }
}
