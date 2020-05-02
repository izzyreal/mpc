#include <sampler/Pad.hpp>

#include <Mpc.hpp>
#include <ui/Uis.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <ui/sampler/MixerSetupGui.hpp>
#include <ui/vmpc/VmpcSettingsGui.hpp>
#include <StartUp.hpp>
#include <sampler/Sampler.hpp>

using namespace mpc::sampler;
using namespace std;

std::vector<int> Pad::originalPadNotes = {	37, 36, 42, 82, 40, 38, 46, 44, 48, 47, 45, 43, 49, 55, 51, 53,
											54, 69, 81, 80, 65, 66, 76, 77, 56, 62, 63, 64, 73, 74, 71, 39,
											52, 57, 58, 59, 60, 61, 67, 68, 70, 72, 75, 78, 79, 35, 41, 50,
											83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98 };

std::vector<int>& Pad::getPadNotes()
{
	if (mpc::ui::vmpc::VmpcSettingsGui::instance().getInitialPadMapping() == 0)
	{
		static vector<int> vmpcPadNotes;
		if (vmpcPadNotes.size() == 0)
		{
			for (int i = 35; i <= 98; i++)
			{
				vmpcPadNotes.push_back(i);
			}
		}
		return vmpcPadNotes;
	}
	return originalPadNotes;
}

Pad::Pad(int number)
{
	this->number = number;

	note = getPadNotes()[number];
	stereoMixerChannel = make_shared<ctoot::mpc::MpcStereoMixerChannel>();
	indivFxMixerChannel = make_shared<ctoot::mpc::MpcIndivFxMixerChannel>();
}

void Pad::setNote(int i)
{
	if (i < 34 || i > 98) return;

	if (Mpc::instance().getUis().lock()->getSamplerGui()->isPadAssignMaster()) {
		(*Mpc::instance().getSampler().lock()->getMasterPadAssign())[number] = i;
	}
	else {
		note = i;
	}
	setChanged();
	notifyObservers(string("padnotenumber"));
	setChanged();
	notifyObservers(string("note"));
	setChanged();
	notifyObservers(string("samplenumber"));
}

int Pad::getNote()
{
	if (Mpc::instance().getUis().lock()->getSamplerGui()->isPadAssignMaster()) {
		return (*Mpc::instance().getSampler().lock()->getMasterPadAssign())[number];
	}
	return note;
}

weak_ptr<ctoot::mpc::MpcStereoMixerChannel> Pad::getStereoMixerChannel()
{
    return stereoMixerChannel;
}

weak_ptr<ctoot::mpc::MpcIndivFxMixerChannel> Pad::getIndivFxMixerChannel()
{
	return indivFxMixerChannel;
}

int Pad::getNumber()
{
    return number;
}

Pad::~Pad() {
}
