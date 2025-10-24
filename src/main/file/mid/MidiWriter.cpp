#include "file/mid/MidiWriter.hpp"

#include "midi/MidiFile.hpp"
#include "midi/MidiTrack.hpp"
#include "midi/event/ChannelAftertouch.hpp"
#include "midi/event/Controller.hpp"
#include "midi/event/MidiEvent.hpp"
#include "midi/event/NoteAftertouch.hpp"
#include "midi/event/NoteOff.hpp"
#include "midi/event/NoteOn.hpp"
#include "midi/event/PitchBend.hpp"
#include "midi/event/ProgramChange.hpp"
#include "midi/event/SystemExclusive.hpp"
#include "midi/event/meta/InstrumentName.hpp"
#include "midi/event/meta/FrameRate.hpp"
#include "midi/event/meta/SmpteOffset.hpp"
#include "midi/event/meta/Tempo.hpp"
#include "midi/event/meta/Text.hpp"
#include "midi/event/meta/TimeSignatureEvent.hpp"
#include "midi/event/meta/TrackName.hpp"
#include <Util.hpp>
#include "disk/AbstractDisk.hpp"
#include "sequencer/ChannelPressureEvent.hpp"
#include "sequencer/ControlChangeEvent.hpp"
#include "sequencer/MixerEvent.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/NoteEvent.hpp"
#include "sequencer/PitchBendEvent.hpp"
#include "sequencer/PolyPressureEvent.hpp"
#include "sequencer/ProgramChangeEvent.hpp"
#include "sequencer/SystemExclusiveEvent.hpp"
#include "sequencer/TempoChangeEvent.hpp"

#include <StrUtil.hpp>
#include "file/ByteUtil.hpp"

#include <set>
#include "midi/util/MidiUtil.hpp"

using namespace mpc::midi::event;
using namespace mpc::file::mid;

