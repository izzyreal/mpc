#include "EditSoundObserver.hpp"

//#include <disk/AbstractDisk.hpp>
#include <Mpc.hpp>
#include <lcdgui/Background.hpp>
#include <lcdgui/Field.hpp>
#include <ui/sampler/window/EditSoundGui.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::ui::sampler::window;
using namespace std;

EditSoundObserver::EditSoundObserver(mpc::Mpc* mpc)  
{
	this->mpc = mpc;
	timeStretchPresetNames = vector<string>{ "FEM VOX", "MALE VOX", "LOW MALE VOX", "VOCAL", "HFREQ RHYTHM", "MFREQ RHYTHM", "LFREQ RHYTHM", "PERCUSSION", "LFREQ PERC.", "STACCATO", "LFREQ SLOW", "MUSIC 1", "MUSIC 2", "MUSIC 3", "SOFT PERC.", "HFREQ ORCH.", "LFREQ ORCH.", "SLOW ORCH." };
	sampler = mpc->getSampler();
	vector<string> newTimeStretchPresetNames = vector<string>(54);
	auto totalCounter = 0;
	auto letters = vector<string>{ "A", "B", "C" };
	for (auto s : timeStretchPresetNames) {
		for (int i = 0; i < 3; i++) {
			s = moduru::lang::StrUtil::padRight(s, " ", 13);
			s += letters.at(i);
			newTimeStretchPresetNames.push_back(s);
			totalCounter++;
		}
	}
	timeStretchPresetNames = newTimeStretchPresetNames;
	//timeStretchPresetNames->set(int (52), " SLOW ORCH.  B");
	auto uis = mpc->getUis().lock();
	editSoundGui = uis->getEditSoundGui();
	editSoundGui->addObserver(this);
	sequencerWindowGui = uis->getSequencerWindowGui();
	sequencerWindowGui->addObserver(this);
	auto ls = mpc->getLayeredScreen().lock();
	editField = ls->lookupField("edit");
	variable0Field = ls->lookupField("variable0");
	variable0Label = ls->lookupLabel("variable0");
	ratioField = ls->lookupField("variable1");
	ratioLabel = ls->lookupLabel("variable1");
	presetField = ls->lookupField("variable2");
	presetLabel = ls->lookupLabel("variable2");
	adjustField = ls->lookupField("variable3");
	adjustLabel = ls->lookupLabel("variable3");
	endMarginField = ls->lookupField("endmargin");
	endMarginLabel = ls->lookupLabel("endmargin");
	createNewProgramField = ls->lookupField("createnewprogram");
	createNewProgramLabel = ls->lookupLabel("createnewprogram");
	displayEdit();
}

