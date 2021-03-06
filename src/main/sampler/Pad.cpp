#include <sampler/Pad.hpp>

#include <Mpc.hpp>

#include <Paths.hpp>
#include <sampler/Sampler.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/PgmAssignScreen.hpp>
#include <lcdgui/screens/VmpcSettingsScreen.hpp>

using namespace mpc::sampler;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace std;

std::vector<int> Pad::originalPadNotes = {	37, 36, 42, 82, 40, 38, 46, 44, 48, 47, 45, 43, 49, 55, 51, 53,
											54, 69, 81, 80, 65, 66, 76, 77, 56, 62, 63, 64, 73, 74, 71, 39,
											52, 57, 58, 59, 60, 61, 67, 68, 70, 72, 75, 78, 79, 35, 41, 50,
											83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98 };

std::vector<int>& Pad::getPadNotes(mpc::Mpc& mpc)
{
	auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>("vmpc-settings");
	
	if (vmpcSettingsScreen->initialPadMapping == 0)
	{
		static vector<int> vmpcPadNotes;

		if (vmpcPadNotes.size() == 0)
		{
			for (int i = 35; i <= 98; i++)
				vmpcPadNotes.push_back(i);
		}
		
		return vmpcPadNotes;
	}
	return originalPadNotes;
}

Pad::Pad(mpc::Mpc& mpc, int number)
	: mpc(mpc)
{
	this->index = number;

	note = getPadNotes(mpc)[number];
}

void Pad::setNote(int i)
{
	if (i < 34 || i > 98)
		return;

	auto pgmAssignScreen = mpc.screens->get<PgmAssignScreen>("program-assign");

	if (pgmAssignScreen->padAssign)
		(*mpc.getSampler().lock()->getMasterPadAssign())[index] = i;
	else
		note = i;
}

int Pad::getNote()
{
	auto pgmAssignScreen = mpc.screens->get<PgmAssignScreen>("program-assign");

	if (pgmAssignScreen->padAssign)
		return (*mpc.getSampler().lock()->getMasterPadAssign())[index];

	return note;
}

int Pad::getNumber()
{
    return index;
}
