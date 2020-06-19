#include <file/mid/MidiWriter.hpp>

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
#include <midi/event/meta/InstrumentName.hpp>
#include <midi/event/meta/FrameRate.hpp>
#include <midi/event/meta/SmpteOffset.hpp>
#include <midi/event/meta/Tempo.hpp>
#include <midi/event/meta/Text.hpp>
#include <midi/event/meta/TimeSignatureEvent.hpp>
#include <midi/event/meta/TrackName.hpp>
#include <Util.hpp>
#include <disk/AbstractDisk.hpp>
#include <sequencer/ChannelPressureEvent.hpp>
#include <sequencer/ControlChangeEvent.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/MixerEvent.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/PitchBendEvent.hpp>
#include <sequencer/PolyPressureEvent.hpp>
#include <sequencer/ProgramChangeEvent.hpp>
#include <sequencer/SystemExclusiveEvent.hpp>
#include <sequencer/TempoChangeEvent.hpp>

#include <lang/StrUtil.hpp>
#include <file/ByteUtil.hpp>
#include <file/FileUtil.hpp>

#include <set>

using namespace mpc::midi::event;
using namespace mpc::file::mid;
using namespace std;

MidiWriter::MidiWriter(mpc::sequencer::Sequence* sequence)
{
	this->sequence = sequence;
	mf = make_unique<mpc::midi::MidiFile>();
	auto meta = make_shared<mpc::midi::MidiTrack>();
	auto seqParams = make_shared<mpc::midi::event::meta::Text>(0, 0, "LOOP=ON  START=000 END=END TEMPO=ON ");
	meta->insertEvent(seqParams);
	auto seqName = make_shared<mpc::midi::event::meta::TrackName>(0, 0, "MPC2000XL 1.00  " + moduru::lang::StrUtil::padRight(sequence->getName(), " ", 16));
	meta->insertEvent(seqName);
	vector<shared_ptr<mpc::midi::event::meta::Tempo>> tempos;
	int previousTick = 0;
	auto tempo = sequence->getInitialTempo();
	auto mpqn = (int)(6.0E7 / tempo);
	tempos.push_back(make_shared<mpc::midi::event::meta::Tempo>(0, 0, mpqn));

	for (auto& e : sequence->getTempoChangeEvents())
	{
		auto tce = e.lock();
		tempo = tce->getTempo();
		mpqn = (int)(6.0E7 / tempo);
		tempos.push_back(make_shared<mpc::midi::event::meta::Tempo>(tce->getTick(), tce->getTick() - previousTick, mpqn));
		previousTick = tce->getTick();
	}

	for (auto& t : tempos)
	{
		meta->insertEvent(t);
	}
	
	meta->insertEvent(make_shared<meta::SmpteOffset>(0, 0, mpc::midi::event::meta::FrameRate::FRAME_RATE_25, 0, 0, 0, 0, 0));
	std::set<vector<int>> tSigs;
	auto tSigTick = 0;
	auto lastAdded = vector<int>(3);
	for (int i = 0; i < sequence->getLastBar() + 1; i++) {
		auto actualTick = tSigTick;
		if (lastAdded[0] == sequence->getNumerator(i) && lastAdded[1] == sequence->getDenominator(i))
			actualTick = lastAdded[2];

		auto vec = vector<int>{ sequence->getNumerator(i) , sequence->getDenominator(i) , actualTick };
		auto foo = tSigs.emplace(vec);
		if (foo.second) {
			lastAdded[0] = sequence->getNumerator(i);
			lastAdded[1] = sequence->getDenominator(i);
			lastAdded[2] = actualTick;
		}
		tSigTick += (*sequence->getBarLengths())[i];
	}
	previousTick = 0;
	for (auto& ia : tSigs) {
		meta->insertEvent(make_shared<meta::TimeSignature>(ia[2], ia[2] - previousTick, ia[0], ia[1], 24, 8));
		previousTick = ia[2];
	}
	meta->setEndOfTrackDelta(sequence->getLastTick());
	mf->addTrack(meta);
	for (auto& t : sequence->getTracks()) {
		auto track = t.lock();
		noteOffs.clear();
		variations.clear();
		noteOns.clear();
		miscEvents.clear();
		if (track->getTrackIndex() > 63 || !track->isUsed())
			break;

		auto mt = make_shared<mpc::midi::MidiTrack>();
		auto in = make_shared<meta::InstrumentName>(0, 0, "        ");
		mt->insertEvent(in);
		string trackNumber = moduru::lang::StrUtil::padLeft(to_string(track->getTrackIndex()), "0", 2);
		auto text = make_shared<meta::Text>(0, 0, "TRACK DATA:" + trackNumber + "C0006403  000107   ");
		mt->insertEvent(text);
		auto tn = make_shared<meta::TrackName>(0, 0, moduru::lang::StrUtil::padRight(track->getName(), " ", 16));
		mt->insertEvent(tn);
		for (auto& event : track->getEvents()) {
			auto noteEvent = dynamic_pointer_cast<mpc::sequencer::NoteEvent>(event.lock());
			auto mpcSysExEvent = dynamic_pointer_cast<mpc::sequencer::SystemExclusiveEvent>(event.lock());
			auto pitchBendEvent = dynamic_pointer_cast<mpc::sequencer::PitchBendEvent>(event.lock());
			auto channelPressureEvent = dynamic_pointer_cast<mpc::sequencer::ChannelPressureEvent>(event.lock());
			auto polyPressureEvent = dynamic_pointer_cast<mpc::sequencer::PolyPressureEvent>(event.lock());
			auto controlChangeEvent = dynamic_pointer_cast<mpc::sequencer::ControlChangeEvent>(event.lock());
			auto programChangeEvent = dynamic_pointer_cast<mpc::sequencer::ProgramChangeEvent>(event.lock());
			auto mixerEvent = dynamic_pointer_cast<mpc::sequencer::MixerEvent>(event.lock());
			if (noteEvent) {
				addNoteOn(make_shared<NoteOn>(noteEvent->getTick(), track->getTrackIndex(), noteEvent->getNote(), noteEvent->getVelocity()));
				noteOffs.push_back(make_shared<NoteOn>(noteEvent->getTick() + noteEvent->getDuration(), track->getTrackIndex(), noteEvent->getNote(), 0));
				auto variation = false;
				shared_ptr<NoteOff> varNoteOff;
				auto varType = noteEvent->getVariationTypeNumber();
				auto varVal = noteEvent->getVariationValue();
				if (varType == 0 && varVal != 64)
					variation = true;

				if ((varType == 1 || varType == 2) && varVal != 0)
					variation = true;

				if (varType == 3 && varVal != 50)
					variation = true;

				if (variation)
					varNoteOff = make_shared<NoteOff>(noteEvent->getTick(), track->getTrackIndex(), varType, varVal);

				if (varNoteOff != nullptr)
					variations.push_back(varNoteOff);

			}
			else if (mpcSysExEvent)
			{
				auto sysExData = vector<char>((int)(mpcSysExEvent->getBytes().size()) - 1);
			
				for (int j = 0; j < sysExData.size(); j++)
				{
					sysExData[j] = mpcSysExEvent->getBytes()[j + 1];
				}

				auto see = make_shared<SystemExclusiveEvent>(240, mpcSysExEvent->getTick(), sysExData);
				miscEvents.push_back(see);
			}
			else if (pitchBendEvent)
			{
				auto amountBytes = moduru::file::ByteUtil::ushort2bytes(pitchBendEvent->getAmount() + 8192);
				auto pb = make_shared<PitchBend>(pitchBendEvent->getTick(), 1, (int)(amountBytes[0] & 255), (int)(amountBytes[1] & 255));
				pb->setBendAmount(pitchBendEvent->getAmount() + 8192);
				miscEvents.push_back(pb);
			}
			else if (channelPressureEvent)
			{
				auto ca = make_shared<ChannelAftertouch>(channelPressureEvent->getTick(), 1, channelPressureEvent->getAmount());
				miscEvents.push_back(ca);
			}
			else if (polyPressureEvent)
			{
				auto na = make_shared<NoteAftertouch>(polyPressureEvent->getTick(), 1, polyPressureEvent->getNote(), polyPressureEvent->getAmount());
				miscEvents.push_back(na);
			}
			else if (controlChangeEvent)
			{
				auto c = make_shared<Controller>(controlChangeEvent->getTick(), 1, controlChangeEvent->getController(), controlChangeEvent->getAmount());
				miscEvents.push_back(c);
			}
			else if (programChangeEvent)
			{
				auto pc = make_shared<ProgramChange>(programChangeEvent->getTick(), 1, programChangeEvent->getProgram());
				miscEvents.push_back(pc);
			}
			else if (mixerEvent)
			{
				auto sysExData = vector<char>(8);
				sysExData[0] = 71;
				sysExData[1] = 0;
				sysExData[2] = 68;
				sysExData[3] = 69;
				sysExData[7] = 247;
				sysExData[4] = mixerEvent->getParameter() + 1;
				sysExData[5] = mixerEvent->getPad();
				sysExData[6] = mixerEvent->getValue();
				auto see = make_shared<SystemExclusiveEvent>(240, mixerEvent->getTick(), sysExData);
				miscEvents.push_back(see);
			}
		}

		for (auto i = 0; i < sequence->getLastTick(); i++)
		{
			for (auto& no : noteOffs)
			{
				if (no->getTick() == i)
				{
					mt->insertEvent(no);
				}
			}

			for (auto& var : variations)
			{
				if (var->getTick() == i)
				{
					mt->insertEvent(var);
				}
			}
			
			for (auto& no : noteOns)
			{
				if (no->getTick() == i)
				{
					mt->insertEvent(no);
				}
			}

			for (auto& e : miscEvents)
			{
				if (e->getTick() == i)
				{
					mt->insertEvent(e);
				}
			}
		}
		createDeltas(mt);
		mf->addTrack(mt);
	}
	mf->setType(1);
	mf->setResolution(96);
}

