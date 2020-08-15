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
#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/SampleScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>

#include <file/FileUtil.hpp>
#include <lang/StrUtil.hpp>

#include <cmath>
#include <set>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::dialog2;
using namespace moduru::file;
using namespace moduru::lang;
using namespace rapidjson;
using namespace std;

//std::vector<std::vector<bool>> LayeredScreen::atlas = _bmfParser.getAtlas();
//moduru::gui::bmfont LayeredScreen::font = _bmfParser.getLoadedFont();

LayeredScreen::LayeredScreen(mpc::Mpc& mpc)
	: mpc(mpc)
{
	moduru::gui::BMFParser bmfParser(string(mpc::Paths::resPath() + moduru::file::FileUtil::getSeparator() + "mpc2000xl-font.fnt"));

	font = bmfParser.getLoadedFont();
	atlas = bmfParser.getAtlas();

	root = make_unique<Component>("root");
	
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

int LayeredScreen::openScreen(string screenName)
{
	if (currentScreenName.compare(screenName) == 0)
		return -1;

	auto ams = mpc.getAudioMidiServices().lock();

	if (currentScreenName.compare("song") == 0 && mpc.getSequencer().lock()->isPlaying())
		return -1;

	if (currentScreenName.compare("sample") == 0)
	{
		ams->muteMonitor(true);
		ams->getSoundRecorder().lock()->setSampleScreenActive(false);
	}
	else if (screenName.compare("sample") == 0)
	{
		auto sampleScreen = dynamic_pointer_cast<SampleScreen>(mpc.screens->getScreenComponent("sample"));
		bool muteMonitor = sampleScreen->getMonitor() == 0;
		ams->muteMonitor(muteMonitor);
		ams->getSoundRecorder().lock()->setSampleScreenActive(true);
	}

	if (currentScreenName.compare("erase") == 0 || currentScreenName.compare("timing-correct") == 0)
	{
		// This field may not be visible the next time we visit this screen.
		// Like the real 2KXL we always set focus to the first Notes: field
		// if the current focus is hte second Notes: field.
		if (getFocus().compare("note1") == 0)
			setFocus("note0");
	}

	auto focus = getFocusedLayer().lock()->findField(getFocus()).lock();

	if (focus)
	{
		if (focus->isSplit())
			focus->setSplit(false);

		if (focus->isTypeModeEnabled())
			focus->disableTypeMode();
	}

	setLastFocus(currentScreenName, getFocus());
		
	auto screenComponent = mpc.screens->getScreenComponent(screenName);

	if (!screenComponent)
		return -1;

	previousScreenName = currentScreenName;
	currentScreenName = screenName;

	auto oldScreenComponent = getFocusedLayer().lock()->findScreenComponent().lock();

	if (oldScreenComponent)
	{
		oldScreenComponent->close();
		getFocusedLayer().lock()->removeChild(oldScreenComponent);
	}

	focusedLayerIndex = screenComponent->getLayerIndex();

	getFocusedLayer().lock()->addChild(screenComponent);

	if (screenComponent->findFields().size() > 0)
		returnToLastFocus(screenComponent->getFirstField());

	screenComponent->open();

	return focusedLayerIndex;
}

vector<vector<bool>>* LayeredScreen::getPixels()
{
	return &pixels;
}

void LayeredScreen::Draw()
{
	MLOG("LayeredScreen::Draw()");
	for (auto& c : root->findHiddenChildren())
	{
		c.lock()->Draw(&pixels);
	}

	root->preDrawClear(&pixels);
	root->Draw(&pixels);
	return;
}

MRECT LayeredScreen::getDirtyArea()
{
	MLOG("LayeredScreen::getDirtyArea()");
	auto dirtyArea = root->getDirtyArea();
	//MLOG("dirtyArea: " + dirtyArea.getInfo());
	return dirtyArea;
}

bool LayeredScreen::IsDirty()
{
	return root->IsDirty();
}

Background* LayeredScreen::getCurrentBackground()
{
	return getFocusedLayer().lock()->getBackground();
}

void LayeredScreen::setCurrentBackground(string s)
{
	getCurrentBackground()->setName(s);
}

void LayeredScreen::returnToLastFocus(string firstFieldOfCurrentScreen)
{
	auto focusCounter = 0;
	
	for (auto& lf : lastFocuses)
	{
		if (lf[0].compare(currentScreenName) == 0)
		{
			focusCounter++;
			setFocus(lf[1]);
		}
	}

	if (focusCounter == 0)
	{
		vector<string> sa(2);
		sa[0] = currentScreenName;
		sa[1] = firstFieldOfCurrentScreen;
		lastFocuses.push_back(sa);
		setFocus(firstFieldOfCurrentScreen);
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
	for (auto& lf : lastFocuses)
	{
		if (lf[0].compare(screenName) == 0)
		{
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

int LayeredScreen::getFocusedLayerIndex()
{
	return focusedLayerIndex;
}

std::weak_ptr<Layer> LayeredScreen::getFocusedLayer()
{
	return layers[focusedLayerIndex];
}

bool LayeredScreen::transfer(int direction)
{
	auto screen = findScreenComponent().lock();
	auto currentFocus = getFocusedLayer().lock()->findField(getFocus()).lock();
	auto transferMap = screen->getTransferMap();
	auto mapCandidate = transferMap.find(currentFocus->getName());

	if (mapCandidate == end(transferMap))
	{
		return false;
	}

	auto mapping = (*mapCandidate).second;
	auto nextFocusNames = StrUtil::split(mapping[direction], ',');

	for (auto& nextFocusName : nextFocusNames)
	{
		if (nextFocusName.compare("_") == 0)
		{
			return true;
		}

		if (setFocus(nextFocusName))
		{
			return true;
		}
	}

	return false;
}

void LayeredScreen::transferLeft()
{
	if (transfer(0))
	{
		return;
	}
	
	auto currentFocus = getFocusedLayer().lock()->findField(getFocus()).lock();

	shared_ptr<Field> candidate;

	for (auto& f : getFocusedLayer().lock()->findFields())
	{
		if (f.lock() == currentFocus || !f.lock()->isFocusable() || f.lock()->IsHidden())
		{
			continue;
		}

		int verticalOffset = abs(currentFocus->getY() - f.lock()->getY());

		if (verticalOffset > 2)
		{
			continue;
		}

		int candidateVerticalOffset = candidate ? abs(currentFocus->getY() - candidate->getY()) : INT_MAX;

		if (verticalOffset <= candidateVerticalOffset)
		{

			if (f.lock()->getX() > currentFocus->getX())
			{
				continue;
			}

			int horizontalOffset = currentFocus->getX() - f.lock()->getX();
			int candidateHorizontalOffset = candidate ? currentFocus->getX() - candidate->getX() : INT_MAX;

			if (horizontalOffset <= candidateHorizontalOffset)
			{
				candidate = f.lock();
			}
		}
	}

	if (candidate)
	{
		setFocus(candidate->getName());
	}
}

void LayeredScreen::transferRight()
{
	if (transfer(1))
	{
		return;
	}

	shared_ptr<Field> candidate;

	auto source = getFocusedLayer().lock()->findField(getFocus()).lock();

	for (auto& f : getFocusedLayer().lock()->findFields())
	{
		if (f.lock() == source || !f.lock()->isFocusable() || f.lock()->IsHidden())
		{
			continue;
		}
		
		int verticalOffset = abs(source->getY() - f.lock()->getY());

		if (verticalOffset > 2)
		{
			continue;
		}

		int candidateVerticalOffset = candidate ? abs(source->getY() - candidate->getY()) : INT_MAX;

		if (verticalOffset <= candidateVerticalOffset)
		{

			if (f.lock()->getX() < source->getX())
			{
				continue;
			}

			int horizontalOffset = f.lock()->getX() - source->getX();
			int candidateHorizontalOffset = candidate ? candidate->getX() - source->getX() : INT_MAX;

			if (horizontalOffset <= candidateHorizontalOffset)
			{
				candidate = f.lock();
			}
		}
	}

	if (candidate)
	{
		setFocus(candidate->getName());
	}
}

void LayeredScreen::transferDown()
{
	if (transfer(3))
	{
		return;
	}

	int marginChars = 8;
	int minDistV = 7;
	int maxDistH = 6 * marginChars;
	auto current = getFocusedLayer().lock()->findField(getFocus()).lock();
	shared_ptr<Field> next;

	for (auto& field : getFocusedLayer().lock()->findFields())
	{
		auto B1 = field.lock()->getRect().B;
		auto B0 = current->getRect().B;
		auto MW1 = 0.5f * (float)(field.lock()->getX() * 2 + field.lock()->getW());
		auto MW0 = 0.5f * (float)(current->getX() * 2 + current->getW());

		if (B1 - B0 >= minDistV)
		{
			if (abs((int)(MW1 - MW0)) <= maxDistH)
			{
				if (!field.lock()->IsHidden() && field.lock()->isFocusable())
				{
					next = field.lock();
					break;
				}
			}
		}
	}

	if (next == current)
	{
		marginChars = 16;
		maxDistH = 6 * marginChars;

		for (auto& field : getFocusedLayer().lock()->findFields())
		{
			auto B0 = current->getY() + current->getH();
			auto B1 = field.lock()->getY() + field.lock()->getH();
			auto MW0 = 0.5f * (float)(current->getX() * 2 + current->getW());
			auto MW1 = 0.5f * (float)(field.lock()->getX() * 2 + field.lock()->getW());

			if (B1 - B0 >= minDistV)
			{
				if (abs((int)(MW1 - MW0)) <= maxDistH)
				{
					if (!field.lock()->IsHidden() && field.lock()->isFocusable())
					{
						next = field.lock();
						break;
					}
				}
			}
		}
	}

	if (next)
	{
		setFocus(next->getName());
	}
}

void LayeredScreen::transferUp()
{
	shared_ptr<Field> newCandidate;

	if (transfer(2))
	{
		return;
	}

	int marginChars = 8;
	int minDistV = -7;
	int maxDistH = 6 * marginChars;
	auto result = getFocusedLayer().lock()->findField(getFocus()).lock();
	shared_ptr<Field> next;
	
	auto revComponents = getFocusedLayer().lock()->findFields();

	reverse(revComponents.begin(), revComponents.end());

	for (auto& field : revComponents)
	{
		auto B1 = field.lock()->getY() + field.lock()->getH();
		auto B0 = result->getY() + result->getH();
		auto MW1 = 0.5f * (float)(field.lock()->getX() * 2 + field.lock()->getW());
		auto MW0 = 0.5f * (float)(result->getX() * 2 + result->getW());

		if (B1 - B0 <= minDistV)
		{
			if (abs((int)(MW1 - MW0)) <= maxDistH)
			{
				if (!field.lock()->IsHidden() && field.lock()->isFocusable())
				{
					next = field.lock();
					break;
				}
			}
		}
	}

	if (next == result)
	{
		marginChars = 16;
		maxDistH = 6 * marginChars;
	
		for (auto& field : revComponents)
		{
			auto B1 = field.lock()->getY() + field.lock()->getH();
			auto B0 = result->getY() + result->getH();
			auto MW1 = 0.5f * (float)(field.lock()->getX() * 2 + field.lock()->getW());
			auto MW0 = 0.5f * (float)(field.lock()->getX() * 2 + field.lock()->getW());

			if (B1 - B0 <= minDistV)
			{
				if (abs((int)(MW1 - MW0)) <= maxDistH)
				{
					if (!field.lock()->IsHidden() && field.lock()->isFocusable())
					{
						next = field.lock();
						break;
					}
				}
			}
		}
	}

	if (next)
	{
		setFocus(next->getName());
	}
}

string LayeredScreen::getFocus()
{
	return getFocusedLayer().lock()->getFocus();
}

bool LayeredScreen::setFocus(const string& focus)
{
	return getFocusedLayer().lock()->setFocus(focus);
}

void LayeredScreen::setFunctionKeysArrangement(int arrangementIndex)
{
	getFunctionKeys()->setActiveArrangement(arrangementIndex);
}

FunctionKeys* LayeredScreen::getFunctionKeys()
{
	return getFocusedLayer().lock()->getFunctionKeys();
}
