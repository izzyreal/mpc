#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{

    class MidiInputScreen final : public ScreenComponent
    {

    public:
        void function(int i) override;
        void turnWheel(int i) override;

    public:
        MidiInputScreen(Mpc &mpc, int layerIndex);

    public:
        void open() override;

    private:
        void displayPass();
        void displayType();
        void displayMidiFilter();
        void displaySustainPedalToDuration();
        void displayProgChangeSeq();
        void displayReceiveCh();

    public:
        int getReceiveCh() const;
        bool getProgChangeSeq() const;
        bool isSustainPedalToDurationEnabled() const;
        bool isMidiFilterEnabled() const;
        int getType() const;
        std::vector<bool> &getCcPassEnabled();
        void setReceiveCh(int i);
        void setProgChangeSeq(bool b);
        void setSustainPedalToDuration(bool b);
        void setMidiFilterEnabled(bool b);
        void setType(int i);
        void setPass(bool b);
        bool isNotePassEnabled() const;
        void setNotePassEnabled(bool b);
        bool isPitchBendPassEnabled() const;
        void setPitchBendPassEnabled(bool b);
        bool isPgmChangePassEnabled() const;
        void setPgmChangePassEnabled(bool b);
        bool isChPressurePassEnabled() const;
        void setChPressurePassEnabled(bool b);
        bool isPolyPressurePassEnabled() const;
        void setPolyPressurePassEnabled(bool b);
        bool isExclusivePassEnabled() const;
        void setExclusivePassEnabled(bool b);

    private:
        const std::vector<std::string> typeNames = {"NOTES",
                                                    "PITCH BEND",
                                                    "PROG CHANGE",
                                                    "CH PRESSURE",
                                                    "POLY PRESS",
                                                    "EXCLUSIVE",
                                                    "  0-BANK SEL MSB",
                                                    "  1-MOD WHEEL",
                                                    "  2-BREATH CONT",
                                                    "  3-    03",
                                                    "  4-FOOT CONTROL",
                                                    "  5-PORTA TIME",
                                                    "  6-DATA ENTRY",
                                                    "  7-MAIN VOLUME",
                                                    "  8-BALANCE",
                                                    "  9-    09",
                                                    " 10-PAN",
                                                    " 11-EXPRESSION",
                                                    " 12-EFFECT 1",
                                                    " 13-EFFECT 2",
                                                    " 14-    14",
                                                    " 15-    15",
                                                    " 16-GEN.PUR. 1",
                                                    " 17-GEN.PUR. 2",
                                                    " 18-GEN.PUR. 3",
                                                    " 19-GEN.PUR. 4",
                                                    " 20-    20",
                                                    " 21-    21",
                                                    " 22-    22",
                                                    " 23-    23",
                                                    " 24-    24",
                                                    " 25-    25",
                                                    " 26-    26",
                                                    " 27-    27",
                                                    " 28-    28",
                                                    " 29-    29",
                                                    " 30-    30",
                                                    " 21-    31",
                                                    " 32-BANK SEL LSB",
                                                    " 33-MOD WHEL LSB",
                                                    " 34-BREATH LSB",
                                                    " 35-    35",
                                                    " 36-FOOT CNT LSB",
                                                    " 37-PORT TIME LS",
                                                    " 38-DATA ENT LSB",
                                                    " 39-MAIN VOL LSB",
                                                    " 40-BALANCE LSB",
                                                    " 41-    41",
                                                    " 42-PAN LSB",
                                                    " 43-EXPRESS LSB",
                                                    " 44-EFFECT 1 LSB",
                                                    " 45-EFFECT 2 MSB",
                                                    " 46-    46",
                                                    " 47-    47",
                                                    " 48-GEN.PUR.1 LS",
                                                    " 49-GEN.PUR.2 LS",
                                                    " 50-GEN.PUR.3 LS",
                                                    " 51-GEN.PUR.4 LS",
                                                    " 52-    52",
                                                    " 53-    53",
                                                    " 54-    54",
                                                    " 55-    55",
                                                    " 56-    56",
                                                    " 57-    57",
                                                    " 58-    58",
                                                    " 59-    59",
                                                    " 60-    60",
                                                    " 61-    61",
                                                    " 62-    62",
                                                    " 63-    63",
                                                    " 64-SUSTAIN PDL",
                                                    " 65-PORTA PEDAL",
                                                    " 66-SOSTENUTO",
                                                    " 67-SOFT PEDAL",
                                                    " 68-LEGATO FT SW",
                                                    " 69-HOLD 2",
                                                    " 70-SOUND VARI",
                                                    " 71-TIMBER/HARMO",
                                                    " 72-RELEASE TIME",
                                                    " 73-ATTACK TIME",
                                                    " 74-BRIGHTNESS",
                                                    " 75-SOUND CONT 6",
                                                    " 76-SOUND CONT 7",
                                                    " 77-SOUND CONT 8",
                                                    " 78-SOUND CONT 9",
                                                    " 79-SOUND CONT10",
                                                    " 80-GEN.PUR. 5",
                                                    " 81-GEN.PUR. 6",
                                                    " 82-GEN.PUR. 7",
                                                    " 83-GEN.PUR. 8",
                                                    " 84-PORTA CNTRL",
                                                    " 85-    85",
                                                    " 86-    86",
                                                    " 87-    87",
                                                    " 88-    88",
                                                    " 89-    89",
                                                    " 90-    90",
                                                    " 91-EXT EFF DPTH",
                                                    " 92-TREMOLO DPTH",
                                                    " 93-CHORUS DEPTH",
                                                    " 94-DETUNE DEPTH",
                                                    " 95-PHASER DEPTH",
                                                    " 96-DATA INCRE",
                                                    " 97-DATA DECRE",
                                                    " 98-NRPN LSB",
                                                    " 99-NRPN MSB",
                                                    "100-RPN LSB",
                                                    "101-RPN MSB",
                                                    "102-    102",
                                                    "103-    103",
                                                    "104-    104",
                                                    "105-    105",
                                                    "106-    106",
                                                    "107-    107",
                                                    "108-    108",
                                                    "109-    109",
                                                    "110-    110",
                                                    "111-    111",
                                                    "112-    112",
                                                    "113-    113",
                                                    "114-    114",
                                                    "115-    115",
                                                    "116-    116",
                                                    "117-    117",
                                                    "118-    118",
                                                    "119-    119",
                                                    "120-ALL SND OFF",
                                                    "121-RESET CONTRL",
                                                    "122-LOCAL ON/OFF",
                                                    "123-ALL NOTE OFF",
                                                    "124-OMNI OFF",
                                                    "125-OMNI ON",
                                                    "126-MONO MODE ON",
                                                    "127-POLY MODE ON"};
        int type = 0;
        bool midiFilter = false;
        int receiveCh = -1; // Default receive channel is -1 or "ALL"
        bool sustainPedalToDuration = true;
        bool progChangeSeq = false;

        bool notePassEnabled = true;
        bool pitchBendPassEnabled = true;
        bool pgmChangePassEnabled = true;
        bool chPressurePassEnabled = true;
        bool polyPressurePassEnabled = true;
        bool exclusivePassEnabled = true;
        std::vector<bool> ccPassEnabled = std::vector<bool>(128, true);
    };
} // namespace mpc::lcdgui::screens::window
