#include "lcdgui/LayeredScreen.hpp"

#include "lcdgui/ScreenGroups.h"
#include "lcdgui/Screens.hpp"
#include "lcdgui/AllScreens.h"
#include "lcdgui/ScreenRegistry.h"

#include "BasicStructs.hpp"

#include <Mpc.hpp>

#include "Field.hpp"
#include "Component.hpp"

#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/SoundRecorder.hpp>

#include <lcdgui/Layer.hpp>
#include <lcdgui/ScreenComponent.hpp>

#include <StrUtil.hpp>

#include <cmath>
#include <memory>
#include <set>
#include <stdexcept>

#include "MpcResourceUtil.hpp"
#include "input/PadAndButtonKeyboard.hpp"
#include "hardware/Hardware.h"

#if __linux__
#include <climits>
#endif

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::dialog2;

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
	
	for (int i = 0; i < LAYER_COUNT; i++)
	{
		auto layer = std::make_shared<Layer>(i);
		layers.push_back(layer);
        root->addChild(layer);
	}
}

template <typename... Ts>
bool LayeredScreen::isPreviousScreen() const
{
    if (history.size() < 2)
    {
        return false;
    }

    const auto& prev = history[history.size() - 2];
    return ((static_cast<bool>(std::dynamic_pointer_cast<Ts>(prev))) || ...);
}

template <typename... Ts>
bool LayeredScreen::isPreviousScreenNot() const
{
    if (history.size() < 2)
    {
        return true;
    }

    const auto& prev = history[history.size() - 2];
    return (!(std::dynamic_pointer_cast<Ts>(prev)) && ...);
}

template <typename... Ts>
bool LayeredScreen::isCurrentScreen() const
{
    if (history.size() < 1)
    {
        return false;
    }

    const auto& curr = history.back();
    return ((static_cast<bool>(std::dynamic_pointer_cast<Ts>(curr))) || ...);
}

void LayeredScreen::showPopupForMs(const std::string msg, const int delayMs)
{
    auto popupScreen = mpc.screens->get<PopupScreen>();
    popupScreen->setText(msg);
    openScreen<PopupScreen>();

    std::thread([this, delayMs]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        closeCurrentScreen();
    }).detach();
}

template <typename T>
void LayeredScreen::showPopupAndThenOpen(const std::string msg, const int delayMs)
{
    auto popupScreen = mpc.screens->get<PopupScreen>();
    popupScreen->setText(msg);
    openScreen<PopupScreen>();

    std::thread([this, delayMs]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        openScreen<T>();
    }).detach();
}

void LayeredScreen::showPopupAndThenReturnToLayer(const std::string msg, const int delayMs, const int layerIndex)
{
    auto popupScreen = mpc.screens->get<PopupScreen>();
    popupScreen->setText(msg);
    openScreen<PopupScreen>();
    std::thread([this, delayMs, layerIndex]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        while (!history.empty() && history.back()->getLayerIndex() != layerIndex)
        {
            closeCurrentScreen();
        }
    }).detach();
}

void LayeredScreen::showPopupAndAwaitInteraction(const std::string msg)
{
    auto popupScreen = mpc.screens->get<PopupScreen>();
    popupScreen->setText(msg);
    openScreen<PopupScreen>();
}

std::shared_ptr<ScreenComponent> LayeredScreen::getCurrentScreen()
{
    assert(!history.empty());
	return history.back();
}

using OpenScreenFunc = std::function<void(LayeredScreen&)>;

#define X(ns, Class, name) { name, [](LayeredScreen& ls){ ls.openScreen<mpc::lcdgui::ns::Class>(); } },
static const std::map<std::string, OpenScreenFunc> openScreenRegistry = {
    SCREEN_LIST
};
#undef X

template <typename T>
bool LayeredScreen::isCurrentScreenPopupFor() const
{
    return history.size() >= 2 &&
        std::dynamic_pointer_cast<PopupScreen>(history.back()) &&
        std::dynamic_pointer_cast<T>(history[history.size() - 2]);
}

void LayeredScreen::openScreen(const std::string name)
{
    if (auto it = openScreenRegistry.find(name); it != openScreenRegistry.end())
        it->second(*this);
    else
        MLOG("Unknown screen name: " + name);
}

template <typename T>
void LayeredScreen::openScreen()
{
    printf("openScreen\n");
	if (!history.empty() && std::dynamic_pointer_cast<T>(history.back()))
    {
        return;
    }

    auto newScreen = mpc.screens->get<T>();

    assert(newScreen);

    if (newScreen) openScreenInternal(newScreen);
}

