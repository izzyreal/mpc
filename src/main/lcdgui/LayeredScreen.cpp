#include "lcdgui/LayeredScreen.hpp"

#include <gui/BasicStructs.hpp>

#include <Mpc.hpp>
#include <StartUp.hpp>

#include "Field.hpp"

#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/SoundRecorder.hpp>

#include <lcdgui/EnvGraph.hpp>
#include <lcdgui/Label.hpp>
#include <lcdgui/Underline.hpp>
#include <lcdgui/HorizontalBar.hpp>
#include <lcdgui/VerticalBar.hpp>
#include <lcdgui/MixerFaderBackground.hpp>
#include <lcdgui/MixerTopBackground.hpp>
#include <lcdgui/Knob.hpp>
#include <lcdgui/TwoDots.hpp>
#include <lcdgui/Wave.hpp>
#include <lcdgui/Popup.hpp>
#include <lcdgui/SelectedEventBar.hpp>
#include <lcdgui/Effect.hpp>

#include <ui/Uis.hpp>

#include <ui/sequencer/AssignObserver.hpp>
#include <ui/sequencer/BarCopyObserver.hpp>
#include <ui/sequencer/EditSequenceObserver.hpp>
#include <ui/sequencer/NextSeqObserver.hpp>
#include <ui/sequencer/NextSeqPadObserver.hpp>
#include <ui/sequencer/SequencerObserver.hpp>
#include <ui/sequencer/SongObserver.hpp>
#include <ui/sequencer/StepEditorObserver.hpp>
#include <ui/sequencer/TrMoveObserver.hpp>
#include <ui/sequencer/TrMuteObserver.hpp>
#include <ui/sequencer/UserObserver.hpp>
#include <ui/sequencer/window/Assign16LevelsObserver.hpp>
#include <ui/sequencer/window/EraseObserver.hpp>
#include <ui/sequencer/window/MetronomeSoundObserver.hpp>
#include <ui/sequencer/window/MidiMonitorObserver.hpp>
#include <ui/sequencer/window/SequencerWindowObserver.hpp>
#include <ui/sequencer/window/StepWindowObserver.hpp>

#include <ui/sampler/DrumObserver.hpp>
#include <ui/sampler/LoopObserver.hpp>
#include <ui/sampler/MixerObserver.hpp>
#include <ui/sampler/FxEditObserver.hpp>
#include <ui/sampler/PgmAssignObserver.hpp>
#include <ui/sampler/PgmParamsObserver.hpp>
#include <ui/sampler/PurgeObserver.hpp>
#include <ui/sampler/SampleObserver.hpp>
#include <ui/sampler/SndParamsObserver.hpp>
#include <ui/sampler/SoundObserver.hpp>
#include <ui/sampler/TrimObserver.hpp>
#include <ui/sampler/ZoneObserver.hpp>

#include <ui/sampler/SamplerGui.hpp>

#include <ui/sampler/window/ChannelSettingsObserver.hpp>
#include <ui/sampler/window/EditSoundObserver.hpp>
#include <ui/sampler/window/MuteAssignObserver.hpp>
#include <ui/sampler/window/SamplerWindowObserver.hpp>
#include <ui/sampler/window/ZoomObserver.hpp>

#include <ui/NameObserver.hpp>

#include <ui/disk/DiskObserver.hpp>
#include <ui/disk/window/LoadASequenceFromAllObserver.hpp>
#include <ui/disk/window/DeleteAllFilesObserver.hpp>
#include <ui/disk/window/DirectoryObserver.hpp>
#include <ui/disk/window/SaveAllFileObserver.hpp>

#include <ui/midisync/SyncObserver.hpp>

#include <ui/misc/PunchObserver.hpp>
#include <ui/misc/SecondSeqObserver.hpp>
#include <ui/misc/TransObserver.hpp>

#include <ui/other/OthersObserver.hpp>

#include <ui/vmpc/DirectToDiskRecorderObserver.hpp>
#include <ui/vmpc/VmpcDiskObserver.hpp>

#include <file/FileUtil.hpp>

#include <rapidjson/filereadstream.h>

#include <cmath>
#include <set>

using namespace std;

using namespace rapidjson;

using namespace mpc::ui;
using namespace mpc::lcdgui;

using namespace mpc::ui::vmpc;
using namespace mpc::ui::sequencer;
using namespace mpc::ui::sequencer::window;
using namespace mpc::ui::sampler;
using namespace mpc::ui::sampler::window;
using namespace mpc::ui::midisync;
using namespace mpc::ui::misc;
using namespace mpc::ui::other;
using namespace mpc::ui::disk;
using namespace mpc::ui::disk::window;

static vector<string> samplerWindowNames = vector<string>{ "program", "deleteprogram", "deleteallprograms", "createnewprogram",
"copyprogram", "assignmentview", "initpadassign", "copynoteparameters", "velocitymodulation",
"veloenvfilter", "velopitch", "autochromaticassignment", "keeporretry" };

