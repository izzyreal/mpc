#include "MpcMultiMidiSynth.hpp"

#include "Voice.hpp"
#include "MpcSoundPlayerChannel.hpp"
#include "MpcMultiSynthControls.hpp"

#include <midi/message/ChannelMsg.hpp>
#include <midi/message/NoteMsg.hpp>
#include <midi/message/ShortMsg.hpp>
#include <midi/misc/Controller.hpp>

#include <synth/SynthChannel.hpp>

using namespace mpc::ctootextensions;
using namespace std;

MpcMultiMidiSynth::MpcMultiMidiSynth(shared_ptr<ctoot::synth::synths::multi::MultiSynthControls> controls)
	: MultiMidiSynth(controls)
{
	auto sr = dynamic_pointer_cast<MpcMultiSynthControls>(controls)->getSampleRate();
	for (int i = 0; i < 32; i++) {
		auto voice = make_shared<Voice>(i + 1, false, sr);
		voices.push_back(std::move(voice));
	}
}

vector<weak_ptr<Voice>> MpcMultiMidiSynth::getVoices() {
	vector<weak_ptr<Voice>> res;
	for (auto& v : voices)
		res.push_back(v);
	return res;
}

void MpcMultiMidiSynth::mpcTransportChannel(int track, ctoot::midi::core::MidiMessage* msg, int chan, int varType, int varValue, int l)
{
    mpcTransportChannel(track, msg, mapChannel(chan).lock().get(), varType, varValue, l);
}

void MpcMultiMidiSynth::mpcTransport(int track, ctoot::midi::core::MidiMessage* msg, int timestamp, int varType, int varValue, int l)
{
    if(ctoot::midi::message::ChannelMsg::isChannel(msg)) {
        mpcTransportChannel(track, msg, ctoot::midi::message::ChannelMsg::getChannel(msg), varType, varValue, l);
    }
}

void MpcMultiMidiSynth::mpcTransportChannel(int track, ctoot::midi::core::MidiMessage* msg, ctoot::synth::SynthChannel* synthChannel, int varType, int varValue, int l)
{
	if (synthChannel == nullptr) {
		return;
	}

	if (ctoot::midi::message::NoteMsg::isNote(msg)) {
		auto pitch = ctoot::midi::message::NoteMsg::getPitch(msg);
		auto velocity = ctoot::midi::message::NoteMsg::getVelocity(msg);
		auto on = ctoot::midi::message::NoteMsg::isOn(msg);
		string onstr = on ? "true" : "false";
		if (on && velocity != 0) {
			dynamic_cast<MpcSoundPlayerChannel*>(synthChannel)->mpcNoteOn(track, pitch, velocity, varType, varValue, l, true);
		}
		else {
			dynamic_cast<MpcSoundPlayerChannel*>(synthChannel)->mpcNoteOff(pitch, l);
		}
	}
	else {
		auto cmd = ctoot::midi::message::ChannelMsg::getCommand(msg);
		{
			int controller;
			switch (cmd) {
			case ctoot::midi::message::ChannelMsg::PITCH_BEND:
				synthChannel->setPitchBend(ctoot::midi::message::ShortMsg::getData1and2(msg));
				break;
			case ctoot::midi::message::ChannelMsg::CONTROL_CHANGE:
				controller = ctoot::midi::message::ShortMsg::getData1(msg);
				if (controller == ctoot::midi::misc::Controller::ALL_CONTROLLERS_OFF) {
					synthChannel->resetAllControllers();
				}
				else if (controller == ctoot::midi::misc::Controller::ALL_NOTES_OFF) {
					synthChannel->allNotesOff();
				}
				else if (controller == ctoot::midi::misc::Controller::ALL_SOUND_OFF) {
					synthChannel->allSoundOff();
				}
				else {
					synthChannel->controlChange(controller, ctoot::midi::message::ShortMsg::getData2(msg));
				}
				break;
			case ctoot::midi::message::ChannelMsg::CHANNEL_PRESSURE:
				synthChannel->setChannelPressure(ctoot::midi::message::ShortMsg::getData1(msg));
				break;
			}
		}

	}
}

weak_ptr<ctoot::synth::SynthChannel> MpcMultiMidiSynth::mapChannel(int chan)
{
	return getChannel(chan);
}

MpcMultiMidiSynth::~MpcMultiMidiSynth() {
}
