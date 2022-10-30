#include "MpcMidiInput.hpp"

#include <Mpc.hpp>
#include <Util.hpp>
#include <hardware/Hardware.hpp>
#include <hardware/DataWheel.hpp>
#include <hardware/HwPad.hpp>
#include <hardware/HwSlider.hpp>
#include <hardware/Button.hpp>
#include <hardware/Pot.hpp>
#include <audiomidi/EventHandler.hpp>
#include <audiomidi/MpcMidiPorts.hpp>
#include <controls/GlobalReleaseControls.hpp>

#include <lcdgui/screens/SyncScreen.hpp>
#include <lcdgui/screens/VmpcSettingsScreen.hpp>
#include <lcdgui/screens/MidiSwScreen.hpp>
#include <lcdgui/screens/VmpcMidiScreen.hpp>
#include <lcdgui/screens/window/MidiInputScreen.hpp>
#include <lcdgui/screens/window/MidiOutputScreen.hpp>

#include <sequencer/Event.hpp>
#include <sequencer/MidiClockEvent.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>

#include "sampler/Pad.hpp"

#include <midi/core/MidiMessage.hpp>
#include <midi/core/ShortMessage.hpp>

using namespace mpc::audiomidi;
using namespace mpc::sequencer;
using namespace mpc::sampler;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace ctoot::midi::core;
using namespace std;

MpcMidiInput::MpcMidiInput(mpc::Mpc &_mpc, int _index)
    : mpc(_mpc),
      sequencer(_mpc.getSequencer()),
      sampler(_mpc.getSampler()),
      index(_index),
      midiAdapter(make_unique<MidiAdapter>()),
      eventAdapter(make_unique<EventAdapter>(mpc, sequencer))
{
}

string MpcMidiInput::getName()
{
  return "mpcmidiin" + to_string(index);
}

void MpcMidiInput::transport(MidiMessage *msg, int timeStamp)
{
  eventAdapter->process(msg);

  auto status = msg->getStatus();
  auto lSampler = sampler.lock();
  string notificationMessage = string(index == 0 ? "a" : "b");
  auto shortMsg = dynamic_cast<ShortMessage *>(msg);

  handleVmpcMidi(shortMsg);

  auto channel = shortMsg->getChannel();

  notificationMessage += to_string(channel);

  notifyObservers(notificationMessage);

  auto lSequencer = sequencer.lock();

  auto isControl = status >= ShortMessage::CONTROL_CHANGE && status < ShortMessage::CONTROL_CHANGE + 16;

  if (isControl)
  {
    handleControl(shortMsg);
  }
  else
  {
    handlePolyAndNote(msg);
  }

  auto event = eventAdapter->get().lock();

  if (!event)
    return;

  auto mce = dynamic_pointer_cast<mpc::sequencer::MidiClockEvent>(event);
  auto note = dynamic_pointer_cast<mpc::sequencer::NoteEvent>(event);

  auto syncScreen = mpc.screens->get<SyncScreen>("sync");

  if (mce && syncScreen->in == index)
  {
    if (syncScreen->getModeIn() != 0)
    {
      switch (mce->getStatus())
      {
        case ShortMessage::START:
          lSequencer->playFromStart();
          break;
        case ShortMessage::STOP:
          lSequencer->stop();
          break;
        case ShortMessage::TIMING_CLOCK:
          break;
      }

    }
  }
  else if (note)
  {
    note->setTick(-1);
    auto s = lSequencer->isPlaying() ? lSequencer->getCurrentlyPlayingSequence()
                                     : lSequencer->getActiveSequence();
    auto track = dynamic_pointer_cast<mpc::sequencer::Track>(s->getTrack(note->getTrack()));
    auto p = lSampler->getProgram(lSampler->getDrumBusProgramIndex(track->getBus())).lock();
    auto controls = mpc.getActiveControls().lock();

    Util::setSliderNoteVariationParameters(mpc, note, p);

    auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>("vmpc-settings");
    auto iRigPads = vmpcSettingsScreen->initialPadMapping == 2;
    auto indexInIrigPadsMappingIt = iRigPads ? std::find(Pad::iRigPadsDefaultMapping.begin(),
                                                    Pad::iRigPadsDefaultMapping.end(),
                                                    note->getNote()) : Pad::iRigPadsDefaultMapping.end();

    int indexInIrigPadsMapping = indexInIrigPadsMappingIt == Pad::iRigPadsDefaultMapping.end() ? -1 :
            (indexInIrigPadsMappingIt - Pad::iRigPadsDefaultMapping.begin());

    auto pad = indexInIrigPadsMapping >= 0 ? indexInIrigPadsMapping : p->getPadIndexFromNote(note->getNote());

    if (note->getVelocity() != 0 && track->getBus() > 0 && track->getIndex() < 64 && mpc.getControls().lock()->isTapPressed() && lSequencer->isPlaying())
    {
        return;
    }

    if (pad == -1)
    {
        mpc.getEventHandler().lock()->handleNoThru(note, track.get(), timeStamp);

        if (lSequencer->isRecordingOrOverdubbing())
        {
            note->setDuration(note->getVelocity() == 0 ? 0 : -1);
            note->setTick(lSequencer->getTickPosition());

            if (note->getVelocity() == 0)
            {
                track->recordNoteOffNow(note->getNote());
            }
            else
            {
                auto recordedEvent = track->recordNoteOnNow(note->getNote());
                note->CopyValuesTo(recordedEvent);
            }
        }
    }
    else
    {
        if (note->getVelocity() == 0)
        {
            mpc.getReleaseControls()->simplePad(pad);
        }
        else
        {
            mpc.getActiveControls().lock()->pad(pad, note->getVelocity());
        }
    }

    auto midiOutputScreen = mpc.screens->get<MidiOutputScreen>("midi-output");

    switch (midiOutputScreen->getSoftThru())
    {
      case 0:
        break;
      case 1:
        midiOut(eventAdapter->get(), track.get());
        break;
      case 2:
        transportOmni(msg, "a");
        break;
      case 3:
        transportOmni(msg, "b");
        break;
      case 4:
        transportOmni(msg, "a");
        transportOmni(msg, "b");
        break;
    }
  }
}

