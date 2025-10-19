#include <sampler/Pad.hpp>

#include <Mpc.hpp>

#include <lcdgui/screens/PgmAssignScreen.hpp>
#include <lcdgui/screens/VmpcSettingsScreen.hpp>

using namespace mpc::sampler;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

std::vector<int> Pad::originalPadNotes = {	37, 36, 42, 82, 40, 38, 46, 44, 48, 47, 45, 43, 49, 55, 51, 53,
											54, 69, 81, 80, 65, 66, 76, 77, 56, 62, 63, 64, 73, 74, 71, 39,
											52, 57, 58, 59, 60, 61, 67, 68, 70, 72, 75, 78, 79, 35, 41, 50,
											83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98 };

std::vector<int>& Pad::getPadNotes(mpc::Mpc& mpc)
{
	auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>();

	if (vmpcSettingsScreen->initialPadMapping == 0)
	{
        // VMPC2000XL's default pad mapping, which is different to the original.
        // It is simply a collection of ascending note numbers from 35 to 98.
		static std::vector<int> vmpcPadNotes;

		if (vmpcPadNotes.size() == 0)
		{
			for (int i = 35; i <= 98; i++)
				vmpcPadNotes.push_back(i);
		}
		
		return vmpcPadNotes;
	}
    else
    {
        return originalPadNotes;
    }
}

Pad::Pad(mpc::Mpc& mpc, int indexToUse) : mpc(mpc)
{
	index = indexToUse;
	note = getPadNotes(mpc)[indexToUse];
}

void Pad::setNote(int i)
{
	if (i < 34 || i > 98)
		return;

	auto pgmAssignScreen = mpc.screens->get<PgmAssignScreen>();

	if (pgmAssignScreen->padAssign)
		(*mpc.getSampler()->getMasterPadAssign())[index] = i;
	else
		note = i;
}

int Pad::getNote()
{
	auto pgmAssignScreen = mpc.screens->get<PgmAssignScreen>();

	if (pgmAssignScreen->padAssign)
		return (*mpc.getSampler()->getMasterPadAssign())[index];

	return note;
}

int Pad::getIndex()
{
    return index;
}
