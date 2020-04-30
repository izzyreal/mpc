#include <ui/sampler/window/ZoomObserver.hpp>

#include <Mpc.hpp>
#include <lcdgui/Wave.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/TwoDots.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <ui/sampler/window/ZoomGui.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>
#include <mpc/MpcSoundOscillatorControls.hpp>

#include <cmath>

using namespace mpc::ui::sampler::window;
using namespace std;

ZoomObserver::ZoomObserver()
{
	auto sampler = Mpc::instance().getSampler();
	playXNames = vector<string>{ "ALL", "ZONE", "BEFOR ST", "BEFOR TO", "AFTR END" };
	auto uis = Mpc::instance().getUis().lock();
	zoomGui = uis->getZoomGui();
	zoomGui->addObserver(this);
	soundGui = uis->getSoundGui();
	soundGui->addObserver(this);
	auto ls = Mpc::instance().getLayeredScreen().lock();
	csn = ls->getCurrentScreenName();
	sound = dynamic_pointer_cast<mpc::sampler::Sound>(sampler.lock()->getSound(soundGui->getSoundIndex()).lock());
;	auto lSound = sound.lock();
	lSound->deleteObservers();
	lSound->getOscillatorControls()->deleteObservers();
	lSound->addObserver(this);
	lSound->getOscillatorControls()->addObserver(this);
	twoDots = ls->getTwoDots();
	twoDots.lock()->Hide(false);
	twoDots.lock()->setVisible(0, false);
	twoDots.lock()->setVisible(1, false);
	twoDots.lock()->setVisible(2, true);
	twoDots.lock()->setVisible(3, true);
	twoDots.lock()->setSelected(3, false);
	wave = ls->getFineWave();
	wave.lock()->Hide(false);

	wave.lock()->setSampleData(lSound->getSampleData(), lSound->isMono(), soundGui->getView());

	startField = ls->lookupField("start");
	endField = ls->lookupField("end");
	lngthLabel = ls->lookupLabel("lngth");
	smplLngthField = ls->lookupField("smpllngth");
	playXField = ls->lookupField("playx");
	lngthField = ls->lookupField("lngth");
	loopLngthField = ls->lookupField("looplngth");
	toField = ls->lookupField("to");
	lngthField = ls->lookupField("lngth");
	
	if (csn.compare("startfine") == 0) {
		startField.lock()->setSize(8 * 6  + 1, 9);
		displayStart();
		displayLngthLabel();
		displaySmplLngth();
	}
	else if (csn.compare("endfine") == 0) {
		endField.lock()->setSize(8 * 6  + 1, 9);
		displayEnd();
		displayLngthLabel();
		displaySmplLngth();
	}
	else if (csn.compare("looptofine") == 0) {
		toField.lock()->setSize(8 * 6  + 1, 9);
		lngthField.lock()->setSize(8 * 6  + 1, 9);
		displayTo();
		displayLngthField();
		displayLoopLngth();
	}
	else if (csn.compare("loopendfine") == 0) {
		endField.lock()->setSize(8 * 6  + 1, 9);
		lngthField.lock()->setSize(8 * 6  + 1, 9);
		displayEnd();
		displayLngthField();
		displayLoopLngth();
	}
	else if (csn.compare("zonestartfine") == 0) {
		startField.lock()->setSize(8 * 6  + 1, 9);
		displayStart();
		displayLngthLabel();
	}
	else if (csn.compare("zoneendfine") == 0) {
		endField.lock()->setSize(8 * 6  + 1, 9);
		displayEnd();
		displayLngthLabel();
	}
	displayPlayX();
	displayFineWaveform();
}

void ZoomObserver::displayLoopLngth()
{
	loopLngthField.lock()->setText(zoomGui->isLoopLngthFix() ? "FIX" : "VARI");
}

void ZoomObserver::displayLngthField()
{
	auto lSound = sound.lock();
	lngthField.lock()->setTextPadded(lSound->getEnd() - lSound->getLoopTo(), " ");
}

