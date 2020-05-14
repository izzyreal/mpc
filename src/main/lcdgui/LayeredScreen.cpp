#include "lcdgui/LayeredScreen.hpp"

#include "Screens.hpp"

#include <gui/BasicStructs.hpp>

#include <Mpc.hpp>
#include <Paths.hpp>

#include "Field.hpp"
#include "Component.hpp"

#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/SoundRecorder.hpp>

#include <lcdgui/Layer.hpp>
#include <lcdgui/ScreenComponent.hpp>
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
#include <ui/vmpc/VmpcSettingsObserver.hpp>
#include <ui/vmpc/VmpcDiskObserver.hpp>

#include <file/FileUtil.hpp>

#include <rapidjson/filereadstream.h>

#include <cmath>
#include <set>

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

using namespace moduru::file;

using namespace rapidjson;

using namespace std;

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

static moduru::gui::BMFParser _bmfParser = moduru::gui::BMFParser(string(mpc::Paths::resPath() + moduru::file::FileUtil::getSeparator() + "font.fnt"));

std::vector<std::vector<bool>> LayeredScreen::atlas = _bmfParser.getAtlas();
moduru::gui::bmfont LayeredScreen::font = _bmfParser.getLoadedFont();

LayeredScreen::LayeredScreen() 
{	
	root = make_unique<Component>("root");

	pixels = vector<vector<bool>>(248, vector<bool>(60));
	popup = make_unique<Popup>();
	popup->Hide(true);

	horizontalBarsTempoChangeEditor = vector<shared_ptr<HorizontalBar>>(4);
	horizontalBarsStepEditor = vector<shared_ptr<HorizontalBar>>(4);
	selectedEventBarsStepEditor = vector<shared_ptr<SelectedEventBar>>(4);

	verticalBarsMixer = vector<shared_ptr<VerticalBar>>(16);
	mixerFaderBackgrounds = vector<shared_ptr<MixerFaderBackground>>(16);
	mixerTopBackgrounds = vector<shared_ptr<MixerTopBackground>>(16);
	knobs = vector<shared_ptr<Knob>>(16);


	underline = make_shared<Underline>();
	envGraph = make_shared<EnvGraph>();

	int x, y, w, h;
	MRECT rect;
	for (int i = 0; i < 4; i++) {
		w = 193;
		h = 9;
		x = 0;
		y = 11 + (i * 9);
		rect = MRECT(x, y, x + w, y + h);
		selectedEventBarsStepEditor[i] = make_shared<SelectedEventBar>(rect);
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

	fineWave = make_shared <Wave>();
	fineWave->setFine(true);
	//fineWave->Hide(true);

	wave = make_shared<Wave>();
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

	shared_ptr<Layer> previousLayer;

	for (int i = 0; i < LAYER_COUNT; i++)
	{
		auto layer = make_shared<Layer>();
		layers.push_back(layer);
		if (previousLayer)
		{
			previousLayer->addChild(layer);
		}
		else
		{
			root->addChild(layer);
		}
		previousLayer = layer;
	}
}

weak_ptr<ScreenComponent> LayeredScreen::findScreenComponent()
{
	return getFocusedLayer().lock()->findScreenComponent();
}

weak_ptr<EnvGraph> LayeredScreen::getEnvGraph() {
	return envGraph;
}

vector<weak_ptr<Effect>> LayeredScreen::getEffects() {
	vector<weak_ptr<Effect>> res;
	for (auto& e : effects)
		res.push_back(e);
	return res;
}

int LayeredScreen::getCurrentFieldIndex() {
	int currentIndex;
	auto layer = layers[focusedLayerIndex].lock();
	auto fields = layer->findFields();
	int size = fields.size();
	
	for (currentIndex = 0; currentIndex <= size; currentIndex++) {
		if (currentIndex == size) break;
		if (fields[currentIndex].lock()->getName().compare(layer->getFocus()) == 0) {
			break;
		}
	}
	
	if (currentIndex == size) {
		return -1;
	}
	return currentIndex;
}

void LayeredScreen::transferFocus(bool backwards) {
	int currentIndex, candidateIndex;
	auto layer = layers[focusedLayerIndex].lock();
	auto fields = layer->findFields();
	int size = fields.size();
	
	currentIndex = getCurrentFieldIndex();
	
	if (currentIndex == -1)
	{
		return;
	}
	
	if (backwards && currentIndex == 0)
	{
		return;
	}

	if (!backwards && currentIndex == size - 1)
	{
		return;
	}

	bool success = false;
	
	while (!success)
	{
		candidateIndex = backwards ? currentIndex-- - 1 : currentIndex++ + 1;
		
		if (candidateIndex >= 0 && candidateIndex < size) {
			if (!fields[candidateIndex].lock()->IsHidden()) {
				success = true;
			}
		}
		else {
			break;
		}
	}

	if (!success)
	{
		return;
	}

	layer->setFocus(fields[candidateIndex].lock()->getName());
}

int LayeredScreen::openScreen(string screenName) {

	if (currentScreenName.compare(screenName) == 0) {
		return -1;
	}

	auto ams = Mpc::instance().getAudioMidiServices().lock();

	if (currentScreenName.compare("sample") == 0)
	{
		ams->muteMonitor(true);
		ams->getSoundRecorder().lock()->setVuMeterActive(false);
	}
	else if (screenName.compare("sample") == 0)
	{
		bool muteMonitor = Mpc::instance().getUis().lock()->getSamplerGui()->getMonitor() == 0 ? true : false;
		ams->muteMonitor(muteMonitor);
		ams->getSoundRecorder().lock()->setVuMeterActive(true);
	}

	if (currentScreenName.compare("erase") == 0 || currentScreenName.compare("timingcorrect") == 0)
	{
		// This field may not be visible the next time we visit this screen.
		// Like the real 2KXL we always set focus to the first Notes: field
		// if the current focus is hte second Notes: field.
		if (getFocus().compare("notes1") == 0)
		{
			setFocus("notes0");
		}
	}

	setLastFocus(currentScreenName, getFocus());

	previousScreenName = currentScreenName;
	currentScreenName = screenName;
		
	auto screenComponent = Screens::getScreenComponent(currentScreenName);

	if (!screenComponent)
	{
		return -1;
	}

	auto oldScreenComponent = getFocusedLayer().lock()->findScreenComponent().lock();

	if (oldScreenComponent)
	{
		oldScreenComponent->close();
		getFocusedLayer().lock()->removeChild(oldScreenComponent);
	}

	focusedLayerIndex = screenComponent->getLayerIndex();

	getFocusedLayer().lock()->addChild(screenComponent);
	
	screenComponent->SetDirtyRecursive();

	screenComponent->open();

	returnToLastFocus(screenComponent->findFields().front().lock()->getName());

	return focusedLayerIndex;
}

vector<vector<bool>>* LayeredScreen::getPixels() {
	return &pixels;
}

void LayeredScreen::Draw() {
	MLOG("LayeredScreen::Draw()");
	root->Draw(&pixels);
	return;
}

MRECT LayeredScreen::getDirtyArea() {
	MLOG("LayeredScreen::getDirtyArea()");
	auto dirtyArea = root->getDirtyArea();
	return dirtyArea;
}

bool LayeredScreen::IsDirty() {
	return root->IsDirty();
}

Layer* LayeredScreen::getLayer(int i) {
	return layers[i].lock().get();
}

void LayeredScreen::createPopup(string text, int textXPos)
{
	popup->Hide(false);
	popup->setText(text, textXPos);
}

Background* LayeredScreen::getCurrentBackground()
{
	return getFocusedLayer().lock()->getBackground();
}

void LayeredScreen::removeCurrentBackground()
{
	getFocusedLayer().lock()->getBackground()->setName("");
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
	
	for (auto& lf : lastFocuses) {
		if (lf[0].compare(currentScreenName) == 0) {
			focusCounter++;
			setFocus(lf[1]);
		}
	}

	if (focusCounter == 0) {
		vector<string> sa(2);
		sa[0] = currentScreenName;
		sa[1] = firstFieldOfThisScreen;
		lastFocuses.push_back(sa);
		setFocus(firstFieldOfThisScreen);
	}
}

void LayeredScreen::redrawEnvGraph(int attack, int decay)
{
	vector<int> line1 { 75, 43, 75 + (int)(attack * 0.17), 24 };
	vector<int> line2 { 119 - (int)(decay * 0.17), 24, 119, 43 };
	vector<int> line3  { 75 + (int)(attack * 0.17), 24, 119 - (int)(decay * 0.17), 24 };
	vector<vector<int>> lines = { line1, line2, line3 };
	envGraph->setCoordinates(lines);
}

void LayeredScreen::setLastFocus(string screenName, string newLastFocus)
{
	for (auto& lastFocus : lastFocuses)
	{
		if (lastFocus[0].compare(screenName) == 0)
		{
			lastFocus[1] = newLastFocus;
		}
	}
}

string LayeredScreen::getLastFocus(string screenName)
{
	string tfName = "";
	for (auto& lf : lastFocuses) {
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

Popup* LayeredScreen::getPopup() {
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

vector<weak_ptr<HorizontalBar>> LayeredScreen::getHorizontalBarsTempoChangeEditor()
{
	auto res = vector<weak_ptr<HorizontalBar>>();
	for (auto& b : horizontalBarsTempoChangeEditor)
		res.push_back(b);
	return res;
}

vector<weak_ptr<HorizontalBar>> LayeredScreen::getHorizontalBarsStepEditor()
{
	auto res = vector<weak_ptr<HorizontalBar>>();
	for (auto& b : horizontalBarsStepEditor)
		res.push_back(b);
	return res;
}

vector<weak_ptr<VerticalBar>> LayeredScreen::getVerticalBarsMixer()
{
	auto res = vector<weak_ptr<VerticalBar>>();
	for (auto& b : verticalBarsMixer)
		res.push_back(b);
	return res;
}

vector<weak_ptr<SelectedEventBar>> LayeredScreen::getSelectedEventBarsStepEditor()
{
	auto res = vector<weak_ptr<SelectedEventBar>>();
	for (auto& b : selectedEventBarsStepEditor)
		res.push_back(b);
	return res;
}

FunctionKeys* LayeredScreen::getFunctionKeys()
{
	return getFocusedLayer().lock()->getFunctionKeys();
}

vector<weak_ptr<Knob>> LayeredScreen::getKnobs()
{
	auto res = vector<weak_ptr<Knob>>();
	for (auto& b : knobs)
		res.push_back(b);
	return res;
}

vector<weak_ptr<MixerFaderBackground>> LayeredScreen::getMixerFaderBackgrounds()
{
	auto res = vector<weak_ptr<MixerFaderBackground>>();
	for (auto& b : mixerFaderBackgrounds)
		res.push_back(b);
	return res;
}

vector<weak_ptr<MixerTopBackground>> LayeredScreen::getMixerTopBackgrounds()
{
	auto res = vector<weak_ptr<MixerTopBackground>>();
	for (auto& b : mixerTopBackgrounds)
		res.push_back(b);
	return res;
}

void LayeredScreen::drawFunctionKeys(string screenName)
{
	//auto& screenJson = layerJsons[currentLayer][screenName.c_str()];
	//Value& fblabels = screenJson["fblabels"];
	//Value& fbtypes = screenJson["fbtypes"];
	//getFunctionKeys()->clearAll(&pixels);
	//getCurrentBackground()->SetDirty(); // only redraw fk area
	//getFunctionKeys()->initialize(fblabels, fbtypes);
}

weak_ptr<TwoDots> LayeredScreen::getTwoDots()
{
	return twoDots;
}

weak_ptr<Wave> LayeredScreen::getWave()
{
	return wave;
}

weak_ptr<Wave> LayeredScreen::getFineWave()
{
	return fineWave;
}

int LayeredScreen::getFocusedLayerIndex()
{
	return focusedLayerIndex;
}

std::weak_ptr<Layer> LayeredScreen::getFocusedLayer()
{
	return layers[focusedLayerIndex];
}


weak_ptr<Field> LayeredScreen::findBelow(weak_ptr<Field> tf0) {
	int marginChars = 8;
	int minDistV = 7;
	int maxDistH = 6 * marginChars;
	weak_ptr<Field> result = tf0;
	auto lTf0 = tf0.lock();
	for (auto& a : getFocusedLayer().lock()->findFields()) {
		auto tf1 = a.lock();
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
		for (auto& a : getFocusedLayer().lock()->findFields()) {
			auto tf1 = a.lock();
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
	
	auto revComponents = getFocusedLayer().lock()->findFields();

	reverse(revComponents.begin(), revComponents.end());
	for (auto& a : revComponents) {
		auto tf1 = a.lock();
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
			auto tf1 = a.lock();
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
	for (auto& a : getFocusedLayer().lock()->findFields()) {
		auto candidate = a.lock();
		if (candidate->getName().compare(tf0) == 0) {
			result = findBelow(candidate).lock()->getName();
			break;
		}
	}
	return result;
}

string LayeredScreen::findAbove(string tf0) {
	string result = tf0;
	for (auto& a : getFocusedLayer().lock()->findFields()) {
		auto candidate = a.lock();
		if (candidate->getName().compare(tf0) == 0) {
			result = findAbove(candidate).lock()->getName();
			break;
		}
	}
	return result;
}

weak_ptr<Underline> LayeredScreen::getUnderline() {
	return underline;
}

weak_ptr<Field> LayeredScreen::lookupField(string s)
{
	return root->findField(s);
}

weak_ptr<Label> LayeredScreen::lookupLabel(string s)
{
	return root->findLabel(s);
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
		activeObserver = make_unique<DirectToDiskRecorderObserver>();
	}
	else if (csn.compare("disk") == 0) {
		activeObserver = make_unique<VmpcDiskObserver>();
	}
	else if (csn.compare("punch") == 0) {
		activeObserver = make_unique<PunchObserver>();
	}
	else if (csn.compare("trans") == 0) {
		activeObserver = make_unique<TransObserver>();
	}
	else if (csn.compare("2ndseq") == 0) {
		activeObserver = make_unique<SecondSeqObserver>();
	}
	else if (csn.compare("others") == 0) {
		activeObserver = make_unique<OthersObserver>();
	}
	else if (csn.compare("erase") == 0) {
		activeObserver = make_unique<EraseObserver>();
	}
	else if (csn.compare("sync") == 0) {
		activeObserver = make_unique<SyncObserver>();
	}
	else if (csn.compare("assign") == 0) {
		activeObserver = make_unique<AssignObserver>();
	}
	else if (csn.compare("assign16levels") == 0) {
		activeObserver = make_unique<Assign16LevelsObserver>();
	}
	else if (csn.compare("metronomesound") == 0) {
		activeObserver = make_unique<MetronomeSoundObserver>();
	}
	else if (csn.compare("saveallfile") == 0) {
		activeObserver = make_unique<SaveAllFileObserver>();
	}
	else if (csn.compare("loadasequencefromall") == 0) {
		activeObserver = make_unique<LoadASequenceFromAllObserver>();
	}
	else if (csn.compare("nextseqpad") == 0) {
		activeObserver = make_unique<NextSeqPadObserver>();
	}
	else if (csn.compare("nextseq") == 0) {
	activeObserver = make_unique<NextSeqObserver>();
	}
	else if (csn.compare("song") == 0) {
	activeObserver = make_unique<SongObserver>();
	}
	else if (csn.compare("trackmute") == 0) {
	activeObserver = make_unique<TrMuteObserver>();
	}
	else if (checkActiveScreen(&diskNames, currentScreenName)) {
	activeObserver = make_unique<DiskObserver>();
	}
	else if (checkActiveScreen(&seqWindowNames, currentScreenName)) {
	activeObserver = make_unique<SequencerWindowObserver>();
	}
	else if (checkActiveScreen(&soundNames, currentScreenName)) {
	activeObserver = make_unique<SoundObserver>();
	}
	else if (csn.compare("sample") == 0) {
	activeObserver = make_unique<SampleObserver>();
	}
	else if (csn.compare("directory") == 0) {
		activeObserver = make_unique<DirectoryObserver>(Mpc::instance().getDisk());
	}
	else if (csn.compare("programparams") == 0) {
		activeObserver = make_unique<PgmParamsObserver>();
	}
	else if (csn.compare("programassign") == 0) {
		activeObserver = make_unique<PgmAssignObserver>();
	}
	else if (csn.compare("sequencer_step") == 0) {
		activeObserver = make_unique<StepEditorObserver>();
	}
	else if (csn.compare("step_tc") == 0 || csn.compare("editmultiple") == 0 || csn.compare("insertevent") == 0) {
		activeObserver = make_unique<StepWindowObserver>();
	}
	else if (csn.compare("mixerv2") == 0 || csn.compare("mixersetup") == 0) {
		activeObserver = make_unique<MixerObserver>();
	}
	else if (csn.compare("fxedit") == 0) {
		activeObserver = make_unique<FxEditObserver>();
	}
	else if (csn.compare("channelsettings") == 0) {
		activeObserver = make_unique<ChannelSettingsObserver>();
	}
	else if (csn.compare("edit") == 0) {
		activeObserver = make_unique<EditSequenceObserver>();
	}
	else if (csn.compare("name") == 0) {
		activeObserver = make_unique<ui::NameObserver>();
	}
	else if (csn.compare("midiinputmonitor") == 0 || csn.compare("midioutputmonitor") == 0) {
		activeObserver = make_unique<MidiMonitorObserver>();
	}
	else if (csn.compare("barcopy") == 0) {
		activeObserver = make_unique<BarCopyObserver>();
	}
	else if (csn.compare("trmove") == 0) {
		activeObserver = make_unique<TrMoveObserver>();
	}
	else if (csn.compare("user") == 0) {
		activeObserver = make_unique<UserObserver>();
	}
	else if (csn.compare("trim") == 0) {
		activeObserver = make_unique<TrimObserver>();
	}
	else if (csn.compare("loop") == 0) {
		activeObserver = make_unique<LoopObserver>();
	}
	else if (csn.compare("editsound") == 0) {
		activeObserver = make_unique<EditSoundObserver>();
	}
	else if (csn.find("startfine") != string::npos || csn.find("endfine") != string::npos || csn.compare("looptofine") == 0) {
		activeObserver = make_unique<ZoomObserver>();
	}
	else if (csn.compare("zone") == 0 || csn.compare("numberofzones") == 0) {
		activeObserver = make_unique<ZoneObserver>();
	}
	else if (csn.compare("params") == 0) {
		activeObserver = make_unique<SndParamsObserver>();
	}
	else if (csn.compare("deleteallfiles") == 0) {
		activeObserver = make_unique<DeleteAllFilesObserver>();
	}
	else if (checkActiveScreen(&samplerWindowNames, csn)) {
		activeObserver = make_unique<SamplerWindowObserver>();
	}
	else if (csn.compare("purge") == 0) {
		activeObserver = make_unique<PurgeObserver>();
	}
	else if (csn.compare("drum") == 0) {
		activeObserver = make_unique<DrumObserver>();
	}
	else if (csn.compare("muteassign") == 0) {
		activeObserver = make_unique<MuteAssignObserver>();
	}
	else if (csn.compare("settings") == 0) {
		activeObserver = make_unique<VmpcSettingsObserver>();
	}
}

string LayeredScreen::getFocus() {
	return getFocusedLayer().lock()->getFocus();
}

void LayeredScreen::setFocus(const string& focus) {
	getFocusedLayer().lock()->setFocus(focus);
}
