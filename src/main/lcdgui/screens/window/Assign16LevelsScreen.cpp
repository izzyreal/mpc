#include "Assign16LevelsScreen.hpp"

#include <Mpc.hpp>

#include <sequencer/Track.hpp>

#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/Label.hpp>

#include <hardware/Hardware.hpp>
#include <hardware/TopPanel.hpp>
#include <hardware/Led.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

Assign16LevelsScreen::Assign16LevelsScreen(const int layerIndex)
	: ScreenComponent("assign-16-levels", layerIndex)
{
}

void Assign16LevelsScreen::function(int i)
{
    BaseControls::function(i);
	switch (i)
	{
    case 4:
	{
		mpc::Mpc::instance().getHardware().lock()->getTopPanel().lock()->setSixteenLevelsEnabled(true);
		Mpc::instance().getHardware().lock()->getLed("sixteenlevels").lock()->light(true);
		ls.lock()->openScreen(ls.lock()->getPreviousScreenName());
		break;
	}
    }
}

void Assign16LevelsScreen::turnWheel(int i)
{
    init();

    if (param.compare("note") == 0)
    {
        setNote(note + i);
    }
    else if (param.compare("param") == 0)
    {
        setParameter(parameter + i);
    }
    else if (param.compare("type") == 0)
    {
        setType(type + i);
    }
    else if (param.compare("originalkeypad") == 0)
    {
        setOriginalKeyPad(originalKeyPad + i);
    }
}

void Assign16LevelsScreen::setNote(int newNote)
{
    if (newNote < 35 || newNote > 98)
    {
        return;
    }

    note = newNote;
    displayNote();
}

void Assign16LevelsScreen::setParameter(int i)
{
    if (i < 0 || i > 1)
    {
        return;
    }

    parameter = i;
    displayParameter();

    if (parameter == 1) {
        displayType();
        displayOriginalKeyPad();
    }
}

void Assign16LevelsScreen::setType(int i)
{
    if (i < 0 || i > 3)
    {
        return;
    }

    type = i;
    displayType();
}

void Assign16LevelsScreen::setOriginalKeyPad(int i)
{
    if (i < 0 || i > 15)
    {
        return;
    }

    originalKeyPad = i;
    displayOriginalKeyPad();
}

int Assign16LevelsScreen::getOriginalKeyPad()
{
    return originalKeyPad;
}

int Assign16LevelsScreen::getType()
{
    return type;
}

int Assign16LevelsScreen::getNote()
{
    return note;
}

int Assign16LevelsScreen::getParameter()
{
    return parameter;
}


vector<string> Assign16LevelsScreen::PARAM_NAMES{ "VELOCITY", "NOTE VAR" };
vector<string> Assign16LevelsScreen::TYPE_NAMES{ "TUNING", "DECAY", "ATTACK", "FILTER" };

void Assign16LevelsScreen::displayNote()
{
    init();

    auto pgmNumber = sampler.lock()->getDrumBusProgramNumber(track.lock()->getBusNumber());
    auto program = sampler.lock()->getProgram(pgmNumber).lock();
    auto pn = program->getPadIndexFromNote(note);
    auto sn = program->getNoteParameters(note)->getSndNumber();
    auto soundName = sn == -1 ? "OFF" : sampler.lock()->getSoundName(sn);
 
    findField("note").lock()->setText(to_string(note) + "/" + sampler.lock()->getPadName(pn) + "-" + soundName);
}

void Assign16LevelsScreen::displayParameter()
{
    findField("param").lock()->setText(PARAM_NAMES[parameter]);
    findField("type").lock()->Hide(parameter != 1);
    findLabel("type").lock()->Hide(parameter != 1);
    findField("originalkeypad").lock()->Hide(!(parameter == 1 && type == 0));
    findLabel("originalkeypad").lock()->Hide(!(parameter == 1 && type == 0));
}

void Assign16LevelsScreen::displayType()
{
    findField("type").lock()->setText(TYPE_NAMES[type]);
    findField("originalkeypad").lock()->Hide(type != 0);
    findLabel("originalkeypad").lock()->Hide(type != 0);
}

void Assign16LevelsScreen::displayOriginalKeyPad()
{
    findField("originalkeypad").lock()->setTextPadded(originalKeyPad + 1, " ");
}