void LayeredScreen::closeCurrentScreen()
{
    const int currentLayerIndex = getFocusedLayerIndex();
    
    if (auto currentScreen = getFocusedLayer()->findChild<ScreenComponent>(); currentScreen)
    {
        currentScreen->close();
        getFocusedLayer()->removeChild(currentScreen);
    }
    
    for (int i = currentLayerIndex - 1; i>= 0; --i  )
    {
        auto screen = layers[i]->findChild<ScreenComponent>();
        
        if (screen)
        {
            openScreenInternal(screen);
            break;
        }
    }
}

void LayeredScreen::openScreenInternal(std::shared_ptr<ScreenComponent> newScreen)
{
    printf("openScreenInternal\n");
    
    while (getFocusedLayerIndex() != -1 && getFocusedLayerIndex() > newScreen->getLayerIndex())
    {
        printf("focused layer index: %i, newScreen layer index: %i\n", getFocusedLayerIndex(), newScreen->getLayerIndex());
        closeCurrentScreen();
    }
        
    auto ams = mpc.getAudioMidiServices();

    if (!history.empty())
    {
        if (isCurrentScreen<SongScreen>() && mpc.getSequencer()->isPlaying())
        {
            return;
        }
        else if (isCurrentScreen<SampleScreen>())
        {
            ams->muteMonitor(true);
            ams->getSoundRecorder()->setSampleScreenActive(false);
        }
        else if (isCurrentScreen<EraseScreen>() ||
                 isCurrentScreen<TimingCorrectScreen>())
        {
            // This field may not be visible the next time we visit this screen.
            // Like the real 2KXL we always set focus to the first Notes: field
            // if the current focus is the second Notes: field.
            if (getFocusedFieldName() == "note1")
            {
                setFocus("note0");
            }
        }

        auto focusedFieldName = getFocusedFieldName();
        auto focus = getFocusedLayer()->findField(focusedFieldName);

        setLastFocus(history.back()->getName(), focusedFieldName);

//        if (focus)
        {
            //focus->loseFocus("");
        }
    }

    if (auto sampleScreen = std::dynamic_pointer_cast<SampleScreen>(newScreen); sampleScreen)
	{
		bool muteMonitor = sampleScreen->getMonitor() == 0;
		ams->muteMonitor(muteMonitor);
		ams->getSoundRecorder()->setSampleScreenActive(true);
	}
    else if (std::dynamic_pointer_cast<NameScreen>(newScreen))
    {
        mpc.getPadAndButtonKeyboard()->resetPreviousPad();
        mpc.getPadAndButtonKeyboard()->resetPressedZeroTimes();
        mpc.getPadAndButtonKeyboard()->resetUpperCase();
    }

    if (!history.empty())
    {
        history.back()->close();
    }
    
    history.push_back(newScreen);
    
    while (history.size() > 5)
    {
        history.pop_front();
    }

    const int screenLayerIndex = newScreen->getLayerIndex();
    
    if (auto existing = layers[screenLayerIndex]->findChild<ScreenComponent>())
    {
        layers[screenLayerIndex]->removeChild(existing);
    }
    
    layers[screenLayerIndex]->addChild(newScreen);
    layers[screenLayerIndex]->sendToBack(newScreen);

	if (newScreen->findFields().size() > 0)
    {
		returnToLastFocus(newScreen->getFirstField());
    }

	newScreen->open();

	mpc.getHardware()->getLed(hardware::ComponentId::OVERDUB_LED)->setEnabled(screengroups::isStepEditorScreen(newScreen->getName()));

	if (!screengroups::isNextSeqScreen(newScreen->getName()) || (std::dynamic_pointer_cast<SequencerScreen>(newScreen) && !mpc.getSequencer()->isPlaying()))
    {
        if (mpc.getSequencer()->getNextSq() != -1)
        {
            mpc.getSequencer()->setNextSq(-1);
        }
    }
}

void LayeredScreen::closeRecentScreensUntilReachingLayer(const int layerIndex)
{
    while(getFocusedLayerIndex() != -1 && getFocusedLayerIndex() != layerIndex)
    {
        closeCurrentScreen();
    }
}

std::vector<std::vector<bool>>* LayeredScreen::getPixels()
{
	return &pixels;
}

