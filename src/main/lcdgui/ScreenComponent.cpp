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

std::optional<int> ScreenComponent::getDrumIndex() const
{
    if (screengroups::isSamplerScreen(ls->getCurrentScreen()))
    {
        const auto drumScreen = mpc.screens->get<ScreenId::DrumScreen>();
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

std::shared_ptr<Bus> ScreenComponent::getBus() const
{
    if (const auto drumIndex = getDrumIndex(); drumIndex)
    {
        return sequencer->getDrumBus(*drumIndex);
    }

    // We should check if the real MPC2000XL records notes while we're
    // in one of the sampler screens (PGM ASSIGN, PGM PARAMS, etc.),
    // or the MIXER screens, because in those screens, we've explicitly
    // selected a DRUM bus to work with. It might still record to the
    // active track, but I'm not sure.
    return sequencer->getBus<MidiBus>(0);
}

std::shared_ptr<DrumBus> ScreenComponent::getActiveDrumBus() const
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
    if (const auto f = getFocusedField(); f)
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

bool ScreenComponent::isFocusedFieldName(const std::string &fieldName)
{
    const auto f = getFocusedField();

    if (!f)
    {
        return false;
    }

    return f->getName() == fieldName;
}
std::optional<int> ScreenComponent::getProgramIndex() const
{
    const auto drumIndex = getDrumIndex();

    if (!drumIndex)
    {
        return std::nullopt;
    }

    return sequencer->getDrumBus(*drumIndex)->getProgram();
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

void ScreenComponent::function(int i)
{
    if (i == 3)
    {
        mpc.getLayeredScreen()->closeCurrentScreen();
    }
}

void ScreenComponent::numpad(int i)
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