static vector<string> seqWindowNames = vector<string>{ "copysequence", "copytrack", "countmetronome", "timedisplay",
"tempochange", "timingcorrect", "changetsig", "changebars", "changebars2", "eraseallofftracks",
"transmitprogramchanges", "multirecordingsetup", "midiinput", "midioutput", "editvelocity", "sequence",
"deletesequence", "track", "deletetrack", "deleteallsequences", "deletealltracks", "loopbarswindow" };

static vector<string> diskNames = vector<string>{ "load", "save", "format", "setup", "device", "loadaprogram",
"saveaprogram", "loadasound", "saveasound", "cantfindfile", "filealreadyexists", "loadasequence",
"saveasequence", "saveapsfile" };

static vector<string> soundNames = vector<string>{ "sound", "deletesound", "deleteallsound", "convertsound", "resample",
"stereotomono", "monotostereo", "copysound" };

static vector<string> soundGuiNames = vector<string>{ "trim", "loop", "zone" };

LayeredScreen::LayeredScreen(mpc::Mpc* mpc) 
{
	moduru::gui::BMFParser bmfParser(mpc::StartUp::resPath + moduru::file::FileUtil::getSeparator() + "font.fnt");
	atlas = bmfParser.getAtlas();
	font = bmfParser.getLoadedFont();

	this->mpc = mpc;
	pixels = std::vector < std::vector <bool>>(248, std::vector<bool>(60));
	popup = make_unique<mpc::lcdgui::Popup>(&atlas, &font);
	popup->Hide(true);

	horizontalBarsTempoChangeEditor = vector<shared_ptr<HorizontalBar>>(4);
	horizontalBarsStepEditor = vector<shared_ptr<HorizontalBar>>(4);
	selectedEventBarsStepEditor = vector<shared_ptr<mpc::lcdgui::SelectedEventBar>>(4);

	verticalBarsMixer = vector<shared_ptr<VerticalBar>>(16);
	mixerFaderBackgrounds = vector<shared_ptr<MixerFaderBackground>>(16);
	mixerTopBackgrounds = vector<shared_ptr<MixerTopBackground>>(16);
	knobs = vector<shared_ptr<Knob>>(16);


	underline = make_shared<mpc::lcdgui::Underline>();
	envGraph = make_shared<mpc::lcdgui::EnvGraph>(mpc);

	int x, y, w, h;
	MRECT rect;
	for (int i = 0; i < 4; i++) {
		w = 193;
		h = 9;
		x = 0;
		y = 11 + (i * 9);
		rect = MRECT(x, y, x + w, y + h);
		selectedEventBarsStepEditor[i] = make_shared<mpc::lcdgui::SelectedEventBar>(rect);
		selectedEventBarsStepEditor[i]->Hide(true);
		nonTextComps.push_back(selectedEventBarsStepEditor[i]);

		w = 33;
		h = 5;
		x = 191;
		y = 13 + (i * 9);
		rect = MRECT(x, y, x + w, y + h);
		horizontalBarsTempoChangeEditor[i] = make_shared<HorizontalBar>(rect, 0);
		horizontalBarsTempoChangeEditor[i]->Hide(true);

		w = 50;
		x = 198;
		rect = MRECT(x, y, x + w, y + h);
		horizontalBarsStepEditor[i] = make_shared<HorizontalBar>(rect, 0);
		horizontalBarsStepEditor[i]->Hide(true);
		nonTextComps.push_back(horizontalBarsStepEditor[i]);
	}

	for (int i = 0; i < 16; i++) {
		w = 13;
		h = 34;
		x = 4 + (i * 15);
		y = 15;
		rect = MRECT(x, y, x + w, y + h);
		mixerFaderBackgrounds[i] = make_shared<MixerFaderBackground>(rect);
		mixerFaderBackgrounds[i]->Hide(true);
		nonTextComps.push_back(mixerFaderBackgrounds[i]);

		w = 13;
		h = 12;
		x = 4 + (i * 15);
		y = 0;
		rect = MRECT(x, y, x + w, y + h);
		mixerTopBackgrounds[i] = make_shared<MixerTopBackground>(rect);
		mixerTopBackgrounds[i]->Hide(true);
		nonTextComps.push_back(mixerTopBackgrounds[i]);

		w = 5;
		h = 37;
		x = 12 + (i * 15);
		y = 16;
		rect = MRECT(x, y, x + w, y + h);
		verticalBarsMixer[i] = make_shared<VerticalBar>(rect);
		verticalBarsMixer[i]->Hide(true);
		nonTextComps.push_back(verticalBarsMixer[i]);

		w = 13;
		h = 13;
		x = 5 + (i * 15);
		y = 1;
		rect = MRECT(x, y, x + w, y + h);
		knobs[i] = make_shared<Knob>(rect);
		knobs[i]->Hide(true);
	}

	fineWave = make_shared <mpc::lcdgui::Wave>();
	fineWave->setFine(true);
	//fineWave->Hide(true);

	wave = make_shared<mpc::lcdgui::Wave>();
	wave->Hide(true);
	wave->setFine(false);
	nonTextComps.push_back(wave);

	twoDots = make_shared<TwoDots>();
	//twoDots->Hide(true);

	int effectWidth = 30;
	int effectHeight = 14;
	int effectDistance = 5;
	int effectOffset = 42;
	//int effectY = 36; // when effect is off
	int effectY = 23; // when effect is on
	for (int i = 0; i < 6; i++) {
		int x = (i * (effectWidth + effectDistance)) + effectOffset;
		MRECT r(x, effectY, x + effectWidth - 1, effectY + effectHeight - 1);
		auto effect = make_shared<Effect>(r);
		effect->Hide(true);
		nonTextComps.push_back(effect);
		effects.push_back(move(effect));
	}

	FILE* fp0 = fopen(string(mpc::StartUp::resPath + "mainpanel.json").c_str(), "r"); // non-Windows use "r"
	FILE* fp1 = fopen(string(mpc::StartUp::resPath + "windowpanel.json").c_str(), "r"); // non-Windows use "r"
	FILE* fp2 = fopen(string(mpc::StartUp::resPath + "dialogpanel.json").c_str(), "r"); // non-Windows use "r"
	FILE* fp3 = fopen(string(mpc::StartUp::resPath + "dialog2panel.json").c_str(), "r"); // non-Windows use "r"

	FILE* fPointers[LAYER_COUNT]{ fp0, fp1, fp2, fp3 };

	for (int i = 0; i < LAYER_COUNT; i++) {
		char readBuffer[65536 * 2];
		layers[i] = make_unique<Layer>(this, &atlas, &font);
		FileReadStream is(fPointers[i], readBuffer, sizeof(readBuffer));
		layerJsons[i].ParseStream(is);
		fclose(fPointers[i]);
	}
}