void ZoomObserver::displayFineWaveform()
{
	auto lSound = sound.lock();
	if (csn.compare("startfine") == 0) {
		wave.lock()->setCenterSamplePos(lSound->getStart());
	}
	else if (csn.compare("zonestartfine") == 0) {
		wave.lock()->setCenterSamplePos(soundGui->getZoneStart(soundGui->getZoneNumber()));
	}
	else if (csn.compare("endfine") == 0 || csn.compare("loopendfine") == 0) {
		wave.lock()->setCenterSamplePos(lSound->getEnd());
	}
	else if (csn.compare("zoneendfine") == 0) {
		wave.lock()->setCenterSamplePos(soundGui->getZoneEnd(soundGui->getZoneNumber()));
	}
	else if (csn.compare("looptofine") == 0) {
		wave.lock()->setCenterSamplePos(lSound->getLoopTo());
	}
}

void ZoomObserver::displayStart()
{
	if (csn.compare("startfine") == 0) {
		startField.lock()->setTextPadded(sound.lock()->getStart(), " ");
	}
	else if (csn.compare("zonestartfine") == 0) {
		startField.lock()->setTextPadded(soundGui->getZoneStart(soundGui->getZoneNumber()), " ");
	}
}

void ZoomObserver::displayEnd()
{
	if (csn.compare("endfine") == 0 || csn.compare("loopendfine") == 0) {
		endField.lock()->setTextPadded(sound.lock()->getEnd(), " ");
	}
	else if (csn.compare("zoneendfine") == 0) {
		endField.lock()->setTextPadded(soundGui->getZoneEnd(soundGui->getZoneNumber()), " ");
	}
}

void ZoomObserver::displayLngthLabel()
{
	auto lSound = sound.lock();
	if (csn.compare("startfine") == 0 || csn.compare("endfine") == 0) {
		lngthLabel.lock()->setTextPadded(lSound->getEnd() - lSound->getStart(), " ");
	}
	else if (csn.compare("zonestartfine") == 0 || csn.compare("zoneendfine") == 0) {
		lngthLabel.lock()->setTextPadded(soundGui->getZoneEnd(soundGui->getZoneNumber()) - soundGui->getZoneStart(soundGui->getZoneNumber()), " ");
	}
}

void ZoomObserver::displaySmplLngth()
{
    smplLngthField.lock()->setText(zoomGui->isSmplLngthFix() ? "FIX" : "VARI");
}

void ZoomObserver::displayPlayX()
{
    playXField.lock()->setText(playXNames[soundGui->getPlayX()]);
}

void ZoomObserver::update(moduru::observer::Observable* o, nonstd::any arg)
{

	string s = nonstd::any_cast<string>(arg);

	if (s.compare("end") == 0) {
		if (csn.compare("startfine") == 0 || csn.compare("endfine") == 0) {
			displayLngthLabel();
		}
		else if (csn.compare("looptofine") == 0 || csn.compare("loopendfine") == 0) {
			displayLngthField();
		}
		if (csn.compare("endfine") == 0 || csn.compare("loopendfine") == 0) {
			displayEnd();
		}
		displayFineWaveform();
	}
	else if (s.compare("start") == 0) {
		if (csn.compare("startfine") == 0) {
			displayStart();
		}
		displayLngthLabel();
		displayFineWaveform();
	}
	else if (s.compare("smpllngthfix") == 0) {
		displaySmplLngth();
	}
	else if (s.compare("looplngthfix") == 0) {
		displayLoopLngth();
	}
	else if (s.compare("playx") == 0) {
		displayPlayX();
	}
	else if (s.compare("zoomlevel") == 0) {
		displayFineWaveform();
	}
	else if (s.compare("loopto") == 0) {
		if (csn.compare("looptofine") == 0) {
			displayTo();
			displayLngthField();
			displayFineWaveform();
		}
	}
	else if (s.compare("lngth") == 0) {
		displayLngthField();
		displayEnd();
		displayFineWaveform();
	}
	else if (s.compare("zone") == 0) {
		displayStart();
		displayEnd();
		displayLngthLabel();
		displayFineWaveform();
	}
}

void ZoomObserver::displayTo()
{
	toField.lock()->setTextPadded(sound.lock()->getLoopTo(), " ");
}

ZoomObserver::~ZoomObserver() {
	if (wave.lock()) {
		wave.lock()->setSampleData(nullptr, false, 0);
	}
	zoomGui->deleteObserver(this);
	soundGui->deleteObserver(this);
	auto lSound = sound.lock();
	lSound->deleteObserver(this);
	lSound->getOscillatorControls()->deleteObserver(this);
	if (twoDots.lock()) twoDots.lock()->Hide(true);
}
