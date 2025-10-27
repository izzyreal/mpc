#include "EventRow.hpp"

#include <Mpc.hpp>
#include <Util.hpp>
#include "lcdgui/Layer.hpp"
#include "lcdgui/HorizontalBar.hpp"
#include "lcdgui/Field.hpp"
#include "lcdgui/Label.hpp"

#include "lcdgui/EventRowParameters.hpp"
#include "sampler/Pad.hpp"
#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"

#include "sequencer/ChannelPressureEvent.hpp"
#include "sequencer/ControlChangeEvent.hpp"
#include "sequencer/EmptyEvent.hpp"
#include "sequencer/Event.hpp"
#include "sequencer/MixerEvent.hpp"
#include "sequencer/NoteEvent.hpp"
#include "sequencer/PitchBendEvent.hpp"
#include "sequencer/PolyPressureEvent.hpp"
#include "sequencer/ProgramChangeEvent.hpp"
#include "sequencer/SystemExclusiveEvent.hpp"
#include "engine/Drum.hpp"

#include <cmath>
#include <StrUtil.hpp>

using namespace mpc::lcdgui;
using namespace mpc::sequencer;

std::vector<std::string> EventRow::controlNames{"BANK SEL MSB",
                                                "MOD WHEEL",
                                                "BREATH CONT",
                                                "03",
                                                "FOOT CONTROL",
                                                "PORTA TIME",
                                                "DATA ENTRY",
                                                "MAIN VOLUME",
                                                "BALANCE",
                                                "09",
                                                "PAN",
                                                "EXPRESSION",
                                                "EFFECT 1",
                                                "EFFECT 2",
                                                "14",
                                                "15",
                                                "GEN.PUR. 1",
                                                "GEN.PUR. 2",
                                                "GEN.PUR. 3",
                                                "GEN.PUR. 4",
                                                "20",
                                                "21",
                                                "22",
                                                "23",
                                                "24",
                                                "25",
                                                "26",
                                                "27",
                                                "28",
                                                "29",
                                                "30",
                                                "31",
                                                "BANK SEL LSB",
                                                "MOD WHEL LSB",
                                                "BREATH LSB",
                                                "35",
                                                "FOOT CNT LSB",
                                                "PORT TIME LS",
                                                "DATA ENT LSB",
                                                "MAIN VOL LSB",
                                                "BALANCE LSB",
                                                "41",
                                                "PAN LSB",
                                                "EXPRESS LSB",
                                                "EFFECT 1 LSB",
                                                "EFFECT 2 MSB",
                                                "46",
                                                "47",
                                                "GEN.PUR.1 LS",
                                                "GEN.PUR.2 LS",
                                                "GEN.PUR.3 LS",
                                                "GEN.PUR.4 LS",
                                                "52",
                                                "53",
                                                "54",
                                                "55",
                                                "56",
                                                "57",
                                                "58",
                                                "59",
                                                "60",
                                                "61",
                                                "62",
                                                "63",
                                                "SUSTAIN PDL",
                                                "PORTA PEDAL",
                                                "SOSTENUTO",
                                                "SOFT PEDAL",
                                                "LEGATO FT SW",
                                                "HOLD 2",
                                                "SOUND VARI",
                                                "TIMBER/HARMO",
                                                "RELEASE TIME",
                                                "ATTACK TIME",
                                                "BRIGHTNESS",
                                                "SOUND CONT 6",
                                                "SOUND CONT 7",
                                                "SOUND CONT 8",
                                                "SOUND CONT 9",
                                                "SOUND CONT10",
                                                "GEN.PUR. 5",
                                                "GEN.PUR. 6",
                                                "GEN.PUR. 7",
                                                "GEN.PUR. 8",
                                                "PORTA CNTRL",
                                                "85",
                                                "86",
                                                "87",
                                                "88",
                                                "89",
                                                "90",
                                                "EXT EFF DPTH",
                                                "TREMOLO DPTH",
                                                "CHORUS DEPTH",
                                                " DETUNE DEPTH",
                                                "PHASER DEPTH",
                                                "DATA INCRE",
                                                "DATA DECRE",
                                                "NRPN LSB",
                                                "NRPN MSB",
                                                "RPN LSB",
                                                "RPN MSB",
                                                "102",
                                                "103",
                                                "104",
                                                "105",
                                                "106",
                                                "107",
                                                "108",
                                                "109",
                                                "110",
                                                "111",
                                                "112",
                                                "113",
                                                "114",
                                                "115",
                                                "116",
                                                "117",
                                                "118",
                                                "119",
                                                "ALL SND OFF",
                                                "RESET CONTRL",
                                                "LOCAL ON/OFF",
                                                "ALL NOTE OFF",
                                                "OMNI OFF",
                                                "OMNI ON",
                                                "MONO MODE ON",
                                                "POLY MODE ON"};