weak_ptr<mpc::lcdgui::EnvGraph> LayeredScreen::getEnvGraph() {
	return envGraph;
}

vector<weak_ptr<Effect>> LayeredScreen::getEffects() {
	vector<weak_ptr<Effect>> res;
	for (auto& e : effects)
		res.push_back(e);
	return res;
}

int LayeredScreen::getCurrentParamIndex() {
	int currentIndex;
	Layer* l = layers[currentLayer].get();
	auto params = l->getParameters();
	int size = params.size();
	for (currentIndex = 0; currentIndex <= size; currentIndex++) {
		if (currentIndex == size) break;
		if (params[currentIndex]->getTf().lock()->getName().compare(l->getFocus()) == 0) {
			break;
		}
	}
	if (currentIndex == size) return -1;
	return currentIndex;
}

void LayeredScreen::transferFocus(bool backwards) {
	int currentIndex, candidateIndex;
	auto params = layers[currentLayer]->getParameters();
	int size = params.size();
	currentIndex = getCurrentParamIndex();
	if (currentIndex == -1) return;
	if (backwards && currentIndex == 0) return;
	if (!backwards && currentIndex == size - 1) return;

	bool success = false;
	while (success == false) {
		candidateIndex = backwards ? currentIndex-- - 1 : currentIndex++ + 1;
		if (candidateIndex >= 0 && candidateIndex < size) {
			if (!params[candidateIndex]->getTf().lock()->IsHidden()) {
				success = true;
			}
		}
		else {
			break;
		}
	}
	if (!success) return;

	layers[currentLayer]->setFocus(params[candidateIndex]->getTf().lock()->getName());
}

int LayeredScreen::openScreen(string screenName) {
	if (currentScreenName.compare(screenName) == 0) {
		return -1;
	}

	auto ams = mpc->getAudioMidiServices().lock();
	if (currentScreenName.compare("sample") == 0) {
		ams->muteMonitor(true);
		ams->getSoundRecorder().lock()->setVuMeterActive(false);
	}
	else if (screenName.compare("sample") == 0) {
		bool muteMonitor = mpc->getUis().lock()->getSamplerGui()->getMonitor() == 0 ? true : false;
		ams->muteMonitor(muteMonitor);
		ams->getSoundRecorder().lock()->setVuMeterActive(true);
	}

	setLastFocus(currentScreenName, getFocus());

	previousScreenName = currentScreenName;
	currentScreenName = screenName;
	std::string firstField = "";
	int oldLayer = currentLayer;
	for (int i = 0; i < LAYER_COUNT; i++) {
		if (layerJsons[i].HasMember(screenName.c_str())) {
			currentLayer = i;
			if (oldLayer > currentLayer) {
				for (int j = oldLayer; j > currentLayer; j--) {
					layers[j]->clear();
				}
			}
			layers[currentLayer]->clear();
			firstField = layers[currentLayer]->openScreen(layerJsons[i][screenName.c_str()], screenName);
			break;
		}
	}
	
	if (oldLayer == 0 && oldLayer == currentLayer) {
		wave->Hide(true);
	}
	
	returnToLastFocus(firstField);

	initObserver();
	return currentLayer;
}

