#include "lcdgui/LayeredScreen.hpp"

#include "Screens.hpp"

#include "BasicStructs.hpp"

#include <Mpc.hpp>

#include "Field.hpp"
#include "Component.hpp"

#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/SoundRecorder.hpp>

#include <lcdgui/Layer.hpp>
#include <lcdgui/ScreenComponent.hpp>
#include <lcdgui/screens/SampleScreen.hpp>

#include <StrUtil.hpp>

#include <cmath>
#include <set>

#include "MpcResourceUtil.hpp"
#include "inputlogic/PadAndButtonKeyboard.hpp"
#include "hardware/Hardware.h"

#if __linux__
#include <climits>
#endif

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

LayeredScreen::LayeredScreen(mpc::Mpc& mpc)
	: mpc(mpc)
{
    const auto fntPath = "fonts/mpc2000xl-font.fnt";
    auto fntData = MpcResourceUtil::get_resource_data(fntPath);

    const auto bmpPath = "fonts/mpc2000xl-font_0.bmp";
    auto bmpData = MpcResourceUtil::get_resource_data(bmpPath);

	BMFParser bmfParser(&fntData[0], fntData.size(), &bmpData[0], bmpData.size());

	font = bmfParser.getLoadedFont();
	atlas = bmfParser.getAtlas();

	root = std::make_unique<Component>("root");
	
	std::shared_ptr<Layer> previousLayer;

	for (int i = 0; i < LAYER_COUNT; i++)
	{
		auto layer = std::make_shared<Layer>();
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

std::shared_ptr<ScreenComponent> LayeredScreen::findScreenComponent()
{
	return getFocusedLayer()->findScreenComponent();
}

int LayeredScreen::openScreen(std::string newScreenName)
{
	if (currentScreenName == newScreenName)
    {
        return -1;
    }

    const auto oldFocusedLayerIndex = focusedLayerIndex;

    auto screenComponent = mpc.screens->getScreenComponent(newScreenName);

    if (!screenComponent)
    {
        return -1;
    }
    
	auto ams = mpc.getAudioMidiServices();

    if (currentScreenName == "song" && mpc.getSequencer()->isPlaying())
    {
        return -1;
    }
    else if (currentScreenName == "sample")
	{
		ams->muteMonitor(true);
		ams->getSoundRecorder()->setSampleScreenActive(false);
	}
    else if (currentScreenName == "erase" || currentScreenName == "timing-correct")
    {
        // This field may not be visible the next time we visit this screen.
        // Like the real 2KXL we always set focus to the first Notes: field
        // if the current focus is hte second Notes: field.
        if (getFocus() == "note1")
            setFocus("note0");
    }

    if (newScreenName == "sample")
	{
		auto sampleScreen = mpc.screens->get<SampleScreen>("sample");
		bool muteMonitor = sampleScreen->getMonitor() == 0;
		ams->muteMonitor(muteMonitor);
		ams->getSoundRecorder()->setSampleScreenActive(true);
	}
    else if (newScreenName == "name")
    {
        mpc.getPadAndButtonKeyboard()->resetPreviousPad();
        mpc.getPadAndButtonKeyboard()->resetPressedZeroTimes();
        mpc.getPadAndButtonKeyboard()->resetUpperCase();
    }

    auto focus = getFocusedLayer()->findField(getFocus());

    setLastFocus(currentScreenName, getFocus());

    if (focus)
        focus->loseFocus("");

    if (currentScreenName != "popup") previousScreenName = currentScreenName;
	currentScreenName = newScreenName;

	auto oldScreenComponent = getFocusedLayer()->findScreenComponent();

	if (oldScreenComponent)
	{
		oldScreenComponent->close();
		getFocusedLayer()->removeChild(oldScreenComponent);
	}

	focusedLayerIndex = screenComponent->getLayerIndex();

	getFocusedLayer()->addChild(screenComponent);

	if (screenComponent->findFields().size() > 0)
		returnToLastFocus(screenComponent->getFirstField());

	screenComponent->open();

	std::vector<std::string> overdubScreens{ "step-editor", "paste-event", "insert-event", "edit-multiple", "step-timing-correct" };

	auto isOverdubScreen = find(begin(overdubScreens), end(overdubScreens), currentScreenName) != end(overdubScreens);
	mpc.getHardware()->getLed(hardware::ComponentId::OVERDUB_LED)->setEnabled(isOverdubScreen);

	std::vector<std::string> nextSeqScreens{ "sequencer", "next-seq", "next-seq-pad", "track-mute", "time-display", "assign" };

	auto isNextSeqScreen = find(begin(nextSeqScreens), end(nextSeqScreens), currentScreenName) != end(nextSeqScreens);
	
	if (!isNextSeqScreen || (currentScreenName == "sequencer" && !mpc.getSequencer()->isPlaying()))
    {
        if (mpc.getSequencer()->getNextSq() != -1)
        {
            mpc.getSequencer()->setNextSq(-1);
        }
    }

    if (focusedLayerIndex < oldFocusedLayerIndex)
    {
        clearScreenToReturnToWhenPressingOpenWindow();
    }

	return focusedLayerIndex;
}

std::vector<std::vector<bool>>* LayeredScreen::getPixels()
{
	return &pixels;
}

void LayeredScreen::Draw()
{
//	MLOG("LayeredScreen::Draw()");
	for (auto& c : root->findHiddenChildren())
		c->Draw(&pixels);

	root->preDrawClear(&pixels);
	root->Draw(&pixels);
	return;
}

MRECT LayeredScreen::getDirtyArea()
{
//	MLOG("LayeredScreen::getDirtyArea()");
	auto dirtyArea = root->getDirtyArea();
	//MLOG("dirtyArea: " + dirtyArea.getInfo());
	return dirtyArea;
}

bool LayeredScreen::IsDirty()
{
	return root->IsDirty();
}

void LayeredScreen::setDirty()
{
	root->SetDirty();
}

Background* LayeredScreen::getCurrentBackground()
{
	return getFocusedLayer()->getBackground();
}

void LayeredScreen::setCurrentBackground(std::string s)
{
	getCurrentBackground()->setName(s);
}

void LayeredScreen::returnToLastFocus(std::string firstFieldOfCurrentScreen)
{
    auto lastFocus = lastFocuses.find(currentScreenName);
    
    if (lastFocus == end(lastFocuses))
    {
        lastFocuses[currentScreenName] = firstFieldOfCurrentScreen;
        setFocus(firstFieldOfCurrentScreen);
        return;
    }
    
    setFocus(lastFocus->second);
}

void LayeredScreen::setLastFocus(std::string screenName, std::string newLastFocus)
{
    lastFocuses[screenName] = newLastFocus;
}

std::string LayeredScreen::getLastFocus(std::string screenName)
{
    auto lastFocus = lastFocuses.find(screenName);
    
    if (lastFocus == end(lastFocuses))
        return "";
    
    return lastFocus->second;
}

std::string LayeredScreen::getCurrentScreenName()
{
	return currentScreenName;
}

void LayeredScreen::setPreviousScreenName(std::string screenName)
{
	previousScreenName = screenName;
}

std::string LayeredScreen::getPreviousScreenName()
{
	return previousScreenName;
}

int LayeredScreen::getFocusedLayerIndex()
{
	return focusedLayerIndex;
}

std::shared_ptr<Layer> LayeredScreen::getFocusedLayer()
{
	return layers[focusedLayerIndex];
}

bool LayeredScreen::transfer(int direction)
{
	auto screen = findScreenComponent();
	auto currentFocus = getFocusedLayer()->findField(getFocus());
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
		if (nextFocusName == "_")
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
	
	auto currentFocus = getFocusedLayer()->findField(getFocus());

	std::shared_ptr<Field> candidate;

	for (auto& f : getFocusedLayer()->findFields())
	{
		if (f == currentFocus || !f->isFocusable() || f->IsHidden())
		{
			continue;
		}

		int verticalOffset = abs(currentFocus->getY() - f->getY());

		if (verticalOffset > 2)
		{
			continue;
		}

		int candidateVerticalOffset = candidate ? abs(currentFocus->getY() - candidate->getY()) : INT_MAX;

		if (verticalOffset <= candidateVerticalOffset)
		{

			if (f->getX() > currentFocus->getX())
			{
				continue;
			}

			int horizontalOffset = currentFocus->getX() - f->getX();
			int candidateHorizontalOffset = candidate ? currentFocus->getX() - candidate->getX() : INT_MAX;

			if (horizontalOffset <= candidateHorizontalOffset)
			{
				candidate = f;
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

	std::shared_ptr<Field> candidate;

	auto source = getFocusedLayer()->findField(getFocus());

	for (auto& f : getFocusedLayer()->findFields())
	{
		if (f == source || !f->isFocusable() || f->IsHidden())
		{
			continue;
		}
		
		int verticalOffset = abs(source->getY() - f->getY());

		if (verticalOffset > 2)
		{
			continue;
		}

		int candidateVerticalOffset = candidate ? abs(source->getY() - candidate->getY()) : INT_MAX;

		if (verticalOffset <= candidateVerticalOffset)
		{

			if (f->getX() < source->getX())
			{
				continue;
			}

			int horizontalOffset = f->getX() - source->getX();
			int candidateHorizontalOffset = candidate ? candidate->getX() - source->getX() : INT_MAX;

			if (horizontalOffset <= candidateHorizontalOffset)
			{
				candidate = f;
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
	auto current = getFocusedLayer()->findField(getFocus());
	std::shared_ptr<Field> next;

	for (auto& field : getFocusedLayer()->findFields())
	{
		auto B1 = field->getRect().B;
		auto B0 = current->getRect().B;
		auto MW1 = 0.5f * (float)(field->getX() * 2 + field->getW());
		auto MW0 = 0.5f * (float)(current->getX() * 2 + current->getW());

		if (B1 - B0 >= minDistV)
		{
			if (abs((int)(MW1 - MW0)) <= maxDistH)
			{
				if (!field->IsHidden() && field->isFocusable())
				{
					next = field;
					break;
				}
			}
		}
	}

	if (next == current)
	{
		marginChars = 16;
		maxDistH = 6 * marginChars;

		for (auto& field : getFocusedLayer()->findFields())
		{
			auto B0 = current->getY() + current->getH();
			auto B1 = field->getY() + field->getH();
			auto MW0 = 0.5f * (float)(current->getX() * 2 + current->getW());
			auto MW1 = 0.5f * (float)(field->getX() * 2 + field->getW());

			if (B1 - B0 >= minDistV)
			{
				if (abs((int)(MW1 - MW0)) <= maxDistH)
				{
					if (!field->IsHidden() && field->isFocusable())
					{
						next = field;
						break;
					}
				}
			}
		}
	}

	if (next)
		setFocus(next->getName());
}

void LayeredScreen::transferUp()
{
	std::shared_ptr<Field> newCandidate;

	if (transfer(2))
		return;

	int marginChars = 8;
	int minDistV = -7;
	int maxDistH = 6 * marginChars;
	auto result = getFocusedLayer()->findField(getFocus());
	std::shared_ptr<Field> next;
	
	auto revComponents = getFocusedLayer()->findFields();

	reverse(revComponents.begin(), revComponents.end());

	for (auto& field : revComponents)
	{
		auto B1 = field->getY() + field->getH();
		auto B0 = result->getY() + result->getH();
		auto MW1 = 0.5f * (float)(field->getX() * 2 + field->getW());
		auto MW0 = 0.5f * (float)(result->getX() * 2 + result->getW());

		if (B1 - B0 <= minDistV)
		{
			if (abs((int)(MW1 - MW0)) <= maxDistH)
			{
				if (!field->IsHidden() && field->isFocusable())
				{
					next = field;
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
			auto B1 = field->getY() + field->getH();
			auto B0 = result->getY() + result->getH();
			auto MW1 = 0.5f * (float)(field->getX() * 2 + field->getW());
			auto MW0 = 0.5f * (float)(field->getX() * 2 + field->getW());

			if (B1 - B0 <= minDistV)
			{
				if (abs((int)(MW1 - MW0)) <= maxDistH)
				{
					if (!field->IsHidden() && field->isFocusable())
					{
						next = field;
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

std::string LayeredScreen::getFocus()
{
	return getFocusedLayer()->getFocus();
}

bool LayeredScreen::setFocus(const std::string& focus)
{
	return getFocusedLayer()->setFocus(focus);
}

void LayeredScreen::setFunctionKeysArrangement(int arrangementIndex)
{
	getFunctionKeys()->setActiveArrangement(arrangementIndex);
}

FunctionKeys* LayeredScreen::getFunctionKeys()
{
	return getFocusedLayer()->getFunctionKeys();
}

std::string LayeredScreen::getScreenToReturnToWhenPressingOpenWindow()
{
    return screenToReturnToWhenPressingOpenWindow;
}

void LayeredScreen::setScreenToReturnToWhenPressingOpenWindow(const std::string screenToUse)
{
    screenToReturnToWhenPressingOpenWindow = screenToUse;
}

void LayeredScreen::clearScreenToReturnToWhenPressingOpenWindow()
{
    screenToReturnToWhenPressingOpenWindow.clear();
}

std::shared_ptr<Field> LayeredScreen::getFocusedField()
{
    const auto focusedLayer = getFocusedLayer();
    const auto focusedFieldName = focusedLayer->getFocus();
    return focusedLayer->findField(focusedFieldName);
}

