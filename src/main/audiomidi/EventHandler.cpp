#include "EventHandler.hpp"

#include <Mpc.hpp>
#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/MpcMidiPorts.hpp>

#include <sequencer/Event.hpp>
#include <sequencer/FrameSeq.hpp>
#include <sequencer/MidiClockEvent.hpp>
#include <sequencer/MixerEvent.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/TempoChangeEvent.hpp>

#include <ui/Uis.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <ui/midisync/MidiSyncGui.hpp>
#include <ui/misc/TransGui.hpp>
#include <ui/vmpc/DirectToDiskRecorderGui.hpp>

#include <hardware/Hardware.hpp>
#include <hardware/HwPad.hpp>

#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/MixerSetupScreen.hpp>
#include <lcdgui/screens/window/CountMetronomeScreen.hpp>

#include <midi/core/MidiMessage.hpp>
#include <midi/core/ShortMessage.hpp>
#include <midi/core/MidiInput.hpp>

#include <audio/server/NonRealTimeAudioServer.hpp>

#include <mpc/MpcStereoMixerChannel.hpp>
#include <mpc/MpcMultiMidiSynth.hpp>
#include <mpc/MpcSoundPlayerChannel.hpp>
#include <mpc/MpcStereoMixerChannel.hpp>
#include <mpc/MpcIndivFxMixerChannel.hpp>

#include <file/File.hpp>
#include <thirdp/bcmath/bcmath_stl.h>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::audiomidi;
using namespace std;

EventHandler::EventHandler()
{
	sequencer = Mpc::instance().getSequencer();
	sampler = Mpc::instance().getSampler();
	msGui = Mpc::instance().getUis().lock()->getMidiSyncGui();
}

void EventHandler::handle(weak_ptr<mpc::sequencer::Event> event, mpc::sequencer::Track* track)
{
	auto ne = dynamic_pointer_cast<mpc::sequencer::NoteEvent>(event.lock());
	if (!track->isOn() && event.lock()->getTick() != -1) return;
	handleNoThru(event, track, -1);
	midiOut(event, track);
}

void EventHandler::handleNoThru(weak_ptr<mpc::sequencer::Event> e, mpc::sequencer::Track* track, int timeStamp)
{
	auto event = e.lock();

	auto lSequencer = sequencer.lock();

	auto countMetronomeScreen = dynamic_pointer_cast<CountMetronomeScreen>(Screens::getScreenComponent("countmetronome"));

	if (track->getName().compare("click") == 0)
	{
		auto lSequencer = sequencer.lock();
		
		if (!lSequencer->isCountEnabled())
		{
			return;
		}
		
		if (lSequencer->isRecordingOrOverdubbing() && !countMetronomeScreen->getInRec() && !lSequencer->isCountingIn())
		{
			return;
		}
		
		if (lSequencer->isPlaying() && !lSequencer->isRecordingOrOverdubbing() && !countMetronomeScreen->getInPlay() && !lSequencer->isCountingIn())
		{
			return;
		}

		auto ne = dynamic_pointer_cast<mpc::sequencer::NoteEvent>(event);

		if (ne->getVelocity() == 0)
		{
			return;
		}

		auto fs = Mpc::instance().getAudioMidiServices().lock()->getFrameSequencer().lock();
		auto eventFrame = fs->getEventFrameOffset(event->getTick());
		sampler.lock()->playMetronome(ne.get(), eventFrame);
		return;
	}
	else
	{
		if (lSequencer->isCountingIn() && event->getTick() != -1)
		{
			return;
		}
	}
	auto tce = dynamic_pointer_cast<mpc::sequencer::TempoChangeEvent>(event);
	auto mce = dynamic_pointer_cast<mpc::sequencer::MidiClockEvent>(event);
	auto ne = dynamic_pointer_cast<mpc::sequencer::NoteEvent>(event);
	auto me = dynamic_pointer_cast<mpc::sequencer::MixerEvent>(event);

	if (tce && tce->getTempo().toDouble() != lSequencer->getTempo().toDouble())
	{
		// Currently unsure whether MASTER tempo mode supports tempo changes.
		// I suspect it does. This would mean Sequencer would need to house a
		// master tempo change meta track.  We'd need to route some get/set
		// tempo calls probably. Until further notice we only allow
		// tempo change events if the tempo source is SEQUENCE.
		if (lSequencer->isTempoSourceSequenceEnabled())
		{
			lSequencer->setTempo(tce->getTempo());
		}
		return;
	}
	else if (mce)
	{
		auto mpcMidiPorts = Mpc::instance().getMidiPorts().lock();
		auto clockMsg = dynamic_cast<ctoot::midi::core::ShortMessage*>(mce->getShortMessage());
		clockMsg->setMessage(mce->getStatus());

		auto midiOutputStreamA = &mpcMidiPorts->getReceivers()[0];
		auto midiOutputStreamB = &mpcMidiPorts->getReceivers()[1];

		switch (msGui->getOut())
		{
		case 0:
			midiOutputStreamA->push_back(*clockMsg);
			break;
		case 1:
			midiOutputStreamB->push_back(*clockMsg);
			break;
		case 2:
			midiOutputStreamA->push_back(*clockMsg);
			midiOutputStreamB->push_back(*clockMsg);
			break;
		}
	}
	else if (ne)
	{
		auto busNumber = track->getBusNumber();
	
		if (busNumber != 0)
		{
			auto drum = busNumber - 1;
			if (ne->getDuration() != -1)
			{
				if (!(lSequencer->isSoloEnabled() && track->getTrackIndex() != lSequencer->getActiveTrackIndex()))
				{
					auto newVelo = static_cast<int>(ne->getVelocity() * (track->getVelocityRatio() * 0.01));
					mpc::sequencer::MidiAdapter midiAdapter;
					midiAdapter.process(ne, drum, newVelo);
					auto eventFrame = Mpc::instance().getAudioMidiServices().lock()->getFrameSequencer().lock()->getEventFrameOffset(event->getTick());
					
					if (timeStamp != -1)
					{
						eventFrame = timeStamp;
					}
					
					Mpc::instance().getMms()->mpcTransport(track->getTrackIndex(), midiAdapter.get().lock().get(), 0, ne->getVariationTypeNumber(), ne->getVariationValue(), eventFrame);
					
					if (Mpc::instance().getAudioMidiServices().lock()->getAudioServer()->isRealTime())
					{
						auto note = ne->getNote();
						auto program = Mpc::instance().getSampler().lock()->getProgram(Mpc::instance().getDrum(drum)->getProgram());
						int pad = program.lock()->getPadNumberFromNote(note);
						int bank = Mpc::instance().getBank();
						pad -= bank * 16;
					
						if (pad >= 0 && pad <= 15)
						{
							int notifyVelo = ne->getVelocity();
							if (notifyVelo == 0)
							{
								notifyVelo = 255;
							}
							Mpc::instance().getHardware().lock()->getPad(pad).lock()->notifyObservers(notifyVelo);
						}
					}
				}
			}
		}
	}
	else if (me)
	{
		auto pad = me->getPad();
		auto lSampler = sampler.lock();
		auto p = lSampler->getProgram(lSampler->getDrumBusProgramNumber(track->getBusNumber())).lock();
		auto mixer = p->getStereoMixerChannel(pad).lock();
	
		auto mixerSetupScreen = dynamic_pointer_cast<MixerSetupScreen>(Screens::getScreenComponent("mixer-setup"));

		if (mixerSetupScreen->isStereoMixSourceDrum())
		{
			auto busNumber = track->getBusNumber();
		
			if (busNumber != 0)
			{
				auto drumIndex = busNumber - 1;
				auto drum = Mpc::instance().getDrum(drumIndex);
				mixer = drum->getStereoMixerChannels().at(pad).lock();
			}
			else
			{
				return;
			}
		}
		
		if (me->getParameter() == 0)
		{
			mixer->setLevel(me->getValue());
		}
		else if (me->getParameter() == 1)
		{
			mixer->setPanning(me->getValue());
		}
	}
}