std::vector<std::vector<bool> >* LayeredScreen::getPixels() {
	return &pixels;
}

void LayeredScreen::Draw() {
	std::set<std::shared_ptr<mpc::lcdgui::Component>> dirtyComponents;
	for (int i = 0; i <= currentLayer; i++) {

		auto components = layers[i]->getComponentsThatNeedClearing();
		for (auto& c : components) {
			c.lock()->Clear(&pixels);
			dirtyComponents.insert(c.lock());
		}
		if (twoDots->NeedsClearing()) {
			twoDots->Clear(&pixels);
			dirtyComponents.insert(twoDots);
		}
		if (envGraph->NeedsClearing()) {
			envGraph->Clear(&pixels);
			dirtyComponents.insert(envGraph);
		}

		for (auto& c : nonTextComps) {
			if (c.lock()->NeedsClearing()) {
				c.lock()->Clear(&pixels);
				dirtyComponents.insert(c.lock());
			}
		}

		if (popup->NeedsClearing()) {
			popup->Clear(&pixels);
			dirtyComponents.insert(popup);
		}

		if (i == 1) {
			for (auto& hbar : horizontalBarsTempoChangeEditor) {
				if (hbar->NeedsClearing()) {
					hbar->Clear(&pixels);
					dirtyComponents.insert(hbar);
				}
			}
		}

		for (auto& k : knobs) {
			if (k->NeedsClearing()) {
				k->Clear(&pixels);
				dirtyComponents.insert(k);
			}
		}

		if (layers[i]->getBackground()->IsDirty()) {
			layers[i]->getBackground()->Draw(&pixels);
			dirtyComponents.insert(layers[i]->getBackgroundWk().lock());
		}

		if (i == currentLayer && envGraph->IsDirty() && !envGraph->IsHidden()) {
			envGraph->Draw(&pixels);
			dirtyComponents.insert(envGraph);
		}

		for (auto& c : nonTextComps) {
			if (c.lock()->IsDirty() && !c.lock()->IsHidden()) {
				string name = typeid(*c.lock().get()).name();
				c.lock()->Draw(&pixels);
				dirtyComponents.insert(c.lock());
			}
		}

		if (i == 1) {
			for (auto& hbar : horizontalBarsTempoChangeEditor) {
				if (hbar->IsDirty() && !hbar->IsHidden()) {
					hbar->Draw(&pixels);
					dirtyComponents.insert(hbar);
				}
			}
		}

		components = layers[i]->getAllLabelsAndFields();
		for (auto& c : components) {
			if (c.lock()->IsDirty() && !c.lock()->IsHidden()) {
				c.lock()->Draw(&pixels);
				dirtyComponents.insert(c.lock());
			}
		}

		if (!underline->IsHidden() && underline->IsDirty() && i == 2) {
			underline->Draw(&pixels);
			dirtyComponents.insert(underline);
		}

		if (!twoDots->IsHidden() && twoDots->IsDirty()) {
			twoDots->Draw(&pixels);
			dirtyComponents.insert(twoDots);
		}
		if (i == 1 && fineWave->IsDirty()) {
			fineWave->Draw(&pixels);
			dirtyComponents.insert(fineWave);
		}
		if (layers[i]->getFunctionKeys()->IsDirty()) {
			layers[i]->getFunctionKeys()->Draw(&pixels);
			dirtyComponents.insert(layers[i]->getFunctionKeysWk().lock());
		}
		for (auto& k : knobs) {
			if (!k->IsHidden() && k->IsDirty()) {
				k->Draw(&pixels);
				dirtyComponents.insert(k);
			}
		}
	}

	if (popup->IsDirty() && !popup->IsHidden()) popup->Draw(&pixels);

	for (auto& c : dirtyComponents) {
		dirtyArea = dirtyArea.Union(c->getDirtyArea());
		c->getDirtyArea()->Clear();
	}
}

MRECT* LayeredScreen::getDirtyArea() {
	return &dirtyArea;
}