EventRow::EventRow(mpc::Mpc &mpc, int rowIndex)
    : Component("event-row-" + std::to_string(rowIndex)), rowIndex(rowIndex),
      mpc(mpc)
{
    int w1 = 194;
    int h1 = 9;
    int x1 = 0;
    int y1 = 11 + (rowIndex * 9);

    MRECT parametersRect = MRECT(x1, y1, x1 + w1, y1 + h1);
    parameters = addChildT<EventRowParameters>(parametersRect);

    for (int i = 4; i >= 0; i--)
    {
        auto label = parameters->addChild(std::make_shared<Label>(
            mpc, letters[i] + std::to_string(rowIndex), drumNoteEventLabels[i],
            drumNoteEventXPos[i] + 1, rowIndex * 9 + 12,
            drumNoteEventLabels[i].length() * 6 + 1));
        labels.insert(begin(labels), std::dynamic_pointer_cast<Label>(label));

        auto tf = parameters->addChild(std::make_shared<Field>(
            mpc, letters[i] + std::to_string(rowIndex),
            drumNoteEventXPos[i] + 1 + drumNoteEventLabels[i].length() * 6 + 1,
            rowIndex * 9 + 12, drumNoteEventSizes[i]));

        fields.insert(begin(fields), std::dynamic_pointer_cast<Field>(tf));
    }

    h1 = 5;
    y1 = 13 + (rowIndex * 9);
    x1 = 198;
    w1 = 50;

    MRECT horizontalBarRect = MRECT(x1, y1, x1 + w1, y1 + h1);
    horizontalBar = std::dynamic_pointer_cast<HorizontalBar>(
        addChild(std::make_shared<HorizontalBar>(horizontalBarRect)));
    horizontalBar->Hide(true);

    setColors();
}

void EventRow::setBus(const int newBus)
{
    bus = newBus;
}

bool EventRow::isEmptyEvent()
{
    return std::dynamic_pointer_cast<EmptyEvent>(event.lock()) ? true : false;
}

void EventRow::init()
{
    if (std::dynamic_pointer_cast<NoteOnEvent>(event.lock()))
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
    else if (std::dynamic_pointer_cast<EmptyEvent>(event.lock()))
    {
        setLabelTexts(emptyEventLabels);
        setSizesAndLocations(emptyEventXPos, emptyEventSizes);
        setEmptyEventValues();
    }
    else if (std::dynamic_pointer_cast<PitchBendEvent>(event.lock()))
    {
        setLabelTexts(miscEventLabels);
        setSizesAndLocations(miscEventXPos, miscEventSizes);
        setMiscEventValues();
    }
    else if (std::dynamic_pointer_cast<ProgramChangeEvent>(event.lock()))
    {
        setLabelTexts(miscEventLabels);
        setSizesAndLocations(miscEventXPos, miscEventSizes);
        setMiscEventValues();
    }
    else if (std::dynamic_pointer_cast<ControlChangeEvent>(event.lock()))
    {
        setLabelTexts(controlChangeEventLabels);
        setSizesAndLocations(controlChangeEventXPos, controlChangeEventSizes);
        setControlChangeEventValues();
    }
    else if (std::dynamic_pointer_cast<ChannelPressureEvent>(event.lock()))
    {
        setLabelTexts(channelPressureEventLabels);
        setSizesAndLocations(channelPressureEventXPos,
                             channelPressureEventSizes);
        setChannelPressureEventValues();
    }
    else if (std::dynamic_pointer_cast<PolyPressureEvent>(event.lock()))
    {
        setLabelTexts(polyPressureEventLabels);
        setSizesAndLocations(polyPressureEventXPos, polyPressureEventSizes);
        setPolyPressureEventValues();
    }
    else if (std::dynamic_pointer_cast<SystemExclusiveEvent>(event.lock()))
    {
        setLabelTexts(sysexEventLabels);
        setSizesAndLocations(sysexEventXPos, sysexEventSizes);
        setSystemExclusiveEventValues();
    }
    else if (std::dynamic_pointer_cast<MixerEvent>(event.lock()))
    {
        setLabelTexts(mixerEventLabels);
        setSizesAndLocations(mixerEventXPos, mixerEventSizes);
        setMixerEventValues();
    }
}