void EventHandler::midiOut(weak_ptr<mpc::sequencer::Event> e, mpc::sequencer::Track* track)
{
	auto event = e.lock();
	auto ne = dynamic_pointer_cast<mpc::sequencer::NoteEvent>(event);

	if (ne)
	{
		auto transGui = Mpc::instance().getUis().lock()->getTransGui();
	
		if (transGui->getTr() == -1 || transGui->getTr() == ne->getTrack())
		{
			ne->setNote(ne->getNote() + transGui->getAmount());
		}
				
		auto deviceNumber = track->getDevice() - 1;
		
		if (deviceNumber < 0)
		{
			return;
		}
		
		auto channel = deviceNumber;
		
		if (channel > 15)
		{
			channel -= 16;
		}
		mpc::sequencer::MidiAdapter midiAdapter;
		midiAdapter.process(event, channel, -1);
		ctoot::midi::core::ShortMessage msg = *midiAdapter.get().lock().get();
		
		auto mpcMidiPorts = Mpc::instance().getMidiPorts().lock();

		vector<ctoot::midi::core::ShortMessage>* r;

		r = &mpcMidiPorts->getReceivers()[0];

		auto notifyLetter = "a";

		if (deviceNumber > 15)
		{
			deviceNumber -= 16;
			r = &mpcMidiPorts->getReceivers()[1];
			notifyLetter = "b";
		}

		if (!(Mpc::instance().getAudioMidiServices().lock()->isBouncing() &&
			Mpc::instance().getUis().lock()->getD2DRecorderGui()->isOffline()) && 
			r != nullptr && track->getDevice() != 0)
		{
			if (r != nullptr)
			{
				auto fs = Mpc::instance().getAudioMidiServices().lock()->getFrameSequencer().lock();
				auto eventFrame = fs->getEventFrameOffset(event->getTick());
				msg.bufferPos = eventFrame;
				if (r->size() < 100)
				{
					r->push_back(msg);
				}
				else
				{
					r->clear();
				}
			}
		}

		if (Mpc::instance().getLayeredScreen().lock()->getCurrentScreenName().compare("midioutputmonitor") == 0)
		{
			setChanged();
			notifyObservers(string(notifyLetter + to_string(deviceNumber)));
		}
	}
}