void LayeredScreen::Draw()
{
//	MLOG("LayeredScreen::Draw()");
	for (auto& c : root->findHiddenChildren())
        c->drawRecursive(&pixels);

	root->preDrawClear(&pixels);
    root->drawRecursive(&pixels);
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
    assert(!history.empty());
    auto lastFocus = lastFocuses.find(history.back()->getName());
    
    if (lastFocus == end(lastFocuses))
    {
        lastFocuses[history.back()->getName()] = firstFieldOfCurrentScreen;
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
    if (history.empty())
    {
        return "";
    }

	return history.back()->getName();
}

int LayeredScreen::getFocusedLayerIndex()
{
    for (int i = 3; i >= 0; --i)
    {
        if (auto screen = layers[i]->findChild<ScreenComponent>(); screen)
        {
            return screen->getLayerIndex();
        }
    }
    
    return -1;
}

std::shared_ptr<Layer> LayeredScreen::getFocusedLayer()
{
    int idx = getFocusedLayerIndex();
    if (idx < 0 || idx >= (int)layers.size()) {
        throw std::runtime_error("no focused layer");
    }
	return layers[getFocusedLayerIndex()];
}

bool LayeredScreen::transfer(int direction)
{
    if (history.empty())
    {
        return false;
    }

	auto currentFocus = getFocusedLayer()->findField(getFocusedFieldName());
	auto transferMap = history.back()->getTransferMap();
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
	
	auto currentFocus = getFocusedLayer()->findField(getFocusedFieldName());

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

	auto source = getFocusedLayer()->findField(getFocusedFieldName());

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
	auto current = getFocusedLayer()->findField(getFocusedFieldName());
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
	auto result = getFocusedLayer()->findField(getFocusedFieldName());
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

std::string LayeredScreen::getFocusedFieldName()
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

std::shared_ptr<Field> LayeredScreen::getFocusedField()
{
    const auto focusedLayer = getFocusedLayer();
    const auto focusedFieldName = focusedLayer->getFocus();
    return focusedLayer->findField(focusedFieldName);
}

std::string LayeredScreen::getFirstLayerScreenName()
{
    if (auto screen = layers[0]->findChild<ScreenComponent>(); screen)
    {
        return screen->getName();
    }
    
    printf("No screen component found in first layer!\n");
    return "sequencer"; // return some sane default
}

#define X(ns, Class, name) \
    template void mpc::lcdgui::LayeredScreen::openScreen<mpc::lcdgui::ns::Class>(); \
    template void mpc::lcdgui::LayeredScreen::showPopupAndThenOpen<mpc::lcdgui::ns::Class>(std::string, int); \
    template bool mpc::lcdgui::LayeredScreen::isCurrentScreen<mpc::lcdgui::ns::Class>() const; \
    template bool mpc::lcdgui::LayeredScreen::isPreviousScreen<mpc::lcdgui::ns::Class>() const; \
    template bool mpc::lcdgui::LayeredScreen::isPreviousScreenNot<mpc::lcdgui::ns::Class>() const; \
    template bool mpc::lcdgui::LayeredScreen::isCurrentScreenPopupFor<mpc::lcdgui::ns::Class>() const;
SCREEN_LIST
#undef X

template bool mpc::lcdgui::LayeredScreen::isPreviousScreen<mpc::lcdgui::screens::NextSeqScreen,
                                                           mpc::lcdgui::screens::NextSeqPadScreen>() const;
template bool mpc::lcdgui::LayeredScreen::isPreviousScreen<mpc::lcdgui::screens::NextSeqScreen,
                                                           mpc::lcdgui::screens::NextSeqPadScreen,
                                                           mpc::lcdgui::screens::SequencerScreen>() const;
template bool mpc::lcdgui::LayeredScreen::isCurrentScreen<
    mpc::lcdgui::screens::NextSeqScreen,
    mpc::lcdgui::screens::NextSeqPadScreen,
    mpc::lcdgui::screens::SequencerScreen>() const;

// Used by MidiOutputScreen
template bool mpc::lcdgui::LayeredScreen::isPreviousScreenNot<
    mpc::lcdgui::screens::window::NameScreen,
    mpc::lcdgui::screens::dialog::MidiOutputMonitorScreen>() const;

// Used by MonoToStereoScreen and StereoToMonoScreen
template bool mpc::lcdgui::LayeredScreen::isPreviousScreenNot<
    mpc::lcdgui::screens::window::NameScreen,
    mpc::lcdgui::screens::dialog2::PopupScreen>() const;

// Used by StepEditorScreen
template bool mpc::lcdgui::LayeredScreen::isPreviousScreenNot<
    mpc::lcdgui::screens::window::StepTcScreen,
    mpc::lcdgui::screens::window::InsertEventScreen,
    mpc::lcdgui::screens::window::PasteEventScreen,
    mpc::lcdgui::screens::window::EditMultipleScreen>() const;
