#include "EventRow.hpp"

#include <Mpc.hpp>
#include <Util.hpp>
#include <lcdgui/Layer.hpp>
#include <lcdgui/HorizontalBar.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>

#include <lcdgui/EventRowParameters.hpp>
#include <sampler/Pad.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>

#include <sequencer/ChannelPressureEvent.hpp>
#include <sequencer/ControlChangeEvent.hpp>
#include <sequencer/EmptyEvent.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/MixerEvent.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/PitchBendEvent.hpp>
#include <sequencer/PolyPressureEvent.hpp>
#include <sequencer/ProgramChangeEvent.hpp>
#include <sequencer/SystemExclusiveEvent.hpp>
#include <mpc/MpcSoundPlayerChannel.hpp>

#include <file/File.hpp>

#include <cmath>
#include <lang/StrUtil.hpp>

#include <sstream>

using namespace mpc::lcdgui;
using namespace mpc::sequencer;
using namespace moduru::lang;
using namespace std;

vector<string> EventRow::controlNames{ "BANK SEL MSB", "MOD WHEEL", "BREATH CONT", "03", "FOOT CONTROL", "PORTA TIME", "DATA ENTRY", "MAIN VOLUME", "BALANCE", "09", "PAN", "EXPRESSION", "EFFECT 1", "EFFECT 2", "14", "15", "GEN.PUR. 1", "GEN.PUR. 2", "GEN.PUR. 3", "GEN.PUR. 4", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "BANK SEL LSB", "MOD WHEL LSB", "BREATH LSB", "35", "FOOT CNT LSB", "PORT TIME LS", "DATA ENT LSB", "MAIN VOL LSB", "BALANCE LSB", "41", "PAN LSB", "EXPRESS LSB", "EFFECT 1 LSB", "EFFECT 2 MSB", "46", "47", "GEN.PUR.1 LS", "GEN.PUR.2 LS", "GEN.PUR.3 LS", "GEN.PUR.4 LS", "52", "53", "54", "55", "56", "57", "58", "59", "60", "61", "62", "63", "SUSTAIN PDL", "PORTA PEDAL", "SOSTENUTO", "SOFT PEDAL", "LEGATO FT SW", "HOLD 2", "SOUND VARI", "TIMBER/HARMO", "RELEASE TIME", "ATTACK TIME", "BRIGHTNESS", "SOUND CONT 6", "SOUND CONT 7", "SOUND CONT 8", "SOUND CONT 9", "SOUND CONT10", "GEN.PUR. 5", "GEN.PUR. 6", "GEN.PUR. 7", "GEN.PUR. 8", "PORTA CNTRL", "85", "86", "87", "88", "89", "90", "EXT EFF DPTH", "TREMOLO DPTH", "CHORUS DEPTH", " DETUNE DEPTH", "PHASER DEPTH", "DATA INCRE", "DATA DECRE", "NRPN LSB", "NRPN MSB", "RPN LSB", "RPN MSB", "102", "103", "104", "105", "106", "107", "108", "109", "110", "111", "112", "113", "114", "115", "116", "117", "118", "119", "ALL SND OFF", "RESET CONTRL", "LOCAL ON/OFF", "ALL NOTE OFF", "OMNI OFF", "OMNI ON", "MONO MODE ON", "POLY MODE ON" };

EventRow::EventRow(mpc::Mpc& mpc, int rowIndex)
	: Component("event-row-" + to_string(rowIndex)), rowIndex(rowIndex), mpc(mpc)
{	
	int w1 = 194;
	int h1 = 9;
	int x1 = 0;
	int y1 = 11 + (rowIndex * 9);
	
	MRECT parametersRect = MRECT(x1, y1, x1 + w1, y1 + h1);
	parameters = dynamic_pointer_cast<EventRowParameters>(addChild(make_shared<EventRowParameters>(parametersRect)).lock());

	for (int i = 4; i >= 0; i--)
	{
		auto label = parameters.lock()->addChild(make_shared<Label>(mpc, letters[i] + to_string(rowIndex), drumNoteEventLabels[i], drumNoteEventXPos[i] + 1, rowIndex * 9 + 12, drumNoteEventLabels[i].length() * 6 + 1)).lock();
		labels.insert(begin(labels), dynamic_pointer_cast<Label>(label));

		auto tf = parameters.lock()->addChild(make_shared<Field>(mpc, letters[i] + to_string(rowIndex), drumNoteEventXPos[i] + 1 + drumNoteEventLabels[i].length() * 6 + 1, rowIndex * 9 + 12, drumNoteEventSizes[i])).lock();
		fields.insert(begin(fields), dynamic_pointer_cast<Field>(tf));
	}

	h1 = 5;
	y1 = 13 + (rowIndex * 9);
	x1 = 198;
	w1 = 50;

	MRECT horizontalBarRect = MRECT(x1, y1, x1 + w1, y1 + h1);
	horizontalBar = dynamic_pointer_cast<HorizontalBar>(addChild(make_shared<HorizontalBar>(horizontalBarRect)).lock());
	horizontalBar.lock()->Hide(true);

	setColors();
}