void MpcMidiInput::handleControl(ShortMessage *shortMsg)
{
  const auto controller = shortMsg->getData1();
  const auto value = shortMsg->getData2();

  auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>("vmpc-settings");
  auto iRigPads = vmpcSettingsScreen->initialPadMapping == 2;

  if (iRigPads)
  {
      // Default slider controller
      if (controller == 1)
      {
          mpc.getHardware().lock()->getSlider().lock()->setValue(127 - value);
      }
      // Default DATA controller
      else if (controller == 7)
      {
          if (previousDataWheelValue == -1)
          {
              previousDataWheelValue = value;
          }

          auto dataWheel = mpc.getHardware().lock()->getDataWheel().lock();

          if (previousDataWheelValue == 0 && value == 0)
          {
                dataWheel->turn(-1);
          }
          else if (previousDataWheelValue == 127 && value == 127)
          {
              dataWheel->turn(1);
          }
          else if (value - previousDataWheelValue != 0)
          {
              dataWheel->turn(value - previousDataWheelValue);
          }
          previousDataWheelValue = value;
      }
      // Default rotary #1
      else if (controller == 10)
      {
          mpc.getHardware().lock()->getVolPot().lock()->setValue((100 / 127.f) * value);
      }
      // Default rotary #2
      else if (controller == 11)
      {
          mpc.getHardware().lock()->getRecPot().lock()->setValue((100 / 127.f) * value);
      }
      // DATA PUSH
      else if (controller == 22)
      {
          std::string buttonName = sequencer.lock()->isPlaying() ? "stop" : "play-start";
          if (value == 127)
          {
              mpc.getHardware().lock()->getButton(buttonName).lock()->push();
          }
          else
          {
              mpc.getHardware().lock()->getButton(buttonName).lock()->release();
          }
      }
  }
  else {
      // As per the MPC2000XL's MIDI implementation chart
      if (controller == 7) {
          // It looks like the internal implementation of the slider is inverted.
          // At least, the Akai MPD16 sends value 0 at the bottom of the slider
          // and 127 at the top.
          // For now we're safe to simply invert the input, but it would be nice
          // to make this congruent with the MPD16 (assuming it's the same for
          // other controllers, but it would be nice to verify some).
          mpc.getHardware().lock()->getSlider().lock()->setValue(127 - value);
      }
  }

  auto midiInputScreen = mpc.screens->get<MidiInputScreen>("midi-input");
  auto midiSwScreen = mpc.screens->get<MidiSwScreen>("midi-sw");
  auto seq = sequencer.lock();

  if (midiInputScreen->getReceiveCh() == -1 ||
      midiInputScreen->getReceiveCh() == shortMsg->getChannel())
  {
    vector<int> processedControllers;

    for (int i = 0; i < MidiSwScreen::SWITCH_COUNT; i++)
    {
      auto ctrl = midiSwScreen->getSwitch(i).first;

      if (ctrl == 0)
        continue;

      ctrl -= 1;

      if (find(begin(processedControllers), end(processedControllers), ctrl) != end(processedControllers))
        continue;

      processedControllers.push_back(ctrl);

      auto func = midiSwScreen->getSwitch(i).second;

      if (ctrl == controller)
      {
        if (value >= 64)
        {
          if (func == 0)
          {
            seq->playFromStart();
          }
          else if (func == 1)
          {
            seq->play();
          }
          else if (func == 2)
          {
            seq->stop();
          }
          else if (func == 3)
          {
            seq->rec();
          }
          else if (func == 4)
          {
            seq->overdub();
          }
          else if (func == 5)
          {
            if (seq->isRecording())
            {
              seq->setRecording(false);
            }
            else if (seq->isPlaying())
            {
              seq->setRecording(true);
            }
            else
            {
              seq->play();
            }
          }
          else if (func == 6)
          {
            if (seq->isOverDubbing())
            {
              seq->setOverdubbing(false);
            }
            else if (seq->isPlaying())
            {
              seq->setOverdubbing(true);
            }
            else
            {
              seq->play();
            }
          }
          else if (func == 7)
          {
            mpc.getActiveControls().lock()->tap();
          }
          else if (func >= 8 && func < 12)
          {
            auto bank = func - 8;
            mpc.setBank(bank);
          }
          else if (func >= 12 && func < 28)
          {
            auto pad = func - 12;
            mpc.getHardware().lock()->getPad(pad).lock()->push(value);
          }
        }
        else // value < 64
        {
          if (func == 7)
          {
            mpc.getReleaseControls()->tap();
          }
          else if (func >= 12 && func < 28)
          {
            auto pad = func - 12;
            mpc.getHardware().lock()->getPad(pad).lock()->release();
          }
        }
      }
    }
  }
}

