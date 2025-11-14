#include "sampler/Pad.hpp"

#include "Mpc.hpp"
#include "IntTypes.hpp"
#include "lcdgui/screens/PgmAssignScreen.hpp"
#include "lcdgui/screens/VmpcSettingsScreen.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc::sampler;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

std::vector<mpc::DrumNoteNumber> Pad::originalPadNotes = {
    DrumNoteNumber{37}, DrumNoteNumber{36}, DrumNoteNumber{42},
    DrumNoteNumber{82}, DrumNoteNumber{40}, DrumNoteNumber{38},
    DrumNoteNumber{46}, DrumNoteNumber{44}, DrumNoteNumber{48},
    DrumNoteNumber{47}, DrumNoteNumber{45}, DrumNoteNumber{43},
    DrumNoteNumber{49}, DrumNoteNumber{55}, DrumNoteNumber{51},
    DrumNoteNumber{53},

    DrumNoteNumber{54}, DrumNoteNumber{69}, DrumNoteNumber{81},
    DrumNoteNumber{80}, DrumNoteNumber{65}, DrumNoteNumber{66},
    DrumNoteNumber{76}, DrumNoteNumber{77}, DrumNoteNumber{56},
    DrumNoteNumber{62}, DrumNoteNumber{63}, DrumNoteNumber{64},
    DrumNoteNumber{73}, DrumNoteNumber{74}, DrumNoteNumber{71},
    DrumNoteNumber{39},

    DrumNoteNumber{52}, DrumNoteNumber{57}, DrumNoteNumber{58},
    DrumNoteNumber{59}, DrumNoteNumber{60}, DrumNoteNumber{61},
    DrumNoteNumber{67}, DrumNoteNumber{68}, DrumNoteNumber{70},
    DrumNoteNumber{72}, DrumNoteNumber{75}, DrumNoteNumber{78},
    DrumNoteNumber{79}, DrumNoteNumber{35}, DrumNoteNumber{41},
    DrumNoteNumber{50},

    DrumNoteNumber{83}, DrumNoteNumber{84}, DrumNoteNumber{85},
    DrumNoteNumber{86}, DrumNoteNumber{87}, DrumNoteNumber{88},
    DrumNoteNumber{89}, DrumNoteNumber{90}, DrumNoteNumber{91},
    DrumNoteNumber{92}, DrumNoteNumber{93}, DrumNoteNumber{94},
    DrumNoteNumber{95}, DrumNoteNumber{96}, DrumNoteNumber{97},
    DrumNoteNumber{98}};

std::vector<mpc::DrumNoteNumber> &Pad::getPadNotes(const Mpc &mpcToUse)
{
    if (const auto vmpcSettingsScreen =
            mpcToUse.screens->get<ScreenId::VmpcSettingsScreen>();
        vmpcSettingsScreen->initialPadMapping == 0)
    {
        // VMPC2000XL's default pad mapping, which is different to the original.
        // It is simply a collection of ascending note numbers from 35 to 98.
        static std::vector<DrumNoteNumber> vmpcPadNotes;

        if (vmpcPadNotes.size() == 0)
        {
            for (int8_t i = MinDrumNoteNumber; i <= MaxDrumNoteNumber; i++)
            {
                vmpcPadNotes.push_back(DrumNoteNumber(i));
            }
        }

        return vmpcPadNotes;
    }
    return originalPadNotes;
}

Pad::Pad(Mpc &mpc, const ProgramPadIndex indexToUse) : mpc(mpc)
{
    index = indexToUse;
    note = getPadNotes(mpc)[indexToUse];
}

void Pad::setNote(const DrumNoteNumber i)
{
    if (i < NoDrumNoteAssigned || i > MaxDrumNoteNumber)
    {
        return;
    }

    if (const auto pgmAssignScreen =
            mpc.screens->get<ScreenId::PgmAssignScreen>();
        pgmAssignScreen->padAssign)
    {
        (*mpc.getSampler()->getMasterPadAssign())[index] = i;
    }
    else
    {
        note = i;
    }
}

mpc::DrumNoteNumber Pad::getNote() const
{
    if (const auto pgmAssignScreen =
            mpc.screens->get<ScreenId::PgmAssignScreen>();
        pgmAssignScreen->padAssign)
    {
        return (*mpc.getSampler()->getMasterPadAssign())[index];
    }

    return note;
}

mpc::ProgramPadIndex Pad::getIndex() const
{
    return index;
}
