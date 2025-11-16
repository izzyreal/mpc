#include "ScreenComponent.hpp"

#include "Background.hpp"

#include "Mpc.hpp"

#include "sequencer/Bus.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/Sequencer.hpp"

#include "sampler/Sampler.hpp"

#include "command/context/PushPadScreenUpdateContext.hpp"
#include "command/AllCommands.hpp"

#include "lcdgui/ScreenGroups.hpp"
#include "lcdgui/screens/dialog/MetronomeSoundScreen.hpp"
#include "lcdgui/screens/dialog/MidiOutputMonitorScreen.hpp"
#include "lcdgui/screens/DrumScreen.hpp"

#include <stdexcept>

using namespace mpc::lcdgui;
using namespace mpc::command;
using namespace mpc::command::context;
using namespace mpc::sequencer;
using namespace mpc::sampler;

ScreenComponent::ScreenComponent(Mpc &mpc, const std::string &name,
                                 const int layer)
    : Component(name), layer(layer), mpc(mpc), sampler(mpc.getSampler()),
      sequencer(mpc.getSequencer()), ls(mpc.getLayeredScreen())
{
    const auto background = std::dynamic_pointer_cast<Background>(
        addChild(std::make_shared<Background>()));
    background->setBackgroundName(name);
}

void ScreenComponent::openScreenById(const ScreenId screenId) const
{
    ls->openScreenById(screenId);
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

const int &ScreenComponent::getLayerIndex() const
{
    return layer;
}

std::shared_ptr<Field> ScreenComponent::findFocus() const
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
                                   const std::string &newLastFocus) const
{
    mpc.getLayeredScreen()->setLastFocus(screenName, newLastFocus);
}

std::string ScreenComponent::getLastFocus(const std::string &screenName) const
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
    if (dynamic_cast<MetronomeSoundScreen *>(this))
    {
        PushMainScreenCommand(mpc).execute();
        return;
    }

    mpc.getLayeredScreen()->closeCurrentScreen();
}

std::shared_ptr<Bus> ScreenComponent::getBus() const
{
    if (screengroups::isSamplerScreen(ls->getCurrentScreen()))
    {
        const auto drumScreen = mpc.screens->get<ScreenId::DrumScreen>();
        return sequencer->getBus(drumScreen->getDrum());
    }

    return mpc.getSequencer()->getBus(
        mpc.getSequencer()->getSelectedTrack()->getBusType());
}

std::shared_ptr<DrumBus> ScreenComponent::getActiveDrumBus() const
{
    auto result = std::dynamic_pointer_cast<DrumBus>(getBus());
    assert(result);
    return result;
}

std::shared_ptr<Field> ScreenComponent::getFocusedField() const
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

std::optional<std::string> ScreenComponent::getFocusedFieldName() const
{
    if (const auto f = getFocusedField(); f)
    {
        return f->getName();
    }

    return std::nullopt;
}

std::shared_ptr<Field> ScreenComponent::getFocusedFieldOrThrow() const
{
    auto f = getFocusedField();
    if (!f)
    {
        throw std::runtime_error("Expected focused field");
    }
    return f;
}

std::string ScreenComponent::getFocusedFieldNameOrThrow() const
{
    return getFocusedFieldOrThrow()->getName();
}

bool ScreenComponent::isFocusedFieldName(const std::string &fieldName) const
{
    const auto f = getFocusedField();

    if (!f)
    {
        return false;
    }

    return f->getName() == fieldName;
}
std::optional<mpc::ProgramIndex> ScreenComponent::getProgramIndex() const
{
    const auto bus = getBus();
    const auto drumBus = std::dynamic_pointer_cast<DrumBus>(bus);

    if (!drumBus)
    {
        return std::nullopt;
    }

    return drumBus->getProgramIndex();
}

std::shared_ptr<Program> ScreenComponent::getProgram() const
{
    const auto programIndex = getProgramIndex();

    if (!programIndex)
    {
        return {};
    }

    return mpc.getSampler()->getProgram(*programIndex);
}

std::shared_ptr<Program> ScreenComponent::getProgramOrThrow() const
{
    auto p = getProgram();
    if (!p)
    {
        throw std::runtime_error("Expected program");
    }
    return p;
}

void ScreenComponent::left()
{
    PushLeftCommand(mpc).execute();
}
void ScreenComponent::right()
{
    PushRightCommand(mpc).execute();
}

void ScreenComponent::up()
{
    PushUpCommand(mpc).execute();
}

void ScreenComponent::down()
{
    PushDownCommand(mpc).execute();
}

void ScreenComponent::function(const int i)
{
    if (i == 3)
    {
        mpc.getLayeredScreen()->closeCurrentScreen();
    }
}

void ScreenComponent::numpad(const int i)
{
    PushNumPadCommand(mpc, i).execute();
}
void ScreenComponent::pressEnter()
{
    PushEnterCommand(mpc).execute();
}
void ScreenComponent::rec()
{
    PushRecCommand(mpc).execute();
}
void ScreenComponent::overDub()
{
    PushOverdubCommand(mpc).execute();
}
void ScreenComponent::stop()
{
    PushStopCommand(mpc).execute();
}
void ScreenComponent::play()
{
    PushPlayCommand(mpc).execute();
}
void ScreenComponent::playStart()
{
    PushPlayStartCommand(mpc).execute();
}

int ScreenComponent::getSoundIncrement(const int dataWheelSteps) const
{
    auto result = dataWheelSteps;

    if (std::abs(result) != 1)
    {
        result *= static_cast<int>(
            ceil(mpc.getSampler()->getSound()->getFrameCount() / 15000.f));
    }

    return result;
}