void MpcMidiInput::midiOut(weak_ptr<mpc::sequencer::Event> e, mpc::sequencer::Track *track)
{
  auto event = e.lock();
  string notify_;
  auto deviceNumber = track->getDevice() - 1;

  if (deviceNumber != -1 && deviceNumber < 32)
  {
    auto channel = deviceNumber;

    if (channel > 15)
      channel -= 16;

    midiAdapter->process(event, channel, -1);
  }

  auto mpcMidiPorts = mpc.getMidiPorts().lock();

  notify_ = "a";

  if (deviceNumber > 15)
  {
    deviceNumber -= 16;
    notify_ = "b";
  }

  if (mpc.getLayeredScreen().lock()->getCurrentScreenName().compare("midi-output-monitor") == 0)
  {
    notifyObservers(notify_ + to_string(deviceNumber));
  }
}

void MpcMidiInput::transportOmni(MidiMessage *msg, string outputLetter)
{
  auto mpcMidiPorts = mpc.getMidiPorts().lock();
  auto screenName = mpc.getLayeredScreen().lock()->getCurrentScreenName();

  if (dynamic_cast<ShortMessage *>(msg) != nullptr && screenName == "midi-output-monitor")
  {
    notifyObservers(string(outputLetter + to_string(dynamic_cast<ShortMessage *>(msg)->getChannel())));
  }
}

