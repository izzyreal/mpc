#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "TestMpc.hpp"
#include "client/event/ClientMidiEvent.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientMidiEventController.hpp"
#include "hardware/Hardware.hpp"
#include "input/midi/MidiControlPresetV3.hpp"
#include "performance/PerformanceManager.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/Track.hpp"

using namespace mpc;
using namespace mpc::client::event;
using namespace mpc::input::midi;
using namespace mpc::performance;

namespace
{
    constexpr int midiNumber = 51;
    constexpr int padIndex = 14;

    struct ChannelMatchingFixture
    {
        Mpc mpc;

        ChannelMatchingFixture(const BindingMessageType type,
                               const int bindingChannel)
        {
            TestMpc::initializeTestMpcWithoutIoServices(mpc);
            const auto sequencer = mpc.getSequencer();
            sequencer->getSequence(0)->init(1);
            drain();
            sequencer->getSequence(0)->getTrack(0)->setBusType(
                sequencer::BusType::DRUM1);
            sequencer->setSelectedTrackIndex(TrackIndex(0));

            const auto program =
                mpc.getSampler()
                    ->createNewProgramAddFirstAvailableSlotAndThen({})
                    .lock();
            REQUIRE(program);
            sequencer->getDrumBus(DrumBusIndex(0))
                ->setProgramIndex(program->getProgramIndex());
            drain();

            Binding binding;
            binding.setTarget("hardware:pad-15-or-hyphen-exclamation-mark");
            binding.setMessageType(type);
            binding.setMidiNumber(midiNumber);
            binding.setMidiValue(64);
            binding.setMidiChannelIndex(bindingChannel);
            auto preset = std::make_shared<MidiControlPresetV3>();
            preset->setBindings({binding});
            mpc.clientEventController->getClientMidiEventController()
                ->getExtendedController()
                ->setActivePreset(preset);
        }

        void drain()
        {
            for (int i = 0; i < 3; ++i)
            {
                mpc.getSequencer()->getStateManager()->drainQueue();
                mpc.getPerformanceManager().lock()->drainQueue();
            }
        }

        void send(const ClientMidiEvent::MessageType type, const int channel,
                  const int value)
        {
            ClientMidiEvent event{type, midiNumber, value};
            event.setChannel(channel);
            mpc.clientEventController->getClientMidiEventController()
                ->handleClientMidiEvent(event);
            drain();
        }

        bool hasOrdinaryNote(const int channel)
        {
            return mpc.getPerformanceManager()
                .lock()
                ->getSnapshot()
                .findNoteOnEvent(PerformanceEventSource::MidiInput,
                                 NoteNumber(midiNumber), MidiChannel(channel))
                .has_value();
        }
    };
} // namespace

TEST_CASE(
    "MIDI preset actions match specific channels for presses and releases",
    "[midi-channel-matching]")
{
    const auto type =
        GENERATE(BindingMessageType::Note, BindingMessageType::Controller);
    const int channel = GENERATE(0, 15);
    const int otherChannel = channel == 0 ? 15 : 0;
    ChannelMatchingFixture fixture(type, channel);
    const auto pad = fixture.mpc.getHardware()->getPad(padIndex);
    const auto press = type == BindingMessageType::Note
                           ? ClientMidiEvent::NOTE_ON
                           : ClientMidiEvent::CONTROLLER;
    const auto release = type == BindingMessageType::Note
                             ? ClientMidiEvent::NOTE_OFF
                             : ClientMidiEvent::CONTROLLER;

    fixture.send(press, otherChannel, 127);
    REQUIRE_FALSE(pad->isPressed());
    fixture.send(release, otherChannel, 0);

    fixture.send(press, channel, 127);
    REQUIRE(pad->isPressed());
    fixture.send(release, otherChannel, 0);
    REQUIRE(pad->isPressed());
    fixture.send(release, channel, 0);
    REQUIRE_FALSE(pad->isPressed());
}

TEST_CASE(
    "MIDI preset all-channel bindings accept presses and releases on every "
    "channel",
    "[midi-channel-matching]")
{
    const auto type =
        GENERATE(BindingMessageType::Note, BindingMessageType::Controller);
    ChannelMatchingFixture fixture(type, AllMidiChannels);
    const auto pad = fixture.mpc.getHardware()->getPad(padIndex);
    const auto press = type == BindingMessageType::Note
                           ? ClientMidiEvent::NOTE_ON
                           : ClientMidiEvent::CONTROLLER;
    const auto release = type == BindingMessageType::Note
                             ? ClientMidiEvent::NOTE_OFF
                             : ClientMidiEvent::CONTROLLER;

    for (int channel = 0; channel < 16; ++channel)
    {
        CAPTURE(channel);
        fixture.send(press, channel, 127);
        REQUIRE(pad->isPressed());
        fixture.send(release, channel, 0);
        REQUIRE_FALSE(pad->isPressed());
    }
}

TEST_CASE(
    "MIDI preset poly aftertouch respects specific and all-channel bindings",
    "[midi-channel-matching]")
{
    const int bindingChannel =
        GENERATE(static_cast<int>(AllMidiChannels), 0, 15);
    ChannelMatchingFixture fixture(BindingMessageType::Note, bindingChannel);
    const auto pad = fixture.mpc.getHardware()->getPad(padIndex);
    const int pressChannel =
        bindingChannel == AllMidiChannels ? 0 : bindingChannel;
    fixture.send(ClientMidiEvent::NOTE_ON, pressChannel, 100);
    REQUIRE(pad->isPressed());

    for (int channel = 0; channel < 16; ++channel)
    {
        CAPTURE(bindingChannel, channel);
        const auto previousPressure = pad->getPressure();
        fixture.send(ClientMidiEvent::AFTERTOUCH, channel, 64 + channel);
        if (bindingChannel == AllMidiChannels || bindingChannel == channel)
        {
            REQUIRE(pad->getPressure() == 64 + channel);
        }
        else
        {
            REQUIRE(pad->getPressure() == previousPressure);
        }
    }
    fixture.send(ClientMidiEvent::NOTE_OFF, pressChannel, 0);
}

TEST_CASE("MIDI preset note suppression uses the binding channel",
          "[midi-channel-matching]")
{
    const int bindingChannel =
        GENERATE(static_cast<int>(AllMidiChannels), 0, 15);
    const int channel = GENERATE(0, 15);
    ChannelMatchingFixture fixture(BindingMessageType::Note, bindingChannel);
    const bool matches =
        bindingChannel == AllMidiChannels || bindingChannel == channel;

    fixture.send(ClientMidiEvent::NOTE_ON, channel, 100);
    REQUIRE(fixture.hasOrdinaryNote(channel) == !matches);
    fixture.send(ClientMidiEvent::NOTE_OFF, channel, 0);
    REQUIRE_FALSE(fixture.hasOrdinaryNote(channel));
}