void EventRow::setBus(const int bus)
{
	this->bus = bus;
}

void EventRow::init()
{
	if (dynamic_pointer_cast<NoteEvent>(event.lock()))
	{
		if (bus == 0)
		{
			setLabelTexts(midiNoteEventLabels);
			setSizesAndLocations(midiNoteEventXPos, midiNoteEventSizes);
			setMidiNoteEventValues();
		}
		else
		{
			setLabelTexts(drumNoteEventLabels);
			setSizesAndLocations(drumNoteEventXPos, drumNoteEventSizes);
			setDrumNoteEventValues();
		}
	}
	else if (dynamic_pointer_cast<EmptyEvent>(event.lock()))
	{
		setLabelTexts(emptyEventLabels);
		setSizesAndLocations(emptyEventXPos, emptyEventSizes);
		setEmptyEventValues();
	}
	else if (dynamic_pointer_cast<PitchBendEvent>(event.lock()))
	{
		setLabelTexts(miscEventLabels);
		setSizesAndLocations(miscEventXPos, miscEventSizes);
		setMiscEventValues();
	}
	else if (dynamic_pointer_cast<ProgramChangeEvent>(event.lock()))
	{
		setLabelTexts(miscEventLabels);
		setSizesAndLocations(miscEventXPos, miscEventSizes);
		setMiscEventValues();
	}
	else if (dynamic_pointer_cast<ControlChangeEvent>(event.lock()))
	{
		setLabelTexts(controlChangeEventLabels);
		setSizesAndLocations(controlChangeEventXPos, controlChangeEventSizes);
		setControlChangeEventValues();
	}
	else if (dynamic_pointer_cast<ChannelPressureEvent>(event.lock()))
	{
		setLabelTexts(channelPressureEventLabels);
		setSizesAndLocations(channelPressureEventXPos, channelPressureEventSizes);
		setChannelPressureEventValues();
	}
	else if (dynamic_pointer_cast<PolyPressureEvent>(event.lock()))
	{
		setLabelTexts(polyPressureEventLabels);
		setSizesAndLocations(polyPressureEventXPos, polyPressureEventSizes);
		setPolyPressureEventValues();
	}
	else if (dynamic_pointer_cast<SystemExclusiveEvent>(event.lock()))
	{
		setLabelTexts(sysexEventLabels);
		setSizesAndLocations(sysexEventXPos, sysexEventSizes);
		setSystemExclusiveEventValues();
	}
	else if (dynamic_pointer_cast<MixerEvent>(event.lock()))
	{
		setLabelTexts(mixerEventLabels);
		setSizesAndLocations(mixerEventXPos, mixerEventSizes);
		setMixerEventValues();
	}
}

void EventRow::setEmptyEventValues()
{
	fields[0].lock()->Hide(false);
	labels[0].lock()->Hide(false);
	labels[0].lock()->setText("");
	fields[0].lock()->setText(" ");
	
	horizontalBar.lock()->Hide(true);

	for (int i = 1; i < 5; i++)
	{
		fields[i].lock()->Hide(true);
		labels[i].lock()->Hide(true);
	}
}