void MpcMidiInput::handlePolyAndNote(MidiMessage* msg)
{
  auto s = sequencer.lock()->getActiveSequence();
  auto t = s->getTrack(sequencer.lock()->getActiveTrackIndex());

  auto bus = t->getBus();

  if (bus != 0)
  {
    auto status = msg->getStatus();
    auto isChannelPressure = status >= ShortMessage::CHANNEL_PRESSURE && status < ShortMessage::CHANNEL_PRESSURE + 16;
    
    if (isChannelPressure)
    {
      for (auto& p : mpc.getHardware().lock()->getPads())
      {
          if (p->isPressed())
          {
              p->setPressure((*msg->getMessage())[1]);
          }
      }
      return;
    }

    int note = (*msg->getMessage())[1];
    int velo = (*msg->getMessage())[2];

    auto pgm = sampler.lock()->getDrumBusProgramIndex(bus);
    auto p = sampler.lock()->getProgram(pgm).lock();

      auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>("vmpc-settings");
      auto iRigPads = vmpcSettingsScreen->initialPadMapping == 2;

      auto indexInIrigPadsMappingIt = iRigPads ? std::find(Pad::iRigPadsDefaultMapping.begin(),
                                                           Pad::iRigPadsDefaultMapping.end(),
                                                           note) : Pad::iRigPadsDefaultMapping.end();

      int indexInIrigPadsMapping = indexInIrigPadsMappingIt == Pad::iRigPadsDefaultMapping.end() ? -1 :
                                   (indexInIrigPadsMappingIt - Pad::iRigPadsDefaultMapping.begin());

      auto padIndexWithBank = indexInIrigPadsMapping >= 0 ? indexInIrigPadsMapping : p->getPadIndexFromNote(note);

    if (padIndexWithBank != -1)
    {
      auto hwPad = mpc.getHardware().lock()->getPad(padIndexWithBank % 16).lock();
      auto isPolyPressure = status >= ShortMessage::POLY_PRESSURE && status < ShortMessage::POLY_PRESSURE + 16;
      auto isNoteOn = status >= ShortMessage::NOTE_ON && status < ShortMessage::NOTE_ON + 16;
      auto isNoteOff = status >= ShortMessage::NOTE_OFF && status < ShortMessage::NOTE_OFF + 16;

      if (isPolyPressure && hwPad->getPadIndexWithBankWhenLastPressed() == padIndexWithBank)
      {
        hwPad->setPressure(velo);
      }
      else if (isNoteOn)
      {
        if (velo > 0)
        {
          hwPad->setPadIndexWithBankWhenLastPressed(padIndexWithBank);
          hwPad->setPressure(velo);
        }
        else
        {
          hwPad->setPadIndexWithBankWhenLastPressed(-1);
          hwPad->setPressure(0);
        }
      }
      else if (isNoteOff)
      {
        hwPad->setPadIndexWithBankWhenLastPressed(-1);
        hwPad->setPressure(0);
      }
    }
  }
}

void MpcMidiInput::handleVmpcMidi(ctoot::midi::core::ShortMessage *shortMsg)
{
    auto status = shortMsg->getStatus();
    auto isControl = status >= ShortMessage::CONTROL_CHANGE && status < ShortMessage::CONTROL_CHANGE + 16;
    auto isNoteOn = status >= ShortMessage::NOTE_ON && status < ShortMessage::NOTE_ON + 16;
    auto isNoteOff = status >= ShortMessage::NOTE_OFF && status < ShortMessage::NOTE_OFF + 16;

    if (!isNoteOn && !isNoteOff && !isControl)
    {
        return;
    }

    auto vmpcMidiScreen = mpc.screens->get<VmpcMidiScreen>("vmpc-midi");

    if (vmpcMidiScreen->isLearning())
    {
        if (!isNoteOn && !isControl)
        {
            return;
        }
        vmpcMidiScreen->setLearnCandidate(isNoteOn, shortMsg->getChannel(), shortMsg->getData1());
        return;
    }

    for (auto& labelCommand : vmpcMidiScreen->labelCommands)
    {
        auto command = labelCommand.second;
        auto channelIndex = command.channelIndex;

        if (channelIndex >= 0 && shortMsg->getChannel() != channelIndex)
        {
            continue;
        }

        auto label = labelCommand.first;
        auto isNote = command.isNote;
        auto value = command.value;

        if (shortMsg->getData1() != value)
        {
            continue;
        }

        auto hwComponent = mpc.getHardware().lock()->getComponentByLabel(label).lock();

        if ((isNote && (isNoteOn || isNoteOff)) ||
            (!isNote && isControl))
        {
            if (shortMsg->getData2() == 0)
            {
                hwComponent->release();
            }
            else
            {
                hwComponent->push(shortMsg->getData2());
                hwComponent->push();
            }
        }
    }
}