bool LayeredScreen::IsDirty() {
	for (int i = 0; i <= currentLayer; i++) {
		//MLOG("\n checking for dirty components in layer " + to_string(i));
		if (layers[i]->getBackground()->IsDirty() || layers[i]->getFunctionKeys()->IsDirty()) {
			//MLOG("background or fk is dirty");
			return true;
		}

		auto components = layers[i]->getAllLabels();
		for (auto& c : components) {
			if (c.lock()->IsDirty() && !(c.lock()->IsHidden() && !c.lock()->NeedsClearing())) {
				//MLOG("label " + dynamic_pointer_cast<Label>(c.lock())->getName() + " is dirty");
				return true;
			}
		}

		components = layers[i]->getAllFields();
		for (auto& c : components) {
			if (c.lock()->IsDirty() && !(c.lock()->IsHidden() && !c.lock()->NeedsClearing())) {
				//MLOG("field " + dynamic_pointer_cast<Field>(c.lock())->getName() + " is dirty");
				return true;
			}
		}
		if (layers[i]->getFunctionKeys()->IsDirty()) return true;
		for (auto& c : nonTextComps) {
			if (c.lock()->IsDirty() && !(c.lock()->IsHidden() && !c.lock()->NeedsClearing())) {
				//MLOG("a nontextcomp is dirty");
				return true;
			}
		}

		if (underline->IsDirty() && i == 2) {
			//MLOG("underline in layer 2 is dirty");
			return true;
		}
		for (auto& k : knobs) {
			if (k->IsDirty() && !(k->IsHidden() && !k->NeedsClearing())) {
				//MLOG("a knob is dirty");
				return true;
			}
		}
		if (i == 1 && fineWave->IsDirty()) {
			//MLOG("finewave is dirty");
			return true;
		}
	}
	if (popup->IsDirty() && !(popup->IsHidden() && !popup->NeedsClearing())) return true;
	if (twoDots->IsDirty() && !(twoDots->IsHidden() && !twoDots->NeedsClearing())) {
		//MLOG("twodots is dirty");
		return true;
	}
	return false;
}

Layer* LayeredScreen::getLayer(int i) {
	return layers[i].get();
}

void LayeredScreen::createPopup(string text, int textXPos)
{
	popup->Hide(false);
	popup->setText(text, textXPos);
}

mpc::lcdgui::Background* LayeredScreen::getCurrentBackground()
{
	return layers[currentLayer]->getBackground();
}

void LayeredScreen::removeCurrentBackground()
{
	layers[currentLayer]->getBackground()->setName("");
}

void LayeredScreen::setCurrentBackground(string s)
{
	getCurrentBackground()->setName(s);
}

void LayeredScreen::removePopup()
{
	popup->Hide(true);
}

void LayeredScreen::setPopupText(string text)
{
	popup->setText(text, 0);
}

void LayeredScreen::returnToLastFocus(string firstFieldOfThisScreen)
{
	auto focusCounter = 0;
	
	for (auto& lf : lastFocus) {
		if (lf[0].compare(currentScreenName) == 0) {
			focusCounter++;
			setFocus(lf[1]);
		}
	}

	if (focusCounter == 0) {
		vector<string> sa(2);
		sa[0] = currentScreenName;
		sa[1] = firstFieldOfThisScreen;
		lastFocus.push_back(sa);
		setFocus(firstFieldOfThisScreen);
	}
}

void LayeredScreen::redrawEnvGraph(int attack, int decay)
{
	vector<int> line1 { 75, 43, 75 + (int)(attack * 0.17), 24 };
	vector<int> line2 { 119 - (int)(decay * 0.17), 24, 119, 43 };
	vector<int> line3  { 75 + (int)(attack * 0.17), 24, 119 - (int)(decay * 0.17), 24 };
	vector<vector<int> > lines = { line1, line2, line3 };
	envGraph->setCoordinates(lines);
}

void LayeredScreen::setLastFocus(string screenName, string tfName)
{
	for (auto& lf : lastFocus) {
		if (lf[0].compare(screenName) == 0) {
			lf[1] = tfName;
		}
	}
}

string LayeredScreen::getLastFocus(string screenName)
{
	string tfName = "";
	for (auto& lf : lastFocus) {
		if (lf[0].compare(screenName) == 0) {
			tfName = lf[1];
		}
	}
	return tfName;
}

void LayeredScreen::setCurrentScreenName(string screenName)
{
	currentScreenName = screenName;
}

string LayeredScreen::getCurrentScreenName()
{
	return currentScreenName;
}

void LayeredScreen::setPreviousScreenName(string screenName)
{
	previousScreenName = screenName;
}

string LayeredScreen::getPreviousScreenName()
{
	return previousScreenName;
}

mpc::lcdgui::Popup* LayeredScreen::getPopup() {
	return popup.get();
}

string LayeredScreen::getPreviousFromNoteText()
{
	return previousFromNoteText;
}

void LayeredScreen::setPreviousFromNoteText(string text)
{
	previousFromNoteText = text;
}

void LayeredScreen::setPreviousViewModeText(string text)
{
	previousViewModeText = text;
}

string LayeredScreen::getPreviousViewModeText()
{
	return previousViewModeText;
}

vector<weak_ptr<mpc::lcdgui::HorizontalBar>> LayeredScreen::getHorizontalBarsTempoChangeEditor()
{
	auto res = vector<weak_ptr<mpc::lcdgui::HorizontalBar>>();
	for (auto& b : horizontalBarsTempoChangeEditor)
		res.push_back(b);
	return res;
}

vector<weak_ptr<mpc::lcdgui::HorizontalBar>> LayeredScreen::getHorizontalBarsStepEditor()
{
	auto res = vector<weak_ptr<mpc::lcdgui::HorizontalBar>>();
	for (auto& b : horizontalBarsStepEditor)
		res.push_back(b);
	return res;
}

