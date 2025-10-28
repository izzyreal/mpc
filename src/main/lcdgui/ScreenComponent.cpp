#include "ScreenComponent.hpp"

#include "Background.hpp"

#include "Mpc.hpp"

#include "command/context/PushPadScreenUpdateContext.hpp"
#include "lcdgui/screens/dialog/MetronomeSoundScreen.hpp"
#include "lcdgui/screens/dialog/MidiOutputMonitorScreen.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Track.hpp"

#include "lcdgui/ScreenGroups.hpp"
#include "lcdgui/screens/DrumScreen.hpp"

#include <stdexcept>

using namespace mpc::lcdgui;
using namespace mpc::command;
using namespace mpc::command::context;
using namespace mpc::sequencer;

ScreenComponent::ScreenComponent(mpc::Mpc &mpc, const std::string &name,
                                 const int layer)
    : Component(name), layer(layer), mpc(mpc), ls(mpc.getLayeredScreen()),
      sampler(mpc.getSampler()), sequencer(mpc.getSequencer())
{
    auto background = std::dynamic_pointer_cast<Background>(
        addChild(std::make_shared<Background>()));
    background->setBackgroundName(name);
}

void ScreenComponent::setTransferMap(
    const std::map<std::string, std::vector<std::string>> &newTransferMap)
{
    transferMap = newTransferMap;
}

std::map<std::string, std::vector<std::string>> &
ScreenComponent::getTransferMap()
{
    return transferMap;
}

void ScreenComponent::setFirstField(const std::string &newFirstField)
{
    firstField = newFirstField;
}

std::string ScreenComponent::getFirstField()
{
    return firstField;
}

std::shared_ptr<Wave> ScreenComponent::findWave()
{
    return findChild<Wave>("wave");
}

std::shared_ptr<EnvGraph> ScreenComponent::findEnvGraph()
{
    return findChild<EnvGraph>("env-graph");
}

const int &ScreenComponent::getLayerIndex()
{
    return layer;
}

std::shared_ptr<Field> ScreenComponent::findFocus()
{
    for (auto &field : findFields())
    {
        if (field->hasFocus())
        {
            return field;
        }
    }
    return {};
}

void ScreenComponent::setLastFocus(const std::string &screenName,
                                   const std::string &newLastFocus)
{
    mpc.getLayeredScreen()->setLastFocus(screenName, newLastFocus);
}

const std::string ScreenComponent::getLastFocus(const std::string &screenName)
{
    return mpc.getLayeredScreen()->getLastFocus(screenName);
}

void ScreenComponent::openWindow()
{
    /**
     * On the real MPC2000XL the OPEN WINDOW button can also be used to close
     * windows. The behaviour is consistent for nearly all cases, with a few
     * exceptions. See https://github.com/izzyreal/mpc/issues/260
     */
    if (dynamic_cast<MidiOutputMonitorScreen *>(this))
    {
        return;
    }
    else if (dynamic_cast<MetronomeSoundScreen *>(this))
    {
        PushMainScreenCommand(mpc).execute();
        return;
    }

    mpc.getLayeredScreen()->closeCurrentScreen();
}

std::optional<int> ScreenComponent::getDrumIndex()
{
    if (screengroups::isSamplerScreen(ls->getCurrentScreen()))
    {
        const auto drumScreen =
            mpc.screens->get<mpc::lcdgui::screens::DrumScreen>();
        return drumScreen->getDrum();
    }

    if (const int drumIndex =
            mpc.getSequencer()->getActiveTrack()->getBus() - 1;
        drumIndex >= 0)
    {
        return drumIndex;
    }

    return std::nullopt;
}

std::shared_ptr<DrumBus> ScreenComponent::getActiveDrumBus()
{
    const auto drumIndex = getDrumIndex();
    assert(drumIndex);
    return sequencer->getDrumBus(*drumIndex);
}

std::shared_ptr<Field> ScreenComponent::getFocusedField()
{
    for (auto &f : findFields())
    {
        if (f->hasFocus())
        {
            return f;
        }
    }

    return {};
}

std::optional<std::string> ScreenComponent::getFocusedFieldName()
{
    if (auto f = getFocusedField(); f)
    {
        return f->getName();
    }

    return std::nullopt;
}

std::shared_ptr<Field> ScreenComponent::getFocusedFieldOrThrow()
{
    auto f = getFocusedField();
    if (!f)
    {
        throw std::runtime_error("Expected focused field");
    }
    return f;
}

std::string ScreenComponent::getFocusedFieldNameOrThrow()
{
    return getFocusedFieldOrThrow()->getName();
}

std::shared_ptr<mpc::sampler::Program> ScreenComponent::getProgram()

{
    const auto drumIndex = getDrumIndex();

    if (!drumIndex)
    {
        return {};
    }

    return mpc.getSampler()->getProgram(sequencer->getDrumBus(*drumIndex)->getProgram());
}

std::shared_ptr<mpc::sampler::Program> ScreenComponent::getProgramOrThrow()

{
    auto p = getProgram();
    if (!p)
    {
        throw std::runtime_error("Expected program");
    }
    return p;
}