void EditSoundObserver::displayEdit()
{
    editField.lock()->setText(editNames[editSoundGui->getEdit()]);
	auto editLabel = mpc->getLayeredScreen().lock()->lookupLabel("edit").lock();
	editLabel->SetDirty();
    auto b = mpc->getLayeredScreen().lock()->getCurrentBackground();
	auto fk = mpc->getLayeredScreen().lock()->getFunctionKeys();
	if (editSoundGui->getEdit() == 0) {
        b->setName("editsound");
		fk->SetDirty();
		variable0Field.lock()->Hide(true);
        variable0Label.lock()->Hide(true);
        ratioField.lock()->Hide(true);
        ratioLabel.lock()->Hide(true);
        presetField.lock()->Hide(true);
        presetLabel.lock()->Hide(true);
        adjustField.lock()->Hide(true);
        adjustLabel.lock()->Hide(true);
        endMarginLabel.lock()->Hide(true);
        endMarginField.lock()->Hide(true);
        createNewProgramLabel.lock()->Hide(true);
        createNewProgramField.lock()->Hide(true);
    }
    else if (editSoundGui->getEdit() == 1) {
        b->setName("editloopfromsttoend");
		fk->SetDirty();
		variable0Field.lock()->Hide(true);
        variable0Label.lock()->Hide(true);
        ratioField.lock()->Hide(true);
        ratioLabel.lock()->Hide(true);
        presetField.lock()->Hide(true);
        presetLabel.lock()->Hide(true);
        adjustField.lock()->Hide(true);
        adjustLabel.lock()->Hide(true);
        endMarginLabel.lock()->Hide(true);
        endMarginField.lock()->Hide(true);
        createNewProgramLabel.lock()->Hide(true);
        createNewProgramField.lock()->Hide(true);
    }
    else if (editSoundGui->getEdit() == 2) {
        b->setName("editempty");
		fk->SetDirty();
		variable0Field.lock()->Hide(false);
        variable0Label.lock()->Hide(false);
        ratioField.lock()->Hide(true);
        ratioLabel.lock()->Hide(true);
        presetField.lock()->Hide(true);
        presetLabel.lock()->Hide(true);
        adjustField.lock()->Hide(true);
        adjustLabel.lock()->Hide(true);
        endMarginLabel.lock()->Hide(true);
        endMarginField.lock()->Hide(true);
        createNewProgramLabel.lock()->Hide(true);
        createNewProgramField.lock()->Hide(true);
        displayVariable();
    }
    else if (editSoundGui->getEdit() == 3) {
        b->setName("editempty");
		fk->SetDirty();
		variable0Field.lock()->Hide(false);
        variable0Label.lock()->Hide(false);
        ratioField.lock()->Hide(true);
        ratioLabel.lock()->Hide(true);
        presetField.lock()->Hide(true);
        presetLabel.lock()->Hide(true);
        adjustField.lock()->Hide(true);
        adjustLabel.lock()->Hide(true);
        endMarginLabel.lock()->Hide(true);
        endMarginField.lock()->Hide(true);
        createNewProgramLabel.lock()->Hide(true);
        createNewProgramField.lock()->Hide(true);
        displayVariable();
    }
    else if (editSoundGui->getEdit() == 4) {
        b->setName("editexecute");
		fk->SetDirty();
		variable0Field.lock()->Hide(true);
        variable0Label.lock()->Hide(true);
        ratioField.lock()->Hide(true);
        ratioLabel.lock()->Hide(true);
        presetField.lock()->Hide(true);
        presetLabel.lock()->Hide(true);
        adjustField.lock()->Hide(true);
        adjustLabel.lock()->Hide(true);
        endMarginLabel.lock()->Hide(true);
        endMarginField.lock()->Hide(true);
        createNewProgramLabel.lock()->Hide(true);
        createNewProgramField.lock()->Hide(true);
    }
    else if (editSoundGui->getEdit() == 5) {
        b->setName("editexecute");
		fk->SetDirty();
		variable0Field.lock()->Hide(true);
        variable0Label.lock()->Hide(true);
        ratioField.lock()->Hide(true);
        ratioLabel.lock()->Hide(true);
        presetField.lock()->Hide(true);
        presetLabel.lock()->Hide(true);
        adjustField.lock()->Hide(true);
        adjustLabel.lock()->Hide(true);
        endMarginLabel.lock()->Hide(true);
        endMarginField.lock()->Hide(true);
        createNewProgramLabel.lock()->Hide(true);
        createNewProgramField.lock()->Hide(true);
    }
    else if (editSoundGui->getEdit() == 6) {
        b->setName("editexecute");
		fk->SetDirty();
		variable0Field.lock()->Hide(true);
        variable0Label.lock()->Hide(true);
        ratioField.lock()->Hide(true);
        ratioLabel.lock()->Hide(true);
        presetField.lock()->Hide(true);
        presetLabel.lock()->Hide(true);
        adjustField.lock()->Hide(true);
        adjustLabel.lock()->Hide(true);
        endMarginLabel.lock()->Hide(true);
        endMarginField.lock()->Hide(true);
        createNewProgramLabel.lock()->Hide(true);
        createNewProgramField.lock()->Hide(true);
    }
    else if(editSoundGui->getEdit() == 7) {
        b->setName("editempty");
		fk->SetDirty();
		variable0Field.lock()->Hide(false);
        variable0Label.lock()->Hide(false);
        ratioField.lock()->Hide(false);
        ratioLabel.lock()->Hide(false);
        presetField.lock()->Hide(false);
        presetLabel.lock()->Hide(false);
        adjustField.lock()->Hide(false);
        adjustLabel.lock()->Hide(false);
        endMarginLabel.lock()->Hide(true);
        endMarginField.lock()->Hide(true);
        createNewProgramLabel.lock()->Hide(true);
        createNewProgramField.lock()->Hide(true);
        displayVariable();
    }
    else if(editSoundGui->getEdit() == 8) {
        b->setName("editempty");
		fk->SetDirty();
		variable0Field.lock()->Hide(true);
        variable0Label.lock()->Hide(true);
        ratioField.lock()->Hide(true);
        ratioLabel.lock()->Hide(true);
        presetField.lock()->Hide(true);
        presetLabel.lock()->Hide(true);
        adjustField.lock()->Hide(true);
        adjustLabel.lock()->Hide(true);
        endMarginLabel.lock()->Hide(false);
        endMarginField.lock()->Hide(false);
        createNewProgramLabel.lock()->Hide(false);
        createNewProgramField.lock()->Hide(false);
        displayEndMargin();
        displayCreateNewProgram();
    }
}