vector<weak_ptr<mpc::lcdgui::VerticalBar>> LayeredScreen::getVerticalBarsMixer()
{
	auto res = vector<weak_ptr<mpc::lcdgui::VerticalBar>>();
	for (auto& b : verticalBarsMixer)
		res.push_back(b);
	return res;
}

vector<weak_ptr<mpc::lcdgui::SelectedEventBar>> LayeredScreen::getSelectedEventBarsStepEditor()
{
	auto res = vector<weak_ptr<mpc::lcdgui::SelectedEventBar>>();
	for (auto& b : selectedEventBarsStepEditor)
		res.push_back(b);
	return res;
}

FunctionKeys* LayeredScreen::getFunctionKeys()
{
	return layers[currentLayer]->getFunctionKeys();
}

vector<weak_ptr<mpc::lcdgui::Knob>> LayeredScreen::getKnobs()
{
	auto res = vector<weak_ptr<mpc::lcdgui::Knob>>();
	for (auto& b : knobs)
		res.push_back(b);
	return res;
}

vector<weak_ptr<mpc::lcdgui::MixerFaderBackground>> LayeredScreen::getMixerFaderBackgrounds()
{
	auto res = vector<weak_ptr<mpc::lcdgui::MixerFaderBackground>>();
	for (auto& b : mixerFaderBackgrounds)
		res.push_back(b);
	return res;
}

vector<weak_ptr<mpc::lcdgui::MixerTopBackground>> LayeredScreen::getMixerTopBackgrounds()
{
	auto res = vector<weak_ptr<mpc::lcdgui::MixerTopBackground>>();
	for (auto& b : mixerTopBackgrounds)
		res.push_back(b);
	return res;
}

void LayeredScreen::drawFunctionKeys(string screenName)
{
	auto& screenJson = layerJsons[currentLayer][screenName.c_str()];
	Value& fblabels = screenJson["fblabels"];
	Value& fbtypes = screenJson["fbtypes"];
	getFunctionKeys()->clearAll(&pixels);
	//getCurrentBackground()->SetDirty(); // only redraw fk area
	getFunctionKeys()->initialize(fblabels, fbtypes);
}

std::weak_ptr<mpc::lcdgui::TwoDots> LayeredScreen::getTwoDots()
{
	return twoDots;
}

std::weak_ptr<Wave> LayeredScreen::getWave() {
	return wave;
}

std::weak_ptr<Wave> LayeredScreen::getFineWave()
{
	return fineWave;
}

int LayeredScreen::getCurrentLayer() {
	return currentLayer;
}

weak_ptr<Field> LayeredScreen::findBelow(weak_ptr<Field> tf0) {
	int marginChars = 8;
	int minDistV = 7;
	int maxDistH = 6 * marginChars;
	weak_ptr<Field> result = tf0;
	auto lTf0 = tf0.lock();
	for (auto& a : layers[currentLayer]->getAllFields()) {
		auto tf1 = dynamic_pointer_cast<Field>(a.lock());
		auto B1 = tf1->getY() + tf1->getH();
		auto B0 = lTf0->getY() + lTf0->getH();
		auto MW1 = 0.5f * (float)(tf1->getX()*2 + tf1->getW());
		auto MW0 = 0.5f * (float)(lTf0->getX() * 2 + lTf0->getW());
		if (B1 - B0 >= minDistV) {
			if (abs((int)(MW1 - MW0)) <= maxDistH) {
				if (!tf1->IsHidden() && tf1->isFocusable()) {
					result = tf1;
					break;
				}
			}
		}
	}

	if (result.lock() == lTf0) {
		marginChars = 16;
		maxDistH = 6 * marginChars;
		for (auto& a : layers[currentLayer]->getAllFields()) {
			auto tf1 = dynamic_pointer_cast<Field>(a.lock());
			auto B1 = tf1->getY() + tf1->getH();
			auto B0 = lTf0->getY() + lTf0->getH();
			auto MW1 = 0.5f * (float)(tf1->getX() * 2 + tf1->getW());
			auto MW0 = 0.5f * (float)(lTf0->getX() * 2 + lTf0->getW());
			if (B1 - B0 >= minDistV) {
				if (abs((int)(MW1 - MW0)) <= maxDistH) {
					if (!tf1->IsHidden() && tf1->isFocusable()) {
						result = tf1;
						break;
					}
				}
			}
		}
	}
	return result;
}

