#include "MpcMidiInput.hpp"

#include <Mpc.hpp>
#include <hardware/Hardware.hpp>
#include <hardware/HwSlider.hpp>
#include <audiomidi/EventHandler.hpp>
#include <audiomidi/MpcMidiPorts.hpp>
#include <controls/BaseControls.hpp>
#include <controls/Controls.hpp>
#include <controls/GlobalReleaseControls.hpp>

#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/SyncScreen.hpp>
#include <lcdgui/screens/MidiSwScreen.hpp>
#include <lcdgui/screens/window/MidiInputScreen.hpp>
#include <lcdgui/screens/window/MidiOutputScreen.hpp>

#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/MidiClockEvent.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/Sequencer.hpp>

#include <midi/core/MidiMessage.hpp>
#include <midi/core/ShortMessage.hpp>
#include <midi/core/MidiInput.hpp>

using namespace mpc::audiomidi;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace ctoot::midi::core;
using namespace std;

MpcMidiInput::MpcMidiInput(mpc::Mpc& mpc, int index)
: mpc(mpc)
{
    sequencer = mpc.getSequencer();
    sampler = mpc.getSampler();
    this->index = index;
    midiAdapter = make_unique<mpc::sequencer::MidiAdapter>();
    eventAdapter = make_unique<mpc::sequencer::EventAdapter>(mpc, sequencer);
}

string MpcMidiInput::getName()
{
    return "mpcmidiin" + to_string(index);
}

void MpcMidiInput::transport(MidiMessage* msg, int timeStamp)
{
    eventAdapter->process(msg);
    
    auto status = msg->getStatus();
    auto lSampler = sampler.lock();
    string notify_ = string(index == 0 ? "a" : "b");
    auto shortMsg = dynamic_cast<ShortMessage*>(msg);
    auto channel = shortMsg->getChannel();
    
    notify_ += to_string(channel);
    
    notifyObservers(notify_);
    
    auto isControl = status >= ShortMessage::CONTROL_CHANGE && status < ShortMessage::CONTROL_CHANGE + 16;
    
    if (isControl)
    {
        const auto controller = shortMsg->getData1();
        const auto value = shortMsg->getData2();
        
        // As per the MPC2000XL's MIDI implementation chart
        if (controller == 7)
            mpc.getHardware().lock()->getSlider().lock()->setValue(value);
        
        auto midiInputScreen = mpc.screens->get<MidiInputScreen>("midi-input");
        auto midiSwScreen = mpc.screens->get<MidiSwScreen>("midi-sw");
        auto seq = sequencer.lock();
        
        if (midiInputScreen->getReceiveCh() == -1 ||
            midiInputScreen->getReceiveCh() == channel)
        {
            vector<int> processedControllers;
            
            for (int i = 0; i < 4; i++)
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
                            mpc.getActiveControls().lock()->pad(pad, value, false, -1);
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
                            mpc.getReleaseControls()->simplePad(pad);
                        }
                    }
                }
            }
        }
    }
    
    auto lSequencer = sequencer.lock();
    
    auto isPolyPressure = status >= ShortMessage::POLY_PRESSURE && status < ShortMessage::POLY_PRESSURE + 16;
    auto isNoteOn = status >= ShortMessage::NOTE_ON && status < ShortMessage::NOTE_ON + 16;
    auto isNoteOff = status >= ShortMessage::NOTE_OFF && status < ShortMessage::NOTE_OFF + 16;
    
    if (isPolyPressure ||
        isNoteOn ||
        isNoteOff)
    {
        int note = (*msg->getMessage())[1];
        int velo = (*msg->getMessage())[2];
        auto s = lSequencer->getActiveSequence().lock();
        auto bus = s->getTrack(lSequencer->getActiveTrackIndex()).lock()->getBus();
        
        if (bus != 0)
        {
            auto pgm = lSampler->getDrumBusProgramNumber(bus);
            auto p = lSampler->getProgram(pgm).lock();
            
            auto pad = p->getPadIndexFromNote(note);
            
            if (pad != -1)
            {
                if (isPolyPressure)
                {
                    (*mpc.getControls().lock()->getPressedPadVelos())[pad] = velo;
                }
                else if (isNoteOn)
                {
                    if (velo > 0)
                        mpc.getControls().lock()->getPressedPads()->emplace(pad);
                    else
                        mpc.getControls().lock()->getPressedPads()->erase(pad);
                }
                else if (isNoteOff)
                {
                    mpc.getControls().lock()->getPressedPads()->erase(pad);
                }
            }
        }
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
        auto s = lSequencer->isPlaying() ? lSequencer->getCurrentlyPlayingSequence().lock() : lSequencer->getActiveSequence().lock();
        auto track = dynamic_pointer_cast<mpc::sequencer::Track>(s->getTrack(note->getTrack()).lock());
        auto p = lSampler->getProgram(lSampler->getDrumBusProgramNumber(track->getBus())).lock();
        auto controls = mpc.getActiveControls().lock();
        
        controls->setSliderNoteVar(note.get(), p);
        
        auto pad = p->getPadIndexFromNote(note->getNote());
        
        mpc.setPadAndNote(pad, note->getNote());
        
        mpc.getEventHandler().lock()->handleNoThru(note, track.get(), timeStamp);
        
        if (lSequencer->isRecordingOrOverdubbing())
        {
            note->setDuration(note->getVelocity() == 0 ? 0 : -1);
            note->setTick(lSequencer->getTickPosition());
            
            if (note->getVelocity() == 0)
            {
                mpc::sequencer::NoteEvent& noteOff = *note.get();
                track->recordNoteOff(noteOff);
            }
            else
            {
                auto recEvent = track->recordNoteOn().lock();
                
                if (recEvent)
                    note->CopyValuesTo(recEvent);
            }
        }
        
        auto midiOutputScreen = mpc.screens->get<MidiOutputScreen>("midi-output");
        
        switch (midiOutputScreen->getSoftThru())
        {
            case 0:
                return;
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


void MpcMidiInput::midiOut(weak_ptr<mpc::sequencer::Event> e, mpc::sequencer::Track* track)
{
    auto event = e.lock();
    string notify_ = "";
    auto msg = event->getShortMessage();
    auto deviceNumber = track->getDevice() - 1;
    
    if (deviceNumber != -1 && deviceNumber < 32)
    {
        auto channel = deviceNumber;
        if (channel > 15)
            channel -= 16;
        
        midiAdapter->process(event, channel, -1);
        msg = midiAdapter->get().lock().get();
    }
    
    auto mpcMidiPorts = mpc.getMidiPorts().lock();
    
    notify_ = "a";
    
    if (deviceNumber > 15)
    {
        deviceNumber -= 16;
        notify_ = "b";
    }
    
    if (mpc.getLayeredScreen().lock()->getCurrentScreenName().compare("midi-output-monitor") == 0)
        notifyObservers(notify_ + to_string(deviceNumber));
}

void MpcMidiInput::transportOmni(MidiMessage* msg, string outputLetter)
{
    auto mpcMidiPorts = mpc.getMidiPorts().lock();
    
    if (dynamic_cast<ShortMessage*>(msg) != nullptr)
    {
        if (mpc.getLayeredScreen().lock()->getCurrentScreenName().compare("midi-output-monitor") == 0)
            notifyObservers(string(outputLetter + to_string(dynamic_cast<ShortMessage*>(msg)->getChannel())));
    }
}