void EventRow::setEmptyEventValues()
{
    fields[0]->Hide(false);
    labels[0]->Hide(false);
    labels[0]->setText("");
    fields[0]->setText(" ");

    horizontalBar->Hide(true);

    for (int i = 1; i < 5; i++)
    {
        fields[i]->Hide(true);
        labels[i]->Hide(true);
    }
}

void EventRow::setSystemExclusiveEventValues()
{
    if (!event.lock())
    {
        return;
    }

    auto see = std::dynamic_pointer_cast<SystemExclusiveEvent>(event.lock());

    for (int i = 0; i < 2; i++)
    {
        fields[i]->Hide(false);
        labels[i]->Hide(false);
    }

    char byteA[3];
    std::snprintf(byteA, sizeof(byteA), "%02X", see->getByteA());
    fields[0]->setText(StrUtil::padLeft(std::string(byteA), "0", 2));

    char byteB[3];
    std::snprintf(byteB, sizeof(byteB), "%02X", see->getByteB());
    fields[1]->setText(StrUtil::padLeft(std::string(byteB), "0", 2));

    horizontalBar->Hide(true);

    for (int i = 2; i < 5; i++)
    {
        fields[i]->Hide(true);
        labels[i]->Hide(true);
    }
}

void EventRow::setPolyPressureEventValues()
{
    if (!event.lock())
    {
        return;
    }

    auto ppe = std::dynamic_pointer_cast<PolyPressureEvent>(event.lock());

    for (int i = 0; i < 2; i++)
    {
        fields[i]->Hide(false);
        labels[i]->Hide(false);
    }

    fields[0]->setText(
        StrUtil::padLeft(std::to_string(ppe->getNote()), " ", 3) + "(" +
        mpc::Util::noteNames()[ppe->getNote()] + ")");
    fields[1]->setText(
        StrUtil::padLeft(std::to_string(ppe->getAmount()), " ", 3));

    horizontalBar->setValue(ppe->getAmount());
    horizontalBar->Hide(false);

    for (int i = 2; i < 5; i++)
    {
        fields[i]->Hide(true);
        labels[i]->Hide(true);
    }
}

void EventRow::setChannelPressureEventValues()
{
    if (!event.lock())
    {
        return;
    }

    auto cpe = std::dynamic_pointer_cast<ChannelPressureEvent>(event.lock());
    fields[0]->Hide(false);
    labels[0]->Hide(false);
    fields[0]->setText(
        StrUtil::padLeft(std::to_string(cpe->getAmount()), " ", 3));

    horizontalBar->setValue(cpe->getAmount());
    horizontalBar->Hide(false);

    for (int i = 1; i < 5; i++)
    {
        fields[i]->Hide(true);
        labels[i]->Hide(true);
    }
}

void EventRow::setControlChangeEventValues()
{
    if (!event.lock())
    {
        return;
    }

    auto cce = std::dynamic_pointer_cast<ControlChangeEvent>(event.lock());

    for (int i = 0; i < 2; i++)
    {
        fields[i]->Hide(false);
        labels[i]->Hide(false);
    }

    fields[0]->setText(controlNames[cce->getController()]);
    fields[1]->setText(
        StrUtil::padLeft(std::to_string(cce->getAmount()), " ", 3));

    auto lHorizontalBar = horizontalBar;
    lHorizontalBar->setValue(cce->getAmount());
    lHorizontalBar->Hide(false);

    for (int i = 2; i < 5; i++)
    {
        fields[i]->Hide(true);
        labels[i]->Hide(true);
    }
}

