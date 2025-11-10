#include "lcdgui/LayeredScreen.hpp"
#include "sequencer/Transport.hpp"

#include "lcdgui/BMFParser.hpp"
#include "lcdgui/ScreenGroups.hpp"
#include "lcdgui/ScreenNames.hpp"
#include "lcdgui/Screens.hpp"
#include "lcdgui/AllScreens.hpp"

#include "Mpc.hpp"

#include "Field.hpp"
#include "Component.hpp"

#include "audiomidi/AudioMidiServices.hpp"
#include "audiomidi/SoundRecorder.hpp"

#include "lcdgui/Layer.hpp"

#include "StrUtil.hpp"
#include "Util.hpp"

#include "MpcResourceUtil.hpp"
#include "input/PadAndButtonKeyboard.hpp"
#include "hardware/Hardware.hpp"
#include "sequencer/Sequencer.hpp"

#include <memory>
#include <stdexcept>

#if __linux__
#include <climits>
#endif

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::dialog2;

LayeredScreen::LayeredScreen(mpc::Mpc &mpc) : mpc(mpc)
{
    const auto fntPath = "fonts/mpc2000xl-font.fnt";
    auto fntData = MpcResourceUtil::get_resource_data(fntPath);

    const auto bmpPath = "fonts/mpc2000xl-font_0.bmp";
    auto bmpData = MpcResourceUtil::get_resource_data(bmpPath);

    BMFParser bmfParser(&fntData[0], fntData.size(), &bmpData[0],
                        bmpData.size());

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

void LayeredScreen::postToUiThread(const std::function<void()> &fn)
{
    uiTasks.post(fn);
}

bool LayeredScreen::isPreviousScreen(std::initializer_list<ScreenId> ids) const
{
    if (history.size() < 2)
    {
        return false;
    }

    const auto &prev = history[history.size() - 2];
    const auto id = getScreenId(prev);

    for (const auto candidate : ids)
    {
        if (id == candidate)
        {
            return true;
        }
    }

    return false;
}

bool LayeredScreen::isPreviousScreenNot(
    std::initializer_list<ScreenId> ids) const
{
    if (history.size() < 2)
    {
        return true;
    }

    const auto &prev = history[history.size() - 2];
    const auto id = getScreenId(prev);

    for (const auto candidate : ids)
    {
        if (id == candidate)
        {
            return false;
        }
    }

    return true;
}

bool LayeredScreen::isCurrentScreen(std::initializer_list<ScreenId> ids) const
{
    if (history.empty())
    {
        return false;
    }

    const auto &curr = history.back();
    if (!curr)
    {
        return false;
    }

    const auto id = getScreenId(curr);

    for (const auto candidate : ids)
    {
        if (id == candidate)
        {
            return true;
        }
    }

    return false;
}

bool LayeredScreen::isCurrentScreenPopupFor(ScreenId targetId) const
{
    if (history.size() < 2)
    {
        return false;
    }

    const auto curr = history.back();
    const auto prev = history[history.size() - 2];

    return getScreenId(curr) == ScreenId::PopupScreen &&
           getScreenId(prev) == targetId;
}

ScreenId
LayeredScreen::getScreenId(const std::shared_ptr<ScreenComponent> &screen)
{
    if (!screen)
    {
        return ScreenId::Count;
    }

#define X(ns, Class, nameStr)                                                  \
    if (dynamic_cast<ns::Class *>(screen.get()))                               \
        return ScreenId::Class;
    SCREEN_LIST
#undef X

    return ScreenId::Count;
}

void LayeredScreen::setPopupScreenText(const std::string &text) const
{
    mpc.screens->get<ScreenId::PopupScreen>()->setText(text);
}

void LayeredScreen::showPopup(const std::string &msg)
{
    PopupScreen::PopupConfig cfg{msg};
    mpc.screens->get<ScreenId::PopupScreen>()->show(cfg);
    openScreenById(ScreenId::PopupScreen);
}

void LayeredScreen::showPopupForMs(const std::string &msg, int delayMs)
{
    PopupScreen::PopupConfig cfg{
        msg,
        false,
        delayMs,
        {PopupScreen::PopupBehavior::OnTimeoutAction::Close}};
    mpc.screens->get<ScreenId::PopupScreen>()->show(cfg);
    openScreenById(ScreenId::PopupScreen);
}

void LayeredScreen::showPopupAndThenOpen(ScreenId targetId,
                                         const std::string &msg, int delayMs)
{
    PopupScreen::PopupConfig cfg{
        msg,
        false,
        delayMs,
        {PopupScreen::PopupBehavior::OnTimeoutAction::OpenScreen, std::nullopt,
         targetId}};
    mpc.screens->get<ScreenId::PopupScreen>()->show(cfg);
    openScreenById(ScreenId::PopupScreen);
}

void LayeredScreen::showPopupAndThenReturnToLayer(const std::string &msg,
                                                  int delayMs, int layerIndex)
{
    PopupScreen::PopupConfig cfg{
        msg,
        false,
        delayMs,
        {PopupScreen::PopupBehavior::OnTimeoutAction::ReturnToLayer,
         layerIndex}};
    mpc.screens->get<ScreenId::PopupScreen>()->show(cfg);
    openScreenById(ScreenId::PopupScreen);
}

void LayeredScreen::showPopupAndAwaitInteraction(const std::string &msg)
{
    PopupScreen::PopupConfig cfg{msg, true};
    mpc.screens->get<ScreenId::PopupScreen>()->show(cfg);
    openScreenById(ScreenId::PopupScreen);
}

std::shared_ptr<ScreenComponent> LayeredScreen::getCurrentScreen()
{
    assert(!history.empty());
    return history.back();
}

void LayeredScreen::openScreen(const std::string &name)
{
    if (const auto id = getScreenIdByName(name))
    {
        openScreenById(*id);
    }
}

void LayeredScreen::openScreenById(const ScreenId id)
{
    if (!history.empty() && history.back() && getScreenId(history.back()) == id)
    {
        return;
    }

    std::shared_ptr<mpc::lcdgui::ScreenComponent> newScreen;

    switch (id)
    {
#define X(ns, Class, name)                                                     \
    case ScreenId::Class:                                                      \
        newScreen = mpc.screens->get<ScreenId::Class>();                       \
        break;
        SCREEN_LIST
#undef X
        default:
            break;
    }

    assert(newScreen);
    if (newScreen)
    {
        openScreenInternal(newScreen);
    }
}

void LayeredScreen::closeCurrentScreen()
{
    const int currentLayerIndex = getFocusedLayerIndex();

    if (currentLayerIndex == 0)
    {
        return;
    }

    if (const auto currentScreen =
            getFocusedLayer()->findChild<ScreenComponent>();
        currentScreen)
    {
        if (const auto focusedField = currentScreen->findFocus(); focusedField)
        {
            const auto focusedFieldName = focusedField->getName();
            setLastFocus(currentScreen->getName(), focusedFieldName);
        }

        currentScreen->close();
        getFocusedLayer()->removeChild(currentScreen);
    }

    for (int i = currentLayerIndex - 1; i >= 0; --i)
    {

        if (const auto screen = layers[i]->findChild<ScreenComponent>())
        {
            openScreenInternal(screen);
            break;
        }
    }
}

void LayeredScreen::openScreenInternal(
    const std::shared_ptr<ScreenComponent> &newScreen)
{
    const auto ams = mpc.getAudioMidiServices();

    if (!history.empty())
    {
        if (isCurrentScreen({ScreenId::SongScreen}) &&
            mpc.getSequencer()->getTransport()->isPlaying())
        {
            return;
        }
        else if (isCurrentScreen({ScreenId::SampleScreen}))
        {
            ams->muteMonitor(true);
            ams->getSoundRecorder()->setSampleScreenActive(false);
        }
        else if (isCurrentScreen({ScreenId::EraseScreen}) ||
                 isCurrentScreen({ScreenId::TimingCorrectScreen}))
        {
            // This field may not be visible the next time we visit this screen.
            // Like the real 2KXL we always set focus to the first Notes: field
            // if the current focus is the second Notes: field.
            if (getFocusedFieldName() == "note1")
            {
                setFocus("note0");
            }
        }
    }

    while (getFocusedLayerIndex() != -1 &&
           getFocusedLayerIndex() > newScreen->getLayerIndex())
    {
        closeCurrentScreen();
    }

    if (const auto sampleScreen =
            std::dynamic_pointer_cast<SampleScreen>(newScreen);
        sampleScreen)
    {
        const bool muteMonitor = sampleScreen->getMonitor() == 0;
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
        if (const auto focusedField = history.back()->findFocus(); focusedField)
        {
            const auto focusedFieldName = focusedField->getName();
            setLastFocus(history.back()->getName(), focusedFieldName);
        }
        history.back()->close();
    }

    history.push_back(newScreen);

    while (history.size() > 5)
    {
        history.pop_front();
    }

    const int screenLayerIndex = newScreen->getLayerIndex();

    if (const auto existing =
            layers[screenLayerIndex]->findChild<ScreenComponent>())
    {
        layers[screenLayerIndex]->removeChild(existing);
    }

    layers[screenLayerIndex]->addChild(newScreen);
    layers[screenLayerIndex]->sendToBack(newScreen);

    if (newScreen->findFields().size() > 0)
    {
        returnToLastFocus(newScreen, newScreen->getFirstField());
    }

    newScreen->open();

    mpc.getHardware()
        ->getLed(hardware::ComponentId::OVERDUB_LED)
        ->setEnabled(screengroups::isStepEditorScreen(newScreen));

    if (std::dynamic_pointer_cast<NextSeqScreen>(newScreen))
    {
        mpc::Util::initSequence(mpc);
    }

    mpc.getHardware()
        ->getLed(hardware::ComponentId::NEXT_SEQ_LED)
        ->setEnabled(mpc.getLayeredScreen()->isCurrentScreen(
            {ScreenId::NextSeqScreen, ScreenId::NextSeqScreen}));

    mpc.getHardware()
        ->getLed(hardware::ComponentId::TRACK_MUTE_LED)
        ->setEnabled(
            mpc.getLayeredScreen()->isCurrentScreen({ScreenId::TrMuteScreen}));

    if (!screengroups::isNextSeqScreen(newScreen) ||
        (std::dynamic_pointer_cast<SequencerScreen>(newScreen) &&
         !mpc.getSequencer()->getTransport()->isPlaying()))
    {
        if (mpc.getSequencer()->getNextSq() != -1)
        {
            mpc.getSequencer()->setNextSq(-1);
        }
    }
}

void LayeredScreen::closeRecentScreensUntilReachingLayer(const int layerIndex)
{
    while (getFocusedLayerIndex() != -1 && getFocusedLayerIndex() != layerIndex)
    {
        closeCurrentScreen();
    }
}

std::vector<std::vector<bool>> *LayeredScreen::getPixels()
{
    return &pixels;
}

void LayeredScreen::timerCallback()
{
    uiTasks.drain();

    if (const auto currentScreen = getCurrentScreen(); currentScreen)
    {
        currentScreen->timerCallback();
    }
}

void LayeredScreen::Draw()
{
    for (const auto &c : root->findHiddenChildren())
    {
        c->drawRecursive(&pixels);
    }

    root->preDrawClear(&pixels);
    root->drawRecursive(&pixels);
    return;
}

MRECT LayeredScreen::getDirtyArea() const
{
    const auto dirtyArea = root->getDirtyArea();
    return dirtyArea;
}

bool LayeredScreen::IsDirty() const
{
    return root->IsDirty();
}

void LayeredScreen::setDirty() const
{
    root->SetDirty();
}

Background *LayeredScreen::getCurrentBackground()
{
    return getFocusedLayer()->getBackground();
}

void LayeredScreen::setCurrentBackground(const std::string &s)
{
    getCurrentBackground()->setBackgroundName(s);
}

void LayeredScreen::returnToLastFocus(
    const std::shared_ptr<ScreenComponent> &screen,
    const std::string &firstFieldOfCurrentScreen)
{
    assert(screen);
    const auto lastFocus = lastFocuses.find(screen->getName());

    if (lastFocus == end(lastFocuses))
    {
        lastFocuses[screen->getName()] = firstFieldOfCurrentScreen;
        setFocus(firstFieldOfCurrentScreen);
        return;
    }

    setFocus(lastFocus->second);
}

void LayeredScreen::setLastFocus(const std::string &screenName,
                                 const std::string &newLastFocus)
{
    lastFocuses[screenName] = newLastFocus;
}

std::string LayeredScreen::getLastFocus(const std::string &screenName)
{
    const auto lastFocus = lastFocuses.find(screenName);

    if (lastFocus == end(lastFocuses))
    {
        return "";
    }

    return lastFocus->second;
}

std::string LayeredScreen::getCurrentScreenName() const
{
    if (history.empty())
    {
        return "";
    }

    return history.back()->getName();
}

int LayeredScreen::getFocusedLayerIndex() const
{
    for (int i = 3; i >= 0; --i)
    {
        if (const auto screen = layers[i]->findChild<ScreenComponent>(); screen)
        {
            return screen->getLayerIndex();
        }
    }

    return -1;
}

std::shared_ptr<Layer> LayeredScreen::getFocusedLayer()
{
    const int idx = getFocusedLayerIndex();
    if (idx < 0 || idx >= static_cast<int>(layers.size()))
    {
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

    const auto currentFocus =
        getFocusedLayer()->findField(getFocusedFieldName());
    auto transferMap = history.back()->getTransferMap();
    const auto mapCandidate = transferMap.find(currentFocus->getName());

    if (mapCandidate == end(transferMap))
    {
        return false;
    }

    const auto mapping = mapCandidate->second;
    const auto nextFocusNames = StrUtil::split(mapping[direction], ',');

    for (auto &nextFocusName : nextFocusNames)
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

    const auto currentFocus =
        getFocusedLayer()->findField(getFocusedFieldName());

    std::shared_ptr<Field> candidate;

    for (auto &f : getFocusedLayer()->findFields())
    {
        if (f == currentFocus || !f->isFocusable() || f->IsHidden())
        {
            continue;
        }

        const int verticalOffset = abs(currentFocus->getY() - f->getY());

        if (verticalOffset > 2)
        {
            continue;
        }

        const int candidateVerticalOffset =
            candidate ? abs(currentFocus->getY() - candidate->getY()) : INT_MAX;

        if (verticalOffset <= candidateVerticalOffset)
        {

            if (f->getX() > currentFocus->getX())
            {
                continue;
            }

            const int horizontalOffset = currentFocus->getX() - f->getX();
            const int candidateHorizontalOffset =
                candidate ? currentFocus->getX() - candidate->getX() : INT_MAX;

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

    const auto source = getFocusedLayer()->findField(getFocusedFieldName());

    for (auto &f : getFocusedLayer()->findFields())
    {
        if (f == source || !f->isFocusable() || f->IsHidden())
        {
            continue;
        }

        const int verticalOffset = abs(source->getY() - f->getY());

        if (verticalOffset > 2)
        {
            continue;
        }

        const int candidateVerticalOffset =
            candidate ? abs(source->getY() - candidate->getY()) : INT_MAX;

        if (verticalOffset <= candidateVerticalOffset)
        {

            if (f->getX() < source->getX())
            {
                continue;
            }

            const int horizontalOffset = f->getX() - source->getX();
            const int candidateHorizontalOffset =
                candidate ? candidate->getX() - source->getX() : INT_MAX;

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
    constexpr int minDistV = 7;
    int maxDistH = 6 * marginChars;
    const auto current = getFocusedLayer()->findField(getFocusedFieldName());
    std::shared_ptr<Field> next;

    for (const auto &field : getFocusedLayer()->findFields())
    {
        const auto B1 = field->getRect().B;
        const auto B0 = current->getRect().B;
        const auto MW1 =
            0.5f * static_cast<float>(field->getX() * 2 + field->getW());
        const auto MW0 =
            0.5f * static_cast<float>(current->getX() * 2 + current->getW());

        if (B1 - B0 >= minDistV)
        {
            if (abs(static_cast<int>(MW1 - MW0)) <= maxDistH)
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

        for (const auto &field : getFocusedLayer()->findFields())
        {
            const auto B0 = current->getY() + current->getH();
            const auto B1 = field->getY() + field->getH();
            const auto MW0 = 0.5f * static_cast<float>(current->getX() * 2 +
                                                       current->getW());
            const auto MW1 =
                0.5f * static_cast<float>(field->getX() * 2 + field->getW());

            if (B1 - B0 >= minDistV)
            {
                if (abs(static_cast<int>(MW1 - MW0)) <= maxDistH)
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

void LayeredScreen::transferUp()
{
    std::shared_ptr<Field> newCandidate;

    if (transfer(2))
    {
        return;
    }

    int marginChars = 8;
    constexpr int minDistV = -7;
    int maxDistH = 6 * marginChars;
    const auto result = getFocusedLayer()->findField(getFocusedFieldName());
    std::shared_ptr<Field> next;

    auto revComponents = getFocusedLayer()->findFields();

    reverse(revComponents.begin(), revComponents.end());

    for (const auto &field : revComponents)
    {
        const auto B1 = field->getY() + field->getH();
        const auto B0 = result->getY() + result->getH();
        const auto MW1 =
            0.5f * static_cast<float>(field->getX() * 2 + field->getW());
        const auto MW0 =
            0.5f * static_cast<float>(result->getX() * 2 + result->getW());

        if (B1 - B0 <= minDistV)
        {
            if (abs(static_cast<int>(MW1 - MW0)) <= maxDistH)
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

        for (const auto &field : revComponents)
        {
            const auto B1 = field->getY() + field->getH();
            const auto B0 = result->getY() + result->getH();
            const auto MW1 =
                0.5f * static_cast<float>(field->getX() * 2 + field->getW());
            const auto MW0 =
                0.5f * static_cast<float>(field->getX() * 2 + field->getW());

            if (B1 - B0 <= minDistV)
            {
                if (abs(static_cast<int>(MW1 - MW0)) <= maxDistH)
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
    auto focusedLayer = getFocusedLayer();
    return getFocusedLayer()->getFocus();
}

bool LayeredScreen::setFocus(const std::string &focus)
{
    return getFocusedLayer()->setFocus(focus);
}

void LayeredScreen::setFunctionKeysArrangement(int arrangementIndex)
{
    getFunctionKeys()->setActiveArrangement(arrangementIndex);
}

FunctionKeys *LayeredScreen::getFunctionKeys()
{
    return getFocusedLayer()->getFunctionKeys();
}

std::shared_ptr<Field> LayeredScreen::getFocusedField()
{
    const auto focusedLayer = getFocusedLayer();
    const auto focusedFieldName = focusedLayer->getFocus();
    return focusedLayer->findField(focusedFieldName);
}

std::string LayeredScreen::getFirstLayerScreenName() const
{
    if (const auto screen = layers[0]->findChild<ScreenComponent>(); screen)
    {
        return screen->getName();
    }

    printf("No screen component found in first layer!\n");
    return "sequencer"; // return some sane default
}
