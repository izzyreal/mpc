#include <file/mid/MidiReader.hpp>

#include <midi/MidiFile.hpp>
#include <midi/MidiTrack.hpp>
#include <midi/event/ChannelAftertouch.hpp>
#include <midi/event/Controller.hpp>
#include <midi/event/MidiEvent.hpp>
#include <midi/event/NoteAftertouch.hpp>
#include <midi/event/NoteOff.hpp>
#include <midi/event/NoteOn.hpp>
#include <midi/event/PitchBend.hpp>
#include <midi/event/ProgramChange.hpp>
#include <midi/event/SystemExclusive.hpp>
#include <midi/event/meta/Tempo.hpp>
#include <midi/event/meta/Text.hpp>
#include <midi/event/meta/TimeSignatureEvent.hpp>
#include <midi/event/meta/TrackName.hpp>
#include <Mpc.hpp>
#include <disk/MpcFile.hpp>
#include <sequencer/ChannelPressureEvent.hpp>
#include <sequencer/ControlChangeEvent.hpp>
#include <sequencer/MixerEvent.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/PitchBendEvent.hpp>
#include <sequencer/PolyPressureEvent.hpp>
#include <sequencer/ProgramChangeEvent.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/SystemExclusiveEvent.hpp>
#include <sequencer/TempoChangeEvent.hpp>

#include <file/File.hpp>
#include <file/FileUtil.hpp>

using namespace moduru::file;
using namespace mpc::file::mid;
using namespace mpc::midi::event;
using namespace mpc::sequencer;
using namespace std;

MidiReader::MidiReader(mpc::disk::MpcFile* file, weak_ptr<Sequence> dest)
{
	
	this->dest = dest;
	auto sequence = dest.lock();
	sequence->setUsed(true);
	try {
		auto path = file->getFile().lock()->getPath();
		auto stream = FileUtil::ifstreamw(path, ios::in | ios::binary);
		midiFile = make_unique<mpc::midi::MidiFile>(stream);
	}
	catch (exception* e) {
		string logmsg = e->what();
	}
}