void EventRow::setMiscEventValues()
{
    if (!event.lock())
    {
        return;
    }

    auto parameterValue = 0;

    auto pitchBendEvent =
        std::dynamic_pointer_cast<PitchBendEvent>(event.lock());
    auto programChangeEvent =
        std::dynamic_pointer_cast<ProgramChangeEvent>(event.lock());

    if (pitchBendEvent != nullptr)
    {
        parameterValue = pitchBendEvent->getAmount();
    }

    if (programChangeEvent)
    {
        parameterValue = programChangeEvent->getProgram();
        labels[0]->setText(">PROGRAM CHANGE:");
        fields[0]->setSize(3 * 6 + 1, 9);
    }

    for (int i = 0; i < 2; i++)
    {
        fields[i]->Hide(false);
        labels[i]->Hide(false);
    }

    fields[0]->setText(
        StrUtil::padLeft(std::to_string(parameterValue), " ", 3));

    if (pitchBendEvent)
    {
        if (parameterValue > 0)
        {
            fields[0]->setText(
                "+" + StrUtil::padLeft(std::to_string(parameterValue), " ", 4));
        }
        else if (parameterValue < 0)
        {
            fields[0]->setText(
                "-" +
                StrUtil::padLeft(std::to_string(abs(parameterValue)), " ", 4));
        }
        else if (parameterValue == 0)
        {
            fields[0]->setText("    0");
        }
    }

    horizontalBar->Hide(true);

    for (int i = 1; i < 5; i++)
    {
        fields[i]->Hide(true);
        labels[i]->Hide(true);
    }
}

void EventRow::setMixerEventValues()
{
    if (!event.lock())
    {
        return;
    }

    auto mixerEvent = std::dynamic_pointer_cast<MixerEvent>(event.lock());

    for (int i = 0; i < 3; i++)
    {
        fields[i]->Hide(false);
        labels[i]->Hide(false);
    }

    fields[0]->setText(mixerParamNames[mixerEvent->getParameter()]);

    auto sampler = mpc.getSampler();

    if (bus == 0)
    {
        return;
    }

    auto program = sampler->getProgram(sampler->getDrumBusProgramIndex(bus));
    auto nn = program->getPad(mixerEvent->getPad())->getNote();

    auto padName = sampler->getPadName(mixerEvent->getPad());
    fields[1]->setText(std::string(nn == 34 ? "--" : std::to_string(nn)) + "/" +
                       padName);

    if (mixerEvent->getParameter() == 1)
    {
        labels[2]->setText("P:");
        auto panning = "L";

        if (mixerEvent->getValue() > 50)
        {
            panning = "R";
        }

        fields[2]->setText(
            panning +
            StrUtil::padLeft(std::to_string(abs(mixerEvent->getValue() - 50)),
                             " ", 2));

        if (mixerEvent->getValue() == 50)
        {
            fields[2]->setText("0  ");
        }
    }
    else
    {
        labels[2]->setText("L:");
        fields[2]->setText(
            StrUtil::padLeft(std::to_string(mixerEvent->getValue()), " ", 3));
    }

    auto lHorizontalBar = horizontalBar;
    lHorizontalBar->setValue(mixerEvent->getValue() * 1.27);
    lHorizontalBar->Hide(false);

    for (int i = 3; i < 5; i++)
    {
        fields[i]->Hide(true);
        labels[i]->Hide(true);
    }
}