void EventRow::setSystemExclusiveEventValues()
{
	if (!event.lock())
	{
		return;
	}

    auto see = dynamic_pointer_cast<SystemExclusiveEvent>(event.lock());
    
	for (int i = 0; i < 2; i++)
	{
        fields[i].lock()->Hide(false);
        labels[i].lock()->Hide(false);
    }
    
	char byteA[3];
	sprintf(byteA, "%X", see->getByteA()); //convert number to hex
	fields[0].lock()->setText(StrUtil::padLeft(string(byteA), "0", 2));

	char byteB[3];
	sprintf(byteB, "%X", see->getByteB()); //convert number to hex
	fields[1].lock()->setText(StrUtil::padLeft(string(byteB), "0", 2));
    
	horizontalBar.lock()->Hide(true);

	for (int i = 2; i < 5; i++)
	{
		fields[i].lock()->Hide(true);
		labels[i].lock()->Hide(true);
	}
}

void EventRow::setPolyPressureEventValues()
{
	if (!event.lock())
	{
		return;
	}
    auto ppe = dynamic_pointer_cast<PolyPressureEvent>(event.lock());
    
	for (int i = 0; i < 2; i++)
	{
        fields[i].lock()->Hide(false);
        labels[i].lock()->Hide(false);
    }

    fields[0].lock()->setText(StrUtil::padLeft(to_string(ppe->getNote()), " ", 3) + "(" + mpc::Util::noteNames()[ppe->getNote()] + ")");
    fields[1].lock()->setText(StrUtil::padLeft(to_string(ppe->getAmount()), " ", 3));
	
	horizontalBar.lock()->setValue(ppe->getAmount());
    horizontalBar.lock()->Hide(false);
	
	for (int i = 2; i < 5; i++)
	{
		fields[i].lock()->Hide(true);
		labels[i].lock()->Hide(true);
	}
}

void EventRow::setChannelPressureEventValues()
{
	if (!event.lock())
	{
		return;
	}

    auto cpe = dynamic_pointer_cast< ChannelPressureEvent>(event.lock());
    fields[0].lock()->Hide(false);
    labels[0].lock()->Hide(false);
    fields[0].lock()->setText(StrUtil::padLeft(to_string(cpe->getAmount()), " ", 3));

	horizontalBar.lock()->setValue(cpe->getAmount());
    horizontalBar.lock()->Hide(false);

	for (int i = 1; i < 5; i++)
	{
		fields[i].lock()->Hide(true);
		labels[i].lock()->Hide(true);
	}
}

void EventRow::setControlChangeEventValues()
{
	if (!event.lock())
	{
		return;
	}

    auto cce = dynamic_pointer_cast< ControlChangeEvent>(event.lock());
    
	for (int i = 0; i < 2; i++)
	{
        fields[i].lock()->Hide(false);
        labels[i].lock()->Hide(false);
    }

    fields[0].lock()->setText(controlNames[cce->getController()]);
    fields[1].lock()->setText(StrUtil::padLeft(to_string(cce->getAmount()), " ", 3));
	
	auto lHorizontalBar = horizontalBar.lock();
	lHorizontalBar->setValue(cce->getAmount());
    lHorizontalBar->Hide(false);
    
	for (int i = 2; i < 5; i++)
	{
		fields[i].lock()->Hide(true);
		labels[i].lock()->Hide(true);
	}
}

void EventRow::setMiscEventValues()
{
	if (!event.lock())
	{
		return;
	}

    auto parameterValue = 0;
    
	auto pitchBendEvent = dynamic_pointer_cast<PitchBendEvent>(event.lock());
	auto programChangeEvent = dynamic_pointer_cast<ProgramChangeEvent>(event.lock());

	if (pitchBendEvent != nullptr)
	{
        parameterValue = pitchBendEvent->getAmount();
    }
    
	if (programChangeEvent)
	{
        parameterValue = programChangeEvent->getProgram();
        labels[0].lock()->setText(">PROGRAM CHANGE:");
        fields[0].lock()->setSize(3 * 6 + 1, 9);
    }
    
	for (int i = 0; i < 2; i++)
	{
        fields[i].lock()->Hide(false);
        labels[i].lock()->Hide(false);
    }
    
	fields[0].lock()->setText(StrUtil::padLeft(to_string(parameterValue), " ", 3));
    
	if (pitchBendEvent)
	{
        if(parameterValue > 0)
		{
            fields[0].lock()->setText("+" + StrUtil::padLeft(to_string(parameterValue), " ", 4));
        }
        else if(parameterValue < 0)
		{
            fields[0].lock()->setText("-" + StrUtil::padLeft(to_string(abs(parameterValue)), " ", 4));
        }
        else if(parameterValue == 0)
		{
            fields[0].lock()->setText("    0");
        }
    }

    horizontalBar.lock()->Hide(true);

	for (int i = 1; i < 5; i++)
	{
		fields[i].lock()->Hide(true);
		labels[i].lock()->Hide(true);
	}
}