weak_ptr<Field> LayeredScreen::findAbove(weak_ptr<Field> tf0) {
	int marginChars = 8;
	int minDistV = - 7;
	int maxDistH = 6 * marginChars;
	weak_ptr<Field> result = tf0;
	auto lTf0 = tf0.lock();
	auto revComponents = layers[currentLayer]->getAllFields();
	reverse(revComponents.begin(), revComponents.end());
	for (auto& a : revComponents) {
		auto tf1 = dynamic_pointer_cast<Field>(a.lock());
		auto B1 = tf1->getY() + tf1->getH();
		auto B0 = lTf0->getY() + lTf0->getH();
		auto MW1 = 0.5f * (float)(tf1->getX() * 2 + tf1->getW());
		auto MW0 = 0.5f * (float)(lTf0->getX() * 2 + lTf0->getW());
		if (B1 - B0 <= minDistV) {
			if (abs((int)(MW1 - MW0)) <= maxDistH) {
				if (!tf1->IsHidden() && tf1->isFocusable()) {
					result = tf1;
					break;
				}
			}
		}
	}
	if (result.lock() == lTf0) {
		marginChars = 16;
		maxDistH = 6 * marginChars;
		for (auto& a : revComponents) {
			auto tf1 = dynamic_pointer_cast<Field>(a.lock());
			auto B1 = tf1->getY() + tf1->getH();
			auto B0 = lTf0->getY() + lTf0->getH();
			auto MW1 = 0.5f * (float)(tf1->getX() * 2 + tf1->getW());
			auto MW0 = 0.5f * (float)(lTf0->getX() * 2 + lTf0->getW());
			if (B1 - B0 <= minDistV) {
				if (abs((int)(MW1 - MW0)) <= maxDistH) {
					if (!tf1->IsHidden() && tf1->isFocusable()) {
						result = tf1;
						break;
					}
				}
			}
		}
	}
	return result;
}

string LayeredScreen::findBelow(string tf0) {
	string result = tf0;
	for (auto& a : layers[currentLayer]->getAllFields()) {
		auto candidate = dynamic_pointer_cast<Field>(a.lock());
		if (candidate->getName().compare(tf0) == 0) {
			result = findBelow(candidate).lock()->getName();
			break;
		}
	}
	return result;
}

string LayeredScreen::findAbove(string tf0) {
	string result = tf0;
	for (auto& a : layers[currentLayer]->getAllFields()) {
		auto candidate = dynamic_pointer_cast<Field>(a.lock());
		if (candidate->getName().compare(tf0) == 0) {
			result = findAbove(candidate).lock()->getName();
			break;
		}
	}
	return result;
}

std::weak_ptr<mpc::lcdgui::Underline> LayeredScreen::getUnderline() {
	return underline;
}

std::weak_ptr<Field> LayeredScreen::lookupField(std::string s)
{
	for (auto& a : layers[currentLayer]->getAllFields() ) {
		auto candidate = dynamic_pointer_cast<Field>(a.lock());
		if (candidate->getName().compare(s) == 0) {
			return candidate;
		}
	}
	return weak_ptr<Field>();
}

std::weak_ptr<Label> LayeredScreen::lookupLabel(std::string s)
{
	for (auto& a : layers[currentLayer]->getAllLabels()) {
		auto candidate = dynamic_pointer_cast<Label>(a.lock());
		if (candidate->getName().compare(s) == 0) {
			return candidate;
		}
	}
	return weak_ptr<Label>();
}

static inline bool checkActiveScreen(vector<string>* sa, string csn)
{
	for (auto s : *sa) {
		if (csn.compare(s) == 0)
			return true;
	}
	return false;
}