MidiWriter::MidiWriter(mpc::sequencer::Sequence* sequence)
{
	this->sequence = sequence;
	mf = std::make_unique<mpc::midi::MidiFile>();
	auto meta = std::make_shared<mpc::midi::MidiTrack>();
	auto seqParams = std::make_shared<mpc::midi::event::meta::Text>(0, 0, "LOOP=ON  START=000 END=END TEMPO=ON ");
	meta->insertEvent(seqParams);
	auto seqName = std::make_shared<mpc::midi::event::meta::TrackName>(0, 0, "MPC2000XL 1.00  " + StrUtil::padRight(sequence->getName(), " ", 16));
	meta->insertEvent(seqName);
	std::vector<std::shared_ptr<mpc::midi::event::meta::Tempo>> tempos;
	int previousTick = 0;
	auto tempo = sequence->getInitialTempo();
	auto mpqn = (int)(6.0E7 / tempo);
	tempos.push_back(std::make_shared<mpc::midi::event::meta::Tempo>(0, 0, mpqn));

	for (auto& e : sequence->getTempoChangeEvents())
	{
		tempo = e->getTempo();
		mpqn = (int)(6.0E7 / tempo);
		tempos.push_back(std::make_shared<mpc::midi::event::meta::Tempo>(e->getTick(), e->getTick() - previousTick, mpqn));
		previousTick = e->getTick();
	}

	for (auto& t : tempos)
	{
		meta->insertEvent(t);
	}
	
	meta->insertEvent(std::make_shared<meta::SmpteOffset>(0, 0, mpc::midi::event::meta::FrameRate::FRAME_RATE_25, 0, 0, 0, 0, 0));
	std::set<std::vector<int>> tSigs;
	auto tSigTick = 0;
	auto lastAdded = std::vector<int>(3);
	for (int i = 0; i < sequence->getLastBarIndex() + 1; i++) {
		auto actualTick = tSigTick;
		if (lastAdded[0] == sequence->getNumerator(i) && lastAdded[1] == sequence->getDenominator(i))
			actualTick = lastAdded[2];

		auto vec = std::vector<int>{ sequence->getNumerator(i) , sequence->getDenominator(i) , actualTick };
		auto foo = tSigs.emplace(vec);
		if (foo.second) {
			lastAdded[0] = sequence->getNumerator(i);
			lastAdded[1] = sequence->getDenominator(i);
			lastAdded[2] = actualTick;
		}
		tSigTick += sequence->getBarLengthsInTicks()[i];
	}
	previousTick = 0;
	for (auto& ia : tSigs) {
		meta->insertEvent(std::make_shared<meta::TimeSignature>(ia[2], ia[2] - previousTick, ia[0], ia[1], 24, 8));
		previousTick = ia[2];
	}
	meta->setEndOfTrackDelta(sequence->getLastTick());
	mf->addTrack(meta);

    int firstUsedTrackIndex = -1;
    int lastUsedTrackIndex = -1;

    for (int i = 0; i < 64; i++)
    {
        if (sequence->getTrack(i)->isUsed())
        {
            if (firstUsedTrackIndex == -1)
            {
                firstUsedTrackIndex = i;
            }
            lastUsedTrackIndex = i;
        }
    }

	for (int trackIndex = firstUsedTrackIndex;
         trackIndex >=0 && trackIndex <= lastUsedTrackIndex;
         trackIndex++) {
		noteOffs.clear();
		variations.clear();
		noteOns.clear();
		miscEvents.clear();

        auto t = sequence->getTrack(trackIndex);

		if (t->getIndex() > 63)
			break;

		auto mt = std::make_shared<mpc::midi::MidiTrack>();
		auto in = std::make_shared<meta::InstrumentName>(0, 0, "        ");
		mt->insertEvent(in);
		auto trackNumber = StrUtil::padLeft(std::to_string(t->getIndex()), "0", 2);
        std::string trackDevice = t->getDeviceIndex() == 0 ? "C0" : "E0";

        if (t->getDeviceIndex() > 0)
        {
            auto value = stoi(trackDevice, 0, 16);
            value += t->getDeviceIndex();
            trackDevice = mpc::midi::util::MidiUtil::byteToHex(static_cast<char>(value));
        }

		auto text = std::make_shared<meta::Text>(0, 0, "TRACK DATA:" + trackNumber + trackDevice +"006403  000107   ");
		mt->insertEvent(text);
		auto tn = std::make_shared<meta::TrackName>(0, 0, StrUtil::padRight(t->getName(), " ", 16));
		mt->insertEvent(tn);
		for (auto& event : t->getEvents()) {
			auto noteEvent = std::dynamic_pointer_cast<mpc::sequencer::NoteOnEvent>(event);
			auto mpcSysExEvent = std::dynamic_pointer_cast<mpc::sequencer::SystemExclusiveEvent>(event);
			auto pitchBendEvent = std::dynamic_pointer_cast<mpc::sequencer::PitchBendEvent>(event);
			auto channelPressureEvent = std::dynamic_pointer_cast<mpc::sequencer::ChannelPressureEvent>(event);
			auto polyPressureEvent = std::dynamic_pointer_cast<mpc::sequencer::PolyPressureEvent>(event);
			auto controlChangeEvent = std::dynamic_pointer_cast<mpc::sequencer::ControlChangeEvent>(event);
			auto programChangeEvent = std::dynamic_pointer_cast<mpc::sequencer::ProgramChangeEvent>(event);
			auto mixerEvent = std::dynamic_pointer_cast<mpc::sequencer::MixerEvent>(event);
			if (noteEvent) {
				addNoteOn(std::make_shared<NoteOn>(noteEvent->getTick(), t->getIndex(), noteEvent->getNote(), noteEvent->getVelocity()));
				noteOffs.push_back(std::make_shared<NoteOn>(noteEvent->getTick() + noteEvent->getDuration(), t->getIndex(), noteEvent->getNote(), 0));
				auto variation = false;
				std::shared_ptr<NoteOff> varNoteOff;
				auto varType = noteEvent->getVariationType();
				auto varVal = noteEvent->getVariationValue();
				if (varType == 0 && varVal != 64)
					variation = true;

				if ((varType == 1 || varType == 2) && varVal != 0)
					variation = true;

				if (varType == 3 && varVal != 50)
					variation = true;

				if (variation)
					varNoteOff = std::make_shared<NoteOff>(noteEvent->getTick(), t->getIndex(), varType, varVal);

				if (varNoteOff != nullptr)
					variations.push_back(varNoteOff);

			}
			else if (mpcSysExEvent)
			{
				auto sysExData = std::vector<char>((int)(mpcSysExEvent->getBytes().size()) - 1);
			
				for (int j = 0; j < sysExData.size(); j++)
				{
					sysExData[j] = mpcSysExEvent->getBytes()[j + 1];
				}

				auto see = std::make_shared<SystemExclusiveEvent>(240, mpcSysExEvent->getTick(), sysExData);
				miscEvents.push_back(see);
			}
			else if (pitchBendEvent)
			{
				auto amountBytes = ByteUtil::ushort2bytes(pitchBendEvent->getAmount() + 8192);
				auto pb = std::make_shared<PitchBend>(pitchBendEvent->getTick(), 1, (int)(amountBytes[0] & 255), (int)(amountBytes[1] & 255));
				pb->setBendAmount(pitchBendEvent->getAmount() + 8192);
				miscEvents.push_back(pb);
			}
			else if (channelPressureEvent)
			{
				auto ca = std::make_shared<ChannelAftertouch>(channelPressureEvent->getTick(), 1, channelPressureEvent->getAmount());
				miscEvents.push_back(ca);
			}
			else if (polyPressureEvent)
			{
				auto na = std::make_shared<NoteAftertouch>(polyPressureEvent->getTick(), 1, polyPressureEvent->getNote(), polyPressureEvent->getAmount());
				miscEvents.push_back(na);
			}
			else if (controlChangeEvent)
			{
				auto c = std::make_shared<Controller>(controlChangeEvent->getTick(), 1, controlChangeEvent->getController(), controlChangeEvent->getAmount());
				miscEvents.push_back(c);
			}
			else if (programChangeEvent)
			{
				auto pc = std::make_shared<ProgramChange>(programChangeEvent->getTick(), 1, programChangeEvent->getProgram());
				miscEvents.push_back(pc);
			}
			else if (mixerEvent)
			{
				auto sysExData = std::vector<char>(8);
				sysExData[0] = 71;
				sysExData[1] = 0;
				sysExData[2] = 68;
				sysExData[3] = 69;
				sysExData[4] = mixerEvent->getParameter() + 1;
				sysExData[5] = mixerEvent->getPad();
				sysExData[6] = mixerEvent->getValue();
                sysExData[7] = 0xF7;
				auto see = std::make_shared<SystemExclusiveEvent>(240, mixerEvent->getTick(), sysExData);
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
}

void MidiWriter::addNoteOn(std::shared_ptr<NoteOn> noteOn)
{
	for (auto& no : noteOffs)
	{
		if (no->getNoteValue() == noteOn->getNoteValue() && no->getTick() > noteOn->getTick())
		{
			no = std::make_shared<NoteOn>(noteOn->getTick(), no->getChannel(), no->getNoteValue(), 0);
		}
	}
	noteOns.push_back(noteOn);
}

void MidiWriter::createDeltas(std::weak_ptr<mpc::midi::MidiTrack> midiTrack)
{
	auto mt = midiTrack.lock();
	std::shared_ptr<mpc::midi::event::MidiEvent> previousEvent;

	for (auto& me : mt->getEvents())
	{
		auto event = std::dynamic_pointer_cast<mpc::midi::event::NoteOn>(me.lock());
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

void MidiWriter::writeToOStream(std::shared_ptr<std::ostream> _ostream)
{
    auto _ofstream = std::dynamic_pointer_cast<std::ofstream>(_ostream);
    if (_ofstream) _ofstream->unsetf(std::ios::skipws);
    mf->writeToOutputStream(_ostream);
    if (_ofstream) _ofstream->close();
}
