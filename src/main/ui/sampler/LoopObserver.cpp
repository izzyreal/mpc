#include "LoopObserver.hpp"

#include <Mpc.hpp>
#include <Util.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>
#include <lcdgui/TwoDots.hpp>
#include <lcdgui/Wave.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>
#include <mpc/MpcSoundOscillatorControls.hpp>

using namespace mpc::ui::sampler;
using namespace std;

LoopObserver::LoopObserver()
{
	
	this->sampler = Mpc::instance().getSampler();
	playXNames = vector<string>{ "ALL", "ZONE", "BEFOR ST", "BEFOR TO", "AFTR END" };
	soundGui = Mpc::instance().getUis().lock()->getSoundGui();
	soundGui->addObserver(this);
	
	if (sampler.lock()->getSoundCount() != 0)
	{
		sound = sampler.lock()->getSound();
		sound.lock()->addObserver(this);
		sound.lock()->getOscillatorControls()->addObserver(this);
	}
	
	auto ls = Mpc::instance().getLayeredScreen().lock();
	twoDots = ls->getTwoDots();
	twoDots.lock()->Hide(false);
	twoDots.lock()->setVisible(0, true);
	twoDots.lock()->setVisible(1, true);
	twoDots.lock()->setVisible(2, false);
	twoDots.lock()->setVisible(3, false);
	wave = ls->getWave();
	wave.lock()->Hide(false);
	sndField = ls->lookupField("snd");
	playXField = ls->lookupField("playx");
	toField = ls->lookupField("to");
	endLengthField = ls->lookupField("endlength");
	endLengthValueField = ls->lookupField("endlengthvalue");
	loopField = ls->lookupField("loop");
	dummyField = ls->lookupField("dummy");
	toField.lock()->setSize(9 * 6 + 1, 9);
	endLengthValueField.lock()->setSize(9 * 6 + 1, 9);
	displaySnd();
	displayPlayX();
	displayTo();
	displayEndLength();
	displayEndLengthValue();
	displayLoop();
	
	if (sampler.lock()->getSoundCount() != 0) 
	{
		dummyField.lock()->setFocusable(false);
		waveformLoadData();

		wave.lock()->setSelection(sound.lock()->getLoopTo(), sound.lock()->getEnd());
		soundGui->initZones(sampler.lock()->getSound().lock()->getFrameCount());
	}
	else
	{
		wave.lock()->setSampleData(nullptr, false, 0);
		sndField.lock()->setFocusable(false);
		playXField.lock()->setFocusable(false);
		toField.lock()->setFocusable(false);
		endLengthField.lock()->setFocusable(false);
		endLengthValueField.lock()->setFocusable(false);
		loopField.lock()->setFocusable(false);
	}
}

void LoopObserver::displaySnd()
{
	auto lSampler = sampler.lock();
	auto ls = Mpc::instance().getLayeredScreen().lock();

	if (sampler.lock()->getSoundCount() != 0)
	{
		if (ls->getFocus().compare("dummy") == 0)
		{
			ls->setFocus(sndField.lock()->getName());
		}
		
		sound.lock()->deleteObserver(this);
		sound.lock()->getOscillatorControls()->deleteObserver(this);
		sound = dynamic_pointer_cast<mpc::sampler::Sound>(sampler.lock()->getSound().lock());
		sound.lock()->addObserver(this);
		sound.lock()->getOscillatorControls()->addObserver(this);
		
		auto sampleName = sound.lock()->getName();
		
		if (!sound.lock()->isMono())
		{
			sampleName = moduru::lang::StrUtil::padRight(sampleName, " ", 16) + "(ST)";
		}
		sndField.lock()->setText(sampleName);
	}
	else {
		sndField.lock()->setText("(no sound)");
		ls->setFocus("dummy");
	}
}

void LoopObserver::displayPlayX()
{
    playXField.lock()->setText(playXNames[soundGui->getPlayX()]);
}

void LoopObserver::displayTo()
{
	if (sampler.lock()->getSoundCount() != 0) {
		toField.lock()->setTextPadded(sound.lock()->getLoopTo(), " ");
	}
	else {
		toField.lock()->setTextPadded("0", " ");
	}
	if (!soundGui->isEndSelected()) displayEndLengthValue();
}

void LoopObserver::displayEndLength()
{
	endLengthField.lock()->setSize(31, 9);
	endLengthField.lock()->setText(soundGui->isEndSelected() ? "  End" : "Lngth");
	displayEndLengthValue();
	Mpc::instance().getLayeredScreen().lock()->lookupLabel("endlengthvalue").lock()->SetDirty();
}

void LoopObserver::displayEndLengthValue()
{
	if (sampler.lock()->getSoundCount() != 0)
	{
		if (soundGui->isEndSelected())
		{
			endLengthValueField.lock()->setTextPadded(sound.lock()->getEnd(), " ");
		}
		else
		{
			auto lSound = sound.lock();
			endLengthValueField.lock()->setTextPadded(sound.lock()->getEnd() - sound.lock()->getLoopTo(), " ");
		}
	}
	else
	{
		endLengthValueField.lock()->setTextPadded("0", " ");
	}
}

void LoopObserver::displayLoop()
{
	if (sampler.lock()->getSoundCount() == 0)
	{
		loopField.lock()->setText("OFF");
		return;
	}
	loopField.lock()->setText(sound.lock()->isLoopEnabled() ? "ON" : "OFF");
}

void LoopObserver::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);
	
	if (s.compare("soundindex") == 0)
	{
		displaySnd();
		displayTo();
		displayEndLength();
		waveformLoadData();
		wave.lock()->setSelection(sound.lock()->getLoopTo(), sound.lock()->getEnd());
		soundGui->initZones(sound.lock()->getLastFrameIndex());
	}
	else if (s.compare("loopto") == 0)
	{
		displayTo();
		wave.lock()->setSelection(sound.lock()->getLoopTo(), sound.lock()->getEnd());
	}
	else if (s.compare("endlength") == 0)
	{
		displayEndLength();
	}
	else if (s.compare("end") == 0)
	{
		displayEndLengthValue();
		wave.lock()->setSelection(sound.lock()->getLoopTo(), sound.lock()->getEnd());
	}
	else if (s.compare("loopenabled") == 0)
	{
		displayLoop();
	}
	else if (s.compare("playx") == 0)
	{
		displayPlayX();
	}
}

void LoopObserver::waveformLoadData()
{
	auto lSound = sound.lock();
	auto sampleData = sound.lock()->getSampleData();
	wave.lock()->setSampleData(sampleData, sound.lock()->isMono(), soundGui->getView());
}

LoopObserver::~LoopObserver()
{
	if (wave.lock())
	{
		//wave.lock()->Hide(true);
		//wave.lock()->setSampleData(nullptr, false, 0);
	}
	
	if (twoDots.lock())
	{
		twoDots.lock()->Hide(true);
	}

	soundGui->deleteObserver(this);
	
	if (sound.lock())
	{
		sound.lock()->deleteObserver(this);
		sound.lock()->getOscillatorControls()->deleteObserver(this);
	}
}
