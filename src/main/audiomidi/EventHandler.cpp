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

#include <hardware/Hardware.hpp>
#include <hardware/HwPad.hpp>

#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/MixerSetupScreen.hpp>
#include <lcdgui/screens/TransScreen.hpp>
#include <lcdgui/screens/SyncScreen.hpp>
#include <lcdgui/screens/window/CountMetronomeScreen.hpp>
#include <lcdgui/screens/window/VmpcDirectToDiskRecorderScreen.hpp>

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

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::audiomidi;
using namespace std;

EventHandler::EventHandler(mpc::Mpc& mpc)
	: mpc(mpc)
{
	sequencer = mpc.getSequencer();
	sampler = mpc.getSampler();
}

void EventHandler::handle(weak_ptr<mpc::sequencer::Event> event, mpc::sequencer::Track* track)
{
	if (!track->isOn() && event.lock()->getTick() != -1)
	{
		return;
	}

	auto ne = dynamic_pointer_cast<mpc::sequencer::NoteEvent>(event.lock());
	
	handleNoThru(event, track, -1);
	midiOut(event, track);
}

void EventHandler::handleNoThru(weak_ptr<mpc::sequencer::Event> e, mpc::sequencer::Track* track, int timeStamp)
{
	auto event = e.lock();

	auto lSequencer = sequencer.lock();

	auto countMetronomeScreen = dynamic_pointer_cast<CountMetronomeScreen>(mpc.screens->getScreenComponent("count-metronome"));

	if (track->getName().compare("click") == 0)
	{
		auto lSequencer = sequencer.lock();
		
		if (!lSequencer->isCountEnabled())
			return;
		
		if (lSequencer->isRecordingOrOverdubbing() && !countMetronomeScreen->getInRec() && !lSequencer->isCountingIn())
			return;
		
		if (lSequencer->isPlaying() && !lSequencer->isRecordingOrOverdubbing() && !countMetronomeScreen->getInPlay() && !lSequencer->isCountingIn())
			return;

		auto ne = dynamic_pointer_cast<mpc::sequencer::NoteEvent>(event);

		if (ne->getVelocity() == 0)
			return;

		auto fs = mpc.getAudioMidiServices().lock()->getFrameSequencer().lock();
		auto eventFrame = fs->getEventFrameOffset(event->getTick());
		sampler.lock()->playMetronome(ne.get(), eventFrame);
		return;
	}
	else
	{
		if (lSequencer->isCountingIn() && event->getTick() != -1)
			return;
	}

	auto tce = dynamic_pointer_cast<mpc::sequencer::TempoChangeEvent>(event);
	auto mce = dynamic_pointer_cast<mpc::sequencer::MidiClockEvent>(event);
	auto ne = dynamic_pointer_cast<mpc::sequencer::NoteEvent>(event);
	auto me = dynamic_pointer_cast<mpc::sequencer::MixerEvent>(event);

	if (tce && tce->getTempo() != lSequencer->getTempo())
	{
		// Currently unsure whether MASTER tempo mode supports tempo changes.
		// I suspect it does. This would mean Sequencer would need to house a
		// master tempo change meta track.  We'd need to route some get/set
		// tempo calls probably. Until further notice we only allow
		// tempo change events if the tempo source is SEQUENCE.
		if (lSequencer->isTempoSourceSequenceEnabled())
			lSequencer->setTempo(tce->getTempo());

		return;
	}
	else if (mce)
	{
		auto mpcMidiPorts = mpc.getMidiPorts().lock();
		auto clockMsg = dynamic_cast<ctoot::midi::core::ShortMessage*>(mce->getShortMessage());
		clockMsg->setMessage(mce->getStatus());

		auto midiOutputStreamA = &mpcMidiPorts->getReceivers()[0];
		auto midiOutputStreamB = &mpcMidiPorts->getReceivers()[1];

		auto syncScreen = dynamic_pointer_cast<SyncScreen>(mpc.screens->getScreenComponent("sync"));

		switch (syncScreen->out)
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
		auto busNumber = track->getBus();
	
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
					auto eventFrame = mpc.getAudioMidiServices().lock()->getFrameSequencer().lock()->getEventFrameOffset(event->getTick());
					
					if (timeStamp != -1)
						eventFrame = timeStamp;
					
					mpc.getMms()->mpcTransport(track->getTrackIndex(), midiAdapter.get().lock().get(), 0, ne->getVariationType(), ne->getVariationValue(), eventFrame);
					
					if (mpc.getAudioMidiServices().lock()->getAudioServer()->isRealTime())
					{
						auto note = ne->getNote();
						auto program = mpc.getSampler().lock()->getProgram(mpc.getDrum(drum)->getProgram());
						int pad = program.lock()->getPadIndexFromNote(note);
						int bank = mpc.getBank();
						pad -= bank * 16;
					
						if (pad >= 0 && pad <= 15)
						{
							int notifyVelo = ne->getVelocity();

							if (notifyVelo == 0)
								notifyVelo = 255;

							mpc.getHardware().lock()->getPad(pad).lock()->notifyObservers(notifyVelo);
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
		auto p = lSampler->getProgram(lSampler->getDrumBusProgramNumber(track->getBus())).lock();
		auto mixer = p->getStereoMixerChannel(pad).lock();
	
		auto mixerSetupScreen = dynamic_pointer_cast<MixerSetupScreen>(mpc.screens->getScreenComponent("mixer-setup"));

		if (mixerSetupScreen->isStereoMixSourceDrum())
		{
			auto busNumber = track->getBus();
		
			if (busNumber != 0)
			{
				auto drumIndex = busNumber - 1;
				auto drum = mpc.getDrum(drumIndex);
				mixer = drum->getStereoMixerChannels().at(pad).lock();
			}
			else
			{
				return;
			}
		}
		
		if (me->getParameter() == 0)
			mixer->setLevel(me->getValue());
		else if (me->getParameter() == 1)
			mixer->setPanning(me->getValue());
	}
}

void EventHandler::midiOut(weak_ptr<mpc::sequencer::Event> e, mpc::sequencer::Track* track)
{
	auto event = e.lock();
	auto ne = dynamic_pointer_cast<mpc::sequencer::NoteEvent>(event);

	if (ne)
	{
		auto transScreen = dynamic_pointer_cast<TransScreen>(mpc.screens->getScreenComponent("trans"));

		if (transScreen->tr == -1 || transScreen->tr == ne->getTrack())
			ne->setNote(ne->getNote() + transScreen->transposeAmount);

		auto deviceNumber = track->getDevice() - 1;
		
		if (deviceNumber < 0)
			return;
		
		auto channel = deviceNumber;
		
		if (channel > 15)
			channel -= 16;

		mpc::sequencer::MidiAdapter midiAdapter;
		midiAdapter.process(event, channel, -1);
		ctoot::midi::core::ShortMessage msg = *midiAdapter.get().lock().get();
		
		auto mpcMidiPorts = mpc.getMidiPorts().lock();

		vector<ctoot::midi::core::ShortMessage>& r = mpcMidiPorts->getReceivers()[0];

		auto notifyLetter = "a";

		if (deviceNumber > 15)
		{
			deviceNumber -= 16;
			r = mpcMidiPorts->getReceivers()[1];
			notifyLetter = "b";
		}

		auto directToDiskRecorderScreen = dynamic_pointer_cast<VmpcDirectToDiskRecorderScreen>(mpc.screens->getScreenComponent("vmpc-direct-to-disk-recorder"));

		if (!(mpc.getAudioMidiServices().lock()->isBouncing() &&
			directToDiskRecorderScreen->offline) &&
			track->getDevice() != 0)
		{
			auto fs = mpc.getAudioMidiServices().lock()->getFrameSequencer().lock();
			auto eventFrame = fs->getEventFrameOffset(event->getTick());
			msg.bufferPos = eventFrame;

			if (r.size() < 100)
				r.push_back(msg);
			else
				r.clear();
		}

		notifyObservers(string(notifyLetter + to_string(deviceNumber)));
	}
}