void MidiWriter::addNoteOn(shared_ptr<NoteOn> noteOn)
{
	for (auto& no : noteOffs)
	{
		if (no->getNoteValue() == noteOn->getNoteValue() && no->getTick() > noteOn->getTick())
		{
			no = make_shared<NoteOn>(noteOn->getTick(), no->getChannel(), no->getNoteValue(), 0);
		}
	}
	noteOns.push_back(noteOn);
}

void MidiWriter::createDeltas(weak_ptr<mpc::midi::MidiTrack> midiTrack)
{
	auto mt = midiTrack.lock();
	shared_ptr<mpc::midi::event::MidiEvent> previousEvent;

	for (auto& me : mt->getEvents())
	{
		auto event = dynamic_pointer_cast<mpc::midi::event::NoteOn>(me.lock());
		if (event)
		{
			if (previousEvent)
			{
				if (event->getTick() != previousEvent->getTick())
				{
					event->setDelta(event->getTick() - previousEvent->getTick());
				}
				else
				{
					event->setDelta(0);
				}
			}
			previousEvent = event;
		}
	}
	auto previousTick = !previousEvent ? 0 : previousEvent->getTick();
	mt->setEndOfTrackDelta(sequence->getLastTick() - previousTick);
}

void MidiWriter::writeToFile(string path)
{
	auto fout = moduru::file::FileUtil::ofstreamw(path, ios::out | ios::binary);
	fout.unsetf(ios::skipws);
	mf->writeToOutputStream(fout);
	fout.close();
}