void EventRow::setDrumNoteEventValues()
{
    if (!event.lock())
    {
        return;
    }

    auto ne = std::dynamic_pointer_cast<NoteOnEvent>(event.lock());

    for (int i = 0; i < 5; i++)
    {
        fields[i]->Hide(false);
        labels[i]->Hide(false);
    }

    if (ne->getNote() < 35 || ne->getNote() > 98)
    {
        fields[0]->setText("--/OFF");
    }
    else
    {
        if (bus != 0)
        {
            auto sampler = mpc.getSampler();
            auto program =
                sampler->getProgram(sampler->getDrumBusProgramIndex(bus));
            auto padName = sampler->getPadName(
                program->getPadIndexFromNote(ne->getNote()));
            fields[0]->setText(std::to_string(ne->getNote()) + "/" + padName);
        }
    }

    fields[1]->setText(noteVarParamNames[ne->getVariationType()]);

    if (ne->getVariationType() == 0)
    {
        fields[2]->setSize(4 * 6 + 1, 9);
        fields[2]->setLocation(90, fields[2]->getY());

        auto noteVarValue = (ne->getVariationValue() * 2) - 128;

        if (noteVarValue < -120)
        {
            noteVarValue = -120;
        }
        else if (noteVarValue > 120)
        {
            noteVarValue = 120;
        }

        if (noteVarValue == 0)
        {
            fields[2]->setText("   0");
        }
        else if (noteVarValue < 0)
        {
            fields[2]->setText(
                "-" +
                StrUtil::padLeft(std::to_string(abs(noteVarValue)), " ", 3));
        }
        else if (noteVarValue > 0)
        {
            fields[2]->setText(
                "+" + StrUtil::padLeft(std::to_string(noteVarValue), " ", 3));
        }
    }
    else if (ne->getVariationType() == 1 || ne->getVariationType() == 2)
    {
        auto noteVarValue = ne->getVariationValue();

        if (noteVarValue > 100)
        {
            noteVarValue = 100;
        }

        fields[2]->setText(
            StrUtil::padLeft(std::to_string(noteVarValue), " ", 3));
        fields[2]->setSize(3 * 6 + 1, 9);
        fields[2]->setLocation(90 + 6, fields[2]->getY());
    }
    else if (ne->getVariationType() == 3)
    {
        fields[2]->setSize(4 * 6 + 1, 9);
        fields[2]->setLocation(90, fields[2]->getY());
        auto noteVarValue = ne->getVariationValue() - 50;

        if (noteVarValue > 50)
        {
            noteVarValue = 50;
        }

        if (noteVarValue < 0)
        {
            fields[2]->setText(
                "-" +
                StrUtil::padLeft(std::to_string(abs(noteVarValue)), " ", 2));
        }
        else if (noteVarValue > 0)
        {
            fields[2]->setText(
                "+" + StrUtil::padLeft(std::to_string(noteVarValue), " ", 2));
        }
        else
        {
            fields[2]->setText("  0");
        }
    }

    fields[3]->setText(StrUtil::padLeft(
        std::to_string(ne->getDuration().value_or(1)), " ", 4));
    fields[4]->setText(
        StrUtil::padLeft(std::to_string(ne->getVelocity()), " ", 3));

    horizontalBar->setValue(ne->getVelocity());
    horizontalBar->Hide(false);
}

void EventRow::setMidiNoteEventValues()
{
    if (!event.lock())
    {
        return;
    }

    auto ne = std::dynamic_pointer_cast<NoteOnEvent>(event.lock());

    for (int i = 0; i < 3; i++)
    {
        fields[i]->Hide(false);
        labels[i]->Hide(false);
    }

    fields[0]->setText(StrUtil::padLeft(std::to_string(ne->getNote()), " ", 3) +
                       "(" + mpc::Util::noteNames()[ne->getNote()] + ")");
    fields[1]->setText(
        StrUtil::padLeft(std::to_string(*ne->getDuration()), " ", 4));
    fields[2]->setText(std::to_string(ne->getVelocity()));

    horizontalBar->setValue(ne->getVelocity());
    horizontalBar->Hide(false);

    for (int i = 3; i < 5; i++)
    {
        fields[i]->Hide(true);
        labels[i]->Hide(true);
    }
}

void EventRow::setColors()
{
    parameters->setColor(isSelected() && !isEmptyEvent());

    for (int i = 0; i < 5; i++)
    {
        if (isEmptyEvent())
        {
            if (i == 0)
            {
                labels[i]->setInverted(false);
            }
            fields[i]->setDoubleInverted(false);

            continue;
        }

        labels[i]->setInverted(isSelected());
        fields[i]->setDoubleInverted(isSelected());
    }
}

void EventRow::setLabelTexts(const std::vector<std::string> &labelTexts)
{
    for (int i = 0; i < labelTexts.size(); i++)
    {
        labels[i]->setText(labelTexts[i]);
    }
}

void EventRow::setSizesAndLocations(const std::vector<int> &xPositions,
                                    const std::vector<int> &fieldWidths)
{
    for (int i = 0; i < xPositions.size(); i++)
    {
        auto tf = fields[i];
        auto label = labels[i];

        auto labelTextLength = label->getText().length();

        tf->setSize((fieldWidths[i] * 6) + 1, 9);
        tf->setLocation(xPositions[i] + (labelTextLength * 6) - 1,
                        11 + (rowIndex * 9));

        label->setSize(labelTextLength * 6, 9);
        label->setLocation(xPositions[i] - 1, 11 + (rowIndex * 9));
    }
}

void EventRow::setEvent(std::weak_ptr<Event> newEvent)
{
    event = newEvent;
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