void EventRow::setMixerEventValues()
{
	if (!event.lock())
	{
		return;
	}

	auto mixerEvent = dynamic_pointer_cast<MixerEvent>(event.lock());
	
	for (int i = 0; i < 3; i++)
	{
		fields[i].lock()->Hide(false);
		labels[i].lock()->Hide(false);
	}
	
	fields[0].lock()->setText(mixerParamNames[mixerEvent->getParameter()]);

	auto sampler = mpc.getSampler().lock();
	
	if (bus == 0)
	{
		return;
	}

	auto program = dynamic_pointer_cast<mpc::sampler::Program>(sampler->getProgram(sampler->getDrumBusProgramNumber(bus)).lock());
	auto nn = program->getPad(mixerEvent->getPad())->getNote();

	fields[1].lock()->setText(string(nn == 34 ? "--" : to_string(nn)) + "/" + sampler->getPadName(mixerEvent->getPad()));
	
	if (mixerEvent->getParameter() == 1)
	{
		labels[2].lock()->setText("P:");
		auto panning = "L";

		if (mixerEvent->getValue() > 50)
		{
			panning = "R";
		}

		fields[2].lock()->setText(panning + StrUtil::padLeft(to_string(abs(mixerEvent->getValue() - 50)), " ", 2));
		
		if (mixerEvent->getValue() == 50)
		{
			fields[2].lock()->setText("0  ");
		}
	}
	else
	{
		labels[2].lock()->setText("L:");
		fields[2].lock()->setText(StrUtil::padLeft(to_string(mixerEvent->getValue()), " ", 3));
	}

	auto lHorizontalBar = horizontalBar.lock();
	lHorizontalBar->setValue(mixerEvent->getValue() * 1.27);
	lHorizontalBar->Hide(false);
	
	for (int i = 3; i < 5; i++)
	{
		fields[i].lock()->Hide(true);
		labels[i].lock()->Hide(true);
	}
}

void EventRow::setDrumNoteEventValues()
{
	if (!event.lock())
	{
		return;
	}

	auto ne = dynamic_pointer_cast<NoteEvent>(event.lock());
	
	for (int i = 0; i < 5; i++)
	{
		fields[i].lock()->Hide(false);
		labels[i].lock()->Hide(false);
	}
	
	if (ne->getNote() < 35 || ne->getNote() > 98)
	{
		fields[0].lock()->setText("--/OFF");
	}
	else
	{
		if (bus != 0)
		{
			auto sampler = mpc.getSampler().lock();
			auto program = dynamic_pointer_cast<mpc::sampler::Program>(sampler->getProgram(sampler->getDrumBusProgramNumber(bus)).lock());
			fields[0].lock()->setText(to_string(ne->getNote()) + "/" + sampler->getPadName(program->getPadIndexFromNote(ne->getNote())));
		}
	}

	fields[1].lock()->setText(noteVarParamNames[ne->getVariationTypeNumber()]);
	
	if (ne->getVariationTypeNumber() == 0)
	{
		fields[2].lock()->setSize(4 * 6 + 1, 9);
		fields[2].lock()->setLocation(90, fields[2].lock()->getY());
	
		auto noteVarValue = (ne->getVariationValue() * 2) - 128;
		
		if (noteVarValue < -120)
		{
			noteVarValue = -120;
		}
		
		if (noteVarValue > 120)
		{
			noteVarValue = 120;
		}
		
		if (noteVarValue == 0)
		{
			fields[2].lock()->setText("   0");
		}
		else if (noteVarValue < 0)
		{
			fields[2].lock()->setText("-" + StrUtil::padLeft(to_string(abs(noteVarValue)), " ", 3));
		}
		else if (noteVarValue > 0)
		{
			fields[2].lock()->setText("+" + StrUtil::padLeft(to_string(noteVarValue), " ", 3));
		}
	}
	else if (ne->getVariationTypeNumber() == 1 || ne->getVariationTypeNumber() == 2)
	{
		auto noteVarValue = ne->getVariationValue();
	
		if (noteVarValue > 100)
		{
			noteVarValue = 100;
		}
		
		fields[2].lock()->setText(StrUtil::padLeft(to_string(noteVarValue), " ", 3));
		fields[2].lock()->setSize(3 * 6 + 1, 9);
		fields[2].lock()->setLocation(90 + 6, fields[2].lock()->getY());
	}
	else if (ne->getVariationTypeNumber() == 3)
	{
		fields[2].lock()->setSize(4 * 6 + 1, 9);
		fields[2].lock()->setLocation(90, fields[2].lock()->getY());
		auto noteVarValue = ne->getVariationValue() - 50;
		
		if (noteVarValue > 50)
		{
			noteVarValue = 50;
		}
		
		if (noteVarValue < 0)
		{
			fields[2].lock()->setText("-" + StrUtil::padLeft(to_string(abs(noteVarValue)), " ", 2));
		}
		else if (noteVarValue > 0)
		{
			fields[2].lock()->setText("+" + StrUtil::padLeft(to_string(noteVarValue), " ", 2));
		}
		else
		{
			fields[2].lock()->setText("  0");
		}
	}

	fields[3].lock()->setText(StrUtil::padLeft(to_string(ne->getDuration()), " ", 4));
	fields[4].lock()->setText(StrUtil::padLeft(to_string(ne->getVelocity()), " ", 3));
	
	horizontalBar.lock()->setValue(ne->getVelocity());
	horizontalBar.lock()->Hide(false);
}

