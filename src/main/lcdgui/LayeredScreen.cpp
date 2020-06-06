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
#include <lcdgui/TwoDots.hpp>
#include <lcdgui/Wave.hpp>
#include <lcdgui/Popup.hpp>
#include <lcdgui/SelectedEventBar.hpp>
#include <lcdgui/Effect.hpp>
#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/SampleScreen.hpp>

#include <ui/Uis.hpp>

#include <file/FileUtil.hpp>

#include <cmath>
#include <set>

using namespace mpc::ui;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

using namespace moduru::file;

using namespace rapidjson;

using namespace std;

static moduru::gui::BMFParser _bmfParser = moduru::gui::BMFParser(string(mpc::Paths::resPath() + moduru::file::FileUtil::getSeparator() + "font.fnt"));

std::vector<std::vector<bool>> LayeredScreen::atlas = _bmfParser.getAtlas();
moduru::gui::bmfont LayeredScreen::font = _bmfParser.getLoadedFont();

LayeredScreen::LayeredScreen() 
{	
	root = make_unique<Component>("root");
	popup = make_unique<Popup>();
	popup->Hide(true);

	horizontalBarsTempoChangeEditor = vector<shared_ptr<HorizontalBar>>(4);
	horizontalBarsStepEditor = vector<shared_ptr<HorizontalBar>>(4);
	selectedEventBarsStepEditor = vector<shared_ptr<SelectedEventBar>>(4);

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
		auto sampleScreen = dynamic_pointer_cast<SampleScreen>(Screens::getScreenComponent("sample"));
		bool muteMonitor = sampleScreen->getMonitor() == 0;
		ams->muteMonitor(muteMonitor);
		ams->getSoundRecorder().lock()->setVuMeterActive(true);
	}

	if (currentScreenName.compare("erase") == 0 || currentScreenName.compare("timing-correct") == 0)
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
	
	screenComponent->open();

	if (screenComponent->findFields().size() > 0)
	{
		returnToLastFocus(screenComponent->findFields().front().lock()->getName());
	}
	return focusedLayerIndex;
}

vector<vector<bool>>* LayeredScreen::getPixels() {
	return &pixels;
}

void LayeredScreen::Draw() {
	MLOG("LayeredScreen::Draw()");
	for (auto& c : root->findHiddenChildren())
	{
		c.lock()->Draw(&pixels);
	}
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

void LayeredScreen::openFileNamePopup(const string& name, const string& extension)
{
	string extUpperCase = "";

	for (auto& c : extension)
	{
		extUpperCase.push_back(toupper(c));
	}

	createPopup("LOADING " + name + "." + extUpperCase, 85);
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

void LayeredScreen::setFunctionKeysArrangement(int arrangementIndex)
{
	getFunctionKeys()->setActiveArrangement(arrangementIndex);
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
	
	for (auto& a : getFocusedLayer().lock()->findFields())
	{
		auto tf1 = a.lock();
		auto B1 = tf1->getY() + tf1->getH();
		auto B0 = lTf0->getY() + lTf0->getH();
		auto MW1 = 0.5f * (float)(tf1->getX()*2 + tf1->getW());
		auto MW0 = 0.5f * (float)(lTf0->getX() * 2 + lTf0->getW());

		if (B1 - B0 >= minDistV)
		{
			if (abs((int)(MW1 - MW0)) <= maxDistH)
			{
				if (!tf1->IsHidden() && tf1->isFocusable())
				{
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

string LayeredScreen::getFocus()
{
	return getFocusedLayer().lock()->getFocus();
}

void LayeredScreen::setFocus(const string& focus)
{
	getFocusedLayer().lock()->setFocus(focus);
}