void EditSoundObserver::displayCreateNewProgram()
{
    createNewProgramField.lock()->setText(editSoundGui->getCreateNewProgram() ? "YES" : "NO");
}

void EditSoundObserver::displayEndMargin()
{
    endMarginField.lock()->setText(to_string(editSoundGui->getEndMargin()));
}

void EditSoundObserver::displayVariable()
{
	auto lSampler = sampler.lock();
	if (editSoundGui->getEdit() == 2) {
		variable0Label.lock()->setSize(9 * 6 * 2, 18);
		variable0Label.lock()->setText("New name:");
		variable0Field.lock()->setLocation((variable0Label.lock()->getW()/2) + 19, 21 - 2);
		variable0Field.lock()->setText(editSoundGui->getNewName());
	}
	else if (editSoundGui->getEdit() == 3) {
		auto sampleName = lSampler->getSoundName(editSoundGui->getInsertSndNr());
		variable0Label.lock()->setSize(11 * 6 * 2, 18);
		variable0Label.lock()->setText("Insert Snd:");
		variable0Field.lock()->setLocation((variable0Label.lock()->getW()/2) + 19 * 2, 21 - 2);
		string stereo = "";
		if (!lSampler->getSound(editSoundGui->getInsertSndNr()).lock()->isMono()) stereo = "(ST)";
		variable0Field.lock()->setText(moduru::lang::StrUtil::padRight(sampleName, " ", 16) + stereo);
	}
	else if (editSoundGui->getEdit() == 7) {
		variable0Label.lock()->setSize(9 * 6 * 2, 18);
		variable0Label.lock()->setText("New name:");
		variable0Field.lock()->setLocation((variable0Label.lock()->getW()/2) + (19 * 2), 21 - 2);
		variable0Field.lock()->setText(editSoundGui->getNewName());
		ratioField.lock()->setText(to_string(editSoundGui->getTimeStretchRatio() / 100.0) + "%");
		presetField.lock()->setText(timeStretchPresetNames[editSoundGui->getTimeStretchPresetNumber()]);
		adjustField.lock()->setText(to_string(editSoundGui->getTimeStretchAdjust()));
	}
}

void EditSoundObserver::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);
	if (s.compare("edit") == 0) {
		displayEdit();
	}
	else if (s.compare("insertsoundnumber") == 0) {
		displayVariable();
	}
	else if (s.compare("timestretchratio") == 0) {
		displayVariable();
	}
	else if (s.compare("timestretchpresetnumber") == 0) {
		displayVariable();
	}
	else if (s.compare("timestretchadjust") == 0) {
		displayVariable();
	}
	else if (s.compare("endmargin") == 0) {
		displayEndMargin();
	}
	else if (s.compare("createnewprogram") == 0) {
		displayCreateNewProgram();
	}
}

EditSoundObserver::~EditSoundObserver() {
	editSoundGui->deleteObserver(this);
	sequencerWindowGui->deleteObserver(this);
}
