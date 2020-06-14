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
#include <lcdgui/Popup.hpp>
#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/SampleScreen.hpp>

#include <file/FileUtil.hpp>

#include <cmath>
#include <set>

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

int LayeredScreen::getCurrentFieldIndex()
{
	int currentIndex;
	auto layer = layers[focusedLayerIndex].lock();
	auto fields = layer->findFields();
	int size = fields.size();
	
	for (currentIndex = 0; currentIndex <= size; currentIndex++)
	{
		if (currentIndex == size)
		{
			break;
		}

		if (fields[currentIndex].lock()->getName().compare(layer->getFocus()) == 0)
		{
			break;
		}
	}
	
	if (currentIndex == size)
	{
		return -1;
	}
	return currentIndex;
}

void LayeredScreen::transferFocus(bool backwards)
{
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
		
		if (candidateIndex >= 0 && candidateIndex < size)
		{
			if (!fields[candidateIndex].lock()->IsHidden())
			{
				success = true;
			}
		}
		else
		{
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
	MLOG("dirtyArea: " + dirtyArea.getInfo());
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

string LayeredScreen::getFocus()
{
	return getFocusedLayer().lock()->getFocus();
}

void LayeredScreen::setFocus(const string& focus)
{
	getFocusedLayer().lock()->setFocus(focus);
}

void LayeredScreen::setFunctionKeysArrangement(int arrangementIndex)
{
	getFunctionKeys()->setActiveArrangement(arrangementIndex);
}

FunctionKeys* LayeredScreen::getFunctionKeys()
{
	return getFocusedLayer().lock()->getFunctionKeys();
}