void EventRow::setMidiNoteEventValues()
{
	if (!event.lock())
	{
		return;
	}

	auto ne = dynamic_pointer_cast<NoteEvent>(event.lock());
	
	for (int i = 0; i < 3; i++)
	{
		fields[i].lock()->Hide(false);
		labels[i].lock()->Hide(false);
	}
	
	fields[0].lock()->setText(StrUtil::padLeft(to_string(ne->getNote()), " ", 3) + "(" + mpc::Util::noteNames()[ne->getNote()] + ")");
	fields[1].lock()->setText(StrUtil::padLeft(to_string(ne->getDuration()), " ", 4));
	fields[2].lock()->setText(to_string(ne->getVelocity()));
	
	horizontalBar.lock()->setValue(ne->getVelocity());
	horizontalBar.lock()->Hide(false);

	for (int i = 3; i < 5; i++)
	{
		fields[i].lock()->Hide(true);
		labels[i].lock()->Hide(true);
	}
}

void EventRow::setColors()
{
	auto ls = mpc.getLayeredScreen().lock();

	for (int i = 0; i < 5; i++)
	{
		if (selected && !dynamic_pointer_cast<EmptyEvent>(event.lock()))
		{
			parameters.lock()->setColor(true);
			labels[i].lock()->setInverted(true);
		
			if (ls->getFocus().compare(fields[i].lock()->getName()) == 0)
			{
				fields[i].lock()->setInverted(false);
			}
			else
			{
				fields[i].lock()->setInverted(true);
			}
		}
		else
		{
			parameters.lock()->setColor(false);
			labels[i].lock()->setInverted(false);
			
			if (fields[i].lock()->hasFocus())
			{
				fields[i].lock()->setInverted(true);
			}
			else
			{
				fields[i].lock()->setInverted(false);
			}
		}
	}
}

void EventRow::setLabelTexts(const vector<string>& labelTexts)
{
	for (int i = 0; i < labelTexts.size(); i++)
	{
		labels[i].lock()->setText(labelTexts[i]);
	}
}

void EventRow::setSizesAndLocations(const vector<int>& xPositions, const vector<int>& fieldWidths)
{
	for (int i = 0; i < xPositions.size(); i++)
	{
		auto tf = fields[i].lock();
		auto label = labels[i].lock();

		auto labelTextLength = label->getText().length();

		tf->setSize((fieldWidths[i] * 6) + 1, 9);
		tf->setLocation(xPositions[i] + (labelTextLength * 6) - 1, 11 + (rowIndex * 9));

		label->setSize(labelTextLength * 6, 9);
		label->setLocation(xPositions[i] - 1, 11 + (rowIndex * 9));
	}
}

void EventRow::setEvent(weak_ptr<Event> event)
{
    this->event = event;
}

void EventRow::setSelected(bool b)
{
    selected = b;
    setColors();
}

bool EventRow::isSelected()
{
    return selected;
}