void MidiReader::parseSequence(mpc::Mpc& mpc)
{
	auto lSequencer = mpc.getSequencer().lock();
	auto sequence = dest.lock();
	auto midiTracks = midiFile->getTracks();
	int tmp = midiFile->getLengthInTicks();
	auto lengthInTicks = (int)(midiFile->getLengthInTicks() + midiTracks[0].lock()->getEndOfTrackDelta());

	vector<weak_ptr<meta::TimeSignature>> timeSignatures;
	vector<weak_ptr<meta::Tempo>> tempoChanges;

	auto firstLoopBar = -1;
	auto lastLoopBar = -1;
	int eventCounter = 0;

	for (auto& me : midiTracks[0].lock()->getEvents()) {
		auto lMe = me.lock();
		auto textEvent = dynamic_pointer_cast<meta::Text>(lMe);
		auto trackNameEvent = dynamic_pointer_cast<meta::TrackName>(lMe);
		auto tempoEvent = dynamic_pointer_cast<meta::Tempo>(lMe);
		auto timeSigEvent = dynamic_pointer_cast<meta::TimeSignature>(lMe);

		if (textEvent) {
			auto text = textEvent->getText();
			if (text.find("LOOP=ON") != string::npos) {
				sequence->setLoopEnabled(true);
			}
			if (text.find("LOOP=OFF") != string::npos) {
				sequence->setLoopEnabled(false);
			}
			if (text.find("TEMPO=ON") != string::npos) {
				lSequencer->setTempoSourceSequence(true);
			}
			if (text.find("TEMPO=OFF") != string::npos) {
				lSequencer->setTempoSourceSequence(false);
			}
			firstLoopBar = stoi(text.substr(15, 18));
			lastLoopBar = -1;
			if (isInteger(text.substr(23, 26))) {
				lastLoopBar = stoi(text.substr(23, 26));
			}
		}
		else if (trackNameEvent) {
			auto sequenceName = trackNameEvent->getTrackName().substr(16);
			sequence->setName(sequenceName);
		}
		else if (tempoEvent) {
			tempoChanges.push_back(tempoEvent);
		}
		else if (timeSigEvent) {
			timeSignatures.push_back(timeSigEvent);
		}
	}

	auto initialTempo = tempoChanges[0].lock()->getBpm();

	sequence->setInitialTempo(initialTempo);

	if (!lSequencer->isTempoSourceSequenceEnabled())
	{
		lSequencer->setTempo(initialTempo);
	}
	
	for (int i = 1; i < tempoChanges.size(); i++)
	{
		auto tce = sequence->addTempoChangeEvent().lock();
		auto lTcMidi = tempoChanges[i].lock();
		auto ratio = (float)(lTcMidi->getBpm()) / initialTempo;
		tce->setRatio((int)(ratio * 1000.0));
		tce->setStepNumber(i);
		tce->setTick(lTcMidi->getTick());
	}

	if (timeSignatures.size() == 1)
	{
		lengthInTicks = midiTracks[0].lock()->getEndOfTrackDelta();
	}

	int accumLength = 0;
	int barCounter = 0;

	for (int i = 0; i < timeSignatures.size(); i++)
	{
		auto current = timeSignatures[i].lock();
		shared_ptr<meta::TimeSignature> next;

		if (timeSignatures.size() > i + 1)
		{
			next = timeSignatures[i + 1].lock();
		}

		if (next)
		{
			while (accumLength < next->getTick())
			{
				sequence->setTimeSignature(barCounter, current->getNumerator(), current->getRealDenominator());
				int barLength = (*sequence->getBarLengths())[barCounter++];
				accumLength += barLength;
			}
		}
		else
		{
			while (accumLength < lengthInTicks)
			{
				sequence->setTimeSignature(barCounter, current->getNumerator(), current->getRealDenominator());
				int barLength = (*sequence->getBarLengths())[barCounter++];
				accumLength += barLength;
			}
		}
	}
	sequence->setLastBar(barCounter - 1);
	sequence->setFirstLoopBarIndex(firstLoopBar);

	if (lastLoopBar == -1)
	{
		sequence->setLastLoopBarIndex(sequence->getLastBarIndex());
		sequence->setLastLoopBarIndex(sequence->getLastLoopBarIndex() + 1);
	}
	else
	{
		sequence->setLastLoopBarIndex(lastLoopBar);
	}
	
	unique_ptr<NoteEvent> nVariation;
	
	for (int i = 1; i < midiTracks.size(); i++)
	{
		auto mt = midiTracks[i].lock();
		vector<shared_ptr<NoteOn>> noteOffs;
		vector<shared_ptr<NoteEvent>> noteOns;
		auto track = sequence->purgeTrack(i - 1).lock();
		track->setUsed(true);

		for (auto& me : mt->getEvents())
		{
			auto noteOff = dynamic_pointer_cast<NoteOff>(me.lock());
			auto noteOn = dynamic_pointer_cast<NoteOn>(me.lock());

			if (noteOff)
			{
				nVariation = make_unique<NoteEvent>();
				nVariation->setVariationTypeNumber(noteOff->getNoteValue());
				nVariation->setVariationValue(noteOff->getVelocity());
			}
			else if (noteOn)
			{
				if (noteOn->getVelocity() == 0)
				{
					if (getNumberOfNotes(noteOn->getNoteValue(), noteOns) > getNumberOfNoteOns(noteOn->getNoteValue(), noteOffs))
					{
						noteOffs.push_back(noteOn);
					}
				}
				else
				{
					if (getNumberOfNotes(noteOn->getNoteValue(), noteOns) > getNumberOfNoteOns(noteOn->getNoteValue(), noteOffs))
					{
						auto noteOff = make_shared<NoteOn>(noteOn->getTick(), 0, (noteOn->getNoteValue()), 0);
						noteOffs.push_back(noteOff);
					}

					auto ne = make_shared<NoteEvent>(noteOn->getNoteValue());
					ne->setTick(noteOn->getTick());
					ne->setVelocity(noteOn->getVelocity());
					
					if (nVariation)
					{
						ne->setVariationTypeNumber(nVariation->getVariationType());
						ne->setVariationValue(nVariation->getVariationValue());
					}
					else
					{
						ne->setVariationValue(64);
					}
					noteOns.push_back(ne);
				}
			}
		}

		for (auto& n: noteOns)
		{
			auto noteOn = dynamic_pointer_cast<mpc::sequencer::NoteEvent>(track->addEvent(n->getTick(), "note").lock());
			n->CopyValuesTo(noteOn);
			int indexCandidate = -1;
			int tickCandidate = 999999999;

			for (int k = 0; k < noteOffs.size(); k++)
			{
				auto noteOff = noteOffs[k];

				if (noteOff->getNoteValue() == noteOn->getNote() && noteOff->getTick() >= noteOn->getTick())
				{
					if (noteOff->getTick() < tickCandidate)
					{
						tickCandidate = noteOff->getTick();
						indexCandidate = k;
						break;
					}
				}
			}

			if (indexCandidate != -1)
			{
				noteOn->setDuration((int)(noteOffs[indexCandidate]->getTick() - noteOn->getTick()));
				noteOffs.erase(noteOffs.begin() + indexCandidate);
			}
			else
			{
				noteOn->setDuration(24);
			}
		}

		for (auto& me : mt->getEvents())
		{

			auto sysEx = dynamic_pointer_cast<mpc::midi::event::SystemExclusiveEvent>(me.lock());
			auto noteAfterTouch = dynamic_pointer_cast<NoteAftertouch>(me.lock());
			auto channelAfterTouch = dynamic_pointer_cast<ChannelAftertouch>(me.lock());
			auto programChange = dynamic_pointer_cast<ProgramChange>(me.lock());
			auto trackName = dynamic_pointer_cast<meta::TrackName>(me.lock());
			auto controller = dynamic_pointer_cast<Controller>(me.lock());
			auto pitchBend = dynamic_pointer_cast<PitchBend>(me.lock());

			if (sysEx)
			{
				auto sysExData = sysEx->getData();
			
				if (sysExData.size() == 8 && sysExData[0] == 71 && sysExData[1] == 0 && sysExData[2] == 68 && sysExData[3] == 69 && sysExData[7] == 247)
				{
					auto mixerEvent = dynamic_pointer_cast<MixerEvent>(track->addEvent(sysEx->getTick(), "mixer").lock());
					mixerEvent->setParameter(sysExData[4] - 1);
					mixerEvent->setPadNumber(sysExData[5]);
					mixerEvent->setValue(sysExData[6]);
				}
				else
				{
					sysExData = vector<char>(sysEx->getData().size() + 1);
					sysExData[0] = 240;
				
					for (int j = 0; j < sysEx->getData().size(); j++)
					{
						sysExData[j + 1] = sysEx->getData()[j];
					}

					auto see = dynamic_pointer_cast<mpc::sequencer::SystemExclusiveEvent>(track->addEvent(sysEx->getTick(), "systemexclusive").lock());
					vector<unsigned char> tmp;

					for (char c : sysExData)
					{
						tmp.push_back(static_cast<unsigned char>(c));
					}
					
					see->setBytes(tmp);
				}
			}
			else if (noteAfterTouch)
			{
				auto ppe = dynamic_pointer_cast<PolyPressureEvent>(track->addEvent(noteAfterTouch->getTick(), "polypressure").lock());
				ppe->setNote(noteAfterTouch->getNoteValue());
				ppe->setAmount(noteAfterTouch->getAmount());
			}
			else if (channelAfterTouch)
			{
				auto cpe = dynamic_pointer_cast<ChannelPressureEvent>(track->addEvent(channelAfterTouch->getTick(), "channelpressure").lock());
				cpe->setAmount(channelAfterTouch->getAmount());
			}
			else if (programChange)
			{
				auto pce = dynamic_pointer_cast<ProgramChangeEvent>(track->addEvent(programChange->getTick(), "programchange").lock());
				pce->setProgram(programChange->getProgramNumber() + 1);
			}
			else if (trackName)
			{
				track->setName(trackName->getTrackName());
			}
			else if (controller)
			{
				auto cce = dynamic_pointer_cast<ControlChangeEvent>(track->addEvent(controller->getTick(), "controlchange").lock());
				cce->setController(controller->getControllerType());
				cce->setAmount(controller->getValue());
			}
			else if (pitchBend)
			{
				auto pbe = dynamic_pointer_cast<PitchBendEvent>(track->addEvent(pitchBend->getTick(), "pitchbend").lock());
				pbe->setAmount(pitchBend->getBendAmount() - 8192);
			}
		}
	}
	sequence->initMetaTracks();
}

bool MidiReader::isInteger(string s)
{
	try {
		stoi(s);
		return true;
	}
	catch (invalid_argument e) {
		return false;
	}
}

int MidiReader::getNumberOfNoteOns(int noteValue, vector<shared_ptr<NoteOn>> allNotes)
{
	vector<weak_ptr<NoteOn>> oneNote;
	for (auto& no : allNotes) {
		if (no->getNoteValue() == noteValue)
			oneNote.push_back(no);
	}
	return oneNote.size();
}

int MidiReader::getNumberOfNotes(int noteValue, vector<shared_ptr<NoteEvent>> allNotes)
{
	vector<weak_ptr<NoteEvent>> oneNote;
	for (auto& ne : allNotes) {
		if (ne->getNote() == noteValue)
			oneNote.push_back(ne);
	}
	return oneNote.size();
}