void LayeredScreen::initObserver()
{

	auto csn = currentScreenName;

	if (activeObserver) {
		activeObserver.reset();
	}

	if (csn.compare("directtodiskrecorder") == 0) {
		activeObserver = make_unique<DirectToDiskRecorderObserver>(mpc);
	}
	else if (csn.compare("disk") == 0) {
		activeObserver = make_unique<VmpcDiskObserver>(mpc);
	}
	else if (csn.compare("punch") == 0) {
		activeObserver = make_unique<PunchObserver>(mpc);
	}
	else if (csn.compare("trans") == 0) {
		activeObserver = make_unique<TransObserver>(mpc);
	}
	else if (csn.compare("2ndseq") == 0) {
		activeObserver = make_unique<SecondSeqObserver>(mpc);
	}
	else if (csn.compare("others") == 0) {
		activeObserver = make_unique<OthersObserver>(mpc);
	}
	else if (csn.compare("erase") == 0) {
		activeObserver = make_unique<EraseObserver>(mpc);
	}
	else if (csn.compare("sync") == 0) {
		activeObserver = make_unique<SyncObserver>(mpc);
	}
	else if (csn.compare("assign") == 0) {
		activeObserver = make_unique<AssignObserver>(mpc);
	}
	else if (csn.compare("assign16levels") == 0) {
		activeObserver = make_unique<Assign16LevelsObserver>(mpc);
	}
	else if (csn.compare("metronomesound") == 0) {
		activeObserver = make_unique<MetronomeSoundObserver>(mpc);
	}
	else if (csn.compare("saveallfile") == 0) {
		activeObserver = make_unique<SaveAllFileObserver>(mpc);
	}
	else if (csn.compare("loadasequencefromall") == 0) {
		activeObserver = make_unique<LoadASequenceFromAllObserver>(mpc);
	}
	else if (csn.compare("nextseqpad") == 0) {
		activeObserver = make_unique<NextSeqPadObserver>(mpc);
	}
	else if (csn.compare("nextseq") == 0) {
		activeObserver = make_unique<NextSeqObserver>(mpc);
	}
	else if (csn.compare("song") == 0) {
		activeObserver = make_unique<SongObserver>(mpc);
	}
	else if (csn.compare("trackmute") == 0) {
		activeObserver = make_unique<TrMuteObserver>(mpc);
	}
	else if (checkActiveScreen(&diskNames, currentScreenName)) {
		activeObserver = make_unique<DiskObserver>(mpc);
	}
	else if (checkActiveScreen(&seqWindowNames, currentScreenName)) {
		activeObserver = make_unique<SequencerWindowObserver>(mpc);
	}
	else if (checkActiveScreen(&soundNames, currentScreenName)) {
		activeObserver = make_unique<SoundObserver>(mpc);
	}
	else if (csn.compare("sample") == 0) {
		activeObserver = make_unique<SampleObserver>(mpc);
	}
	else 
	if (csn.compare("sequencer") == 0) {
		activeObserver = make_unique<SequencerObserver>(mpc);
	}
	else if (csn.compare("directory") == 0) {
		activeObserver = make_unique<DirectoryObserver>(mpc->getDisk(), mpc);
	}
	else if (csn.compare("programparams") == 0) {
		activeObserver = make_unique<PgmParamsObserver>(mpc);
	}
	else if (csn.compare("programassign") == 0) {
		activeObserver = make_unique<PgmAssignObserver>(mpc);
	}
	else if (csn.compare("sequencer_step") == 0) {
		activeObserver = make_unique<StepEditorObserver>(mpc);
	}
	else if (csn.compare("step_tc") == 0 || csn.compare("editmultiple") == 0 || csn.compare("insertevent") == 0) {
		activeObserver = make_unique<StepWindowObserver>(mpc);
	}
	else if (csn.compare("mixerv2") == 0 || csn.compare("mixersetup") == 0) {
		activeObserver = make_unique<MixerObserver>(mpc);
	}
	else if (csn.compare("fxedit") == 0) {
		activeObserver = make_unique<FxEditObserver>(mpc);
	}
	else if (csn.compare("channelsettings") == 0) {
		activeObserver = make_unique<ChannelSettingsObserver>(mpc);
	}
	else if (csn.compare("edit") == 0) {
		activeObserver = make_unique<EditSequenceObserver>(mpc);
	}
	else if (csn.compare("name") == 0) {
		activeObserver = make_unique<ui::NameObserver>(mpc);
	}
	else if (csn.compare("midiinputmonitor") == 0 || csn.compare("midioutputmonitor") == 0) {
		activeObserver = make_unique<MidiMonitorObserver>(mpc);
	}
	else if (csn.compare("barcopy") == 0) {
		activeObserver = make_unique<BarCopyObserver>(mpc);
	}
	else if (csn.compare("trmove") == 0) {
		activeObserver = make_unique<TrMoveObserver>(mpc);
	}
	else if (csn.compare("user") == 0) {
		activeObserver = make_unique<UserObserver>(mpc);
	}
	else if (csn.compare("trim") == 0) {
		activeObserver = make_unique<TrimObserver>(mpc);
	}
	else if (csn.compare("loop") == 0) {
		activeObserver = make_unique<LoopObserver>(mpc);
	}
	else if (csn.compare("editsound") == 0) {
		activeObserver = make_unique<EditSoundObserver>(mpc);
	}
	else if (csn.find("startfine") != string::npos || csn.find("endfine") != string::npos || csn.compare("looptofine") == 0) {
		activeObserver = make_unique<ZoomObserver>(mpc);
	}
	else if (csn.compare("zone") == 0 || csn.compare("numberofzones") == 0) {
		activeObserver = make_unique<ZoneObserver>(mpc);
	}
	else if (csn.compare("params") == 0) {
		activeObserver = make_unique<SndParamsObserver>(mpc);
	}
	else if (csn.compare("deleteallfiles") == 0) {
		activeObserver = make_unique<DeleteAllFilesObserver>(mpc);
	}
	else if (checkActiveScreen(&samplerWindowNames, csn)) {
		activeObserver = make_unique<SamplerWindowObserver>(mpc);
	}
	else if (csn.compare("purge") == 0) {
		activeObserver = make_unique<PurgeObserver>(mpc);
	}
	else if (csn.compare("drum") == 0) {
		activeObserver = make_unique<DrumObserver>(mpc);
	}
	else if (csn.compare("muteassign") == 0) {
		activeObserver = make_unique<MuteAssignObserver>(mpc);
	}
}

string LayeredScreen::getFocus() {
	return layers[currentLayer]->getFocus();
}

void LayeredScreen::setFocus(string focus) {
	layers[currentLayer]->setFocus(focus);
}

LayeredScreen::~LayeredScreen() {
	/*
	* We want to make sure the active observer's destructor can still access Components
	*/ 
	activeObserver.reset();
}
