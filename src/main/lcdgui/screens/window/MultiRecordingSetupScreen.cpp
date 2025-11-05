#include "MultiRecordingSetupScreen.hpp"

#include "StrUtil.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/VmpcSettingsScreen.hpp"

#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

using namespace mpc::lcdgui::screens::window;

MultiRecordingSetupScreen::MultiRecordingSetupScreen(mpc::Mpc &mpc,
                                                     const int layerIndex)
    : ScreenComponent(mpc, "multi-recording-setup", layerIndex)
{
    for (int i = 0; i < 34; i++)
    {
        mrsLines[i].setTrack(i);
        mrsLines[i].setIn(i);
    }

    for (int i = 0; i < 34; i++)
    {
        if (i < 16)
        {
            inNames[i] = "1-" + StrUtil::padLeft(std::to_string(i + 1), "0", 2);
        }

        if (i == 16)
        {
            inNames[i] = "1-Ex";
        }

        if (i > 16 && i < 33)
        {
            inNames[i] =
                "2-" + StrUtil::padLeft(std::to_string(i - 16), "0", 2);
        }

        if (i == 33)
        {
            inNames[i] = "2-Ex";
        }
    }
}

void MultiRecordingSetupScreen::function(int i)
{
    if (i == 3)
    {
        openScreenById(ScreenId::SequencerScreen);
    }
}

void MultiRecordingSetupScreen::open()
{
    setYOffset(yOffset);
    displayMrsLine(0);
    displayMrsLine(1);
    displayMrsLine(2);
}

void MultiRecordingSetupScreen::left()
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName[0] == 'a')
    {
        return;
    }

    ScreenComponent::left();
}

void MultiRecordingSetupScreen::right()
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName[0] == 'c')
    {
        return;
    }

    ScreenComponent::right();
}

void MultiRecordingSetupScreen::turnWheel(int i)
{
    auto seq = sequencer->getActiveSequence();

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName[0] == 'a')
    {
        if (i > 0)
        {
            if (yPos == 0)
            {
                std::string res =
                    focusedFieldName.substr(0, 1) + std::to_string(yPos + 1);
                ls->setFocus(focusedFieldName.substr(0, 1).append(
                    std::to_string(yPos + 1)));
            }
            else if (yPos == 1)
            {
                ls->setFocus(focusedFieldName.substr(0, 1).append(
                    std::to_string(yPos + 1)));
            }
            else if (yPos == 2)
            {
                setYOffset(yOffset + 1);
            }
        }
        else if (i < 0)
        {
            if (yPos == 0)
            {
                setYOffset(yOffset - 1);
            }
            else if (yPos == 1)
            {
                ls->setFocus(focusedFieldName.substr(0, 1).append(
                    std::to_string(yPos - 1)));
            }
            else if (yPos == 2)
            {
                ls->setFocus(focusedFieldName.substr(0, 1).append(
                    std::to_string(yPos - 1)));
            }
        }
    }
    else if (focusedFieldName[0] == 'b')
    {
        setMrsTrack(yPos + yOffset, visibleMrsLines[yPos]->getTrack() + i);
    }
    else if (focusedFieldName[0] == 'c')
    {
        if (visibleMrsLines[yPos]->getTrack() != -1)
        {
            auto track = seq->getTrack(visibleMrsLines[yPos]->getTrack());
            track->setDeviceIndex(track->getDeviceIndex() + i);
            displayMrsLine(yPos);
        }
    }
}

void MultiRecordingSetupScreen::up()
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (yPos == 0)
    {
        setYOffset(yOffset - 1);
    }
    else if (yPos == 1)
    {
        ls->setFocus(
            focusedFieldName.substr(0, 1).append(std::to_string(yPos - 1)));
        --yPos;
    }
    else if (yPos == 2)
    {
        ls->setFocus(
            focusedFieldName.substr(0, 1).append(std::to_string(yPos - 1)));
        --yPos;
    }
}

void MultiRecordingSetupScreen::down()
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (yPos == 0)
    {
        std::string res =
            focusedFieldName.substr(0, 1) + std::to_string(yPos + 1);
        ls->setFocus(
            focusedFieldName.substr(0, 1).append(std::to_string(yPos + 1)));
        ++yPos;
    }
    else if (yPos == 1)
    {
        ls->setFocus(
            focusedFieldName.substr(0, 1).append(std::to_string(yPos + 1)));
        ++yPos;
    }
    else if (yPos == 2)
    {
        setYOffset(yOffset + 1);
    }
}

void MultiRecordingSetupScreen::displayMrsLine(int i)
{
    auto seq = sequencer->getActiveSequence();
    auto trackIndex = visibleMrsLines[i]->getTrack();

    auto aField = findField("a" + std::to_string(i));
    auto bField = findField("b" + std::to_string(i));
    auto cField = findField("c" + std::to_string(i));

    aField->setText(inNames[visibleMrsLines[i]->getIn()]);

    if (visibleMrsLines[i]->getTrack() == -1)
    {
        bField->setText("---OFF");
    }
    else
    {
        auto track = seq->getTrack(visibleMrsLines[i]->getTrack());
        auto trackNumber = std::to_string(trackIndex + 1);
        trackNumber = StrUtil::padLeft(trackNumber, "0", 2);
        bField->setText(std::string(trackNumber + "-" + track->getName()));
    }

    if (trackIndex == -1)
    {
        cField->setText("");
    }
    else
    {
        auto track = seq->getTrack(visibleMrsLines[i]->getTrack());
        auto dev = track->getDeviceIndex();

        if (dev == 0)
        {
            cField->setText("OFF");
        }
        else if (dev < 17)
        {
            cField->setTextPadded(std::to_string(dev) + "A", " ");
        }
        else if (dev <= 32)
        {
            cField->setTextPadded(std::to_string(dev - 16) + "B", " ");
        }
    }
}

void MultiRecordingSetupScreen::setYOffset(int i)
{
    i = std::clamp(i, 0, static_cast<int>(mrsLines.size()) - 3);

    visibleMrsLines = std::vector<MultiRecordingSetupLine *>(3);
    yOffset = i;

    for (auto j = 0; j < 3; j++)
    {
        visibleMrsLines[j] = &mrsLines[yOffset + j];
    }

    displayMrsLine(0);
    displayMrsLine(1);
    displayMrsLine(2);
}

void MultiRecordingSetupScreen::setMrsTrack(int inputNumber, int newTrackNumber)
{
    mrsLines[inputNumber].setTrack(newTrackNumber);

    visibleMrsLines = std::vector<MultiRecordingSetupLine *>(3);

    for (auto j = 0; j < 3; j++)
    {
        visibleMrsLines[j] = &mrsLines[yOffset + j];
    }

    const auto focusedFieldName = getFocusedFieldNameOrThrow();
    auto displayYPos = stoi(focusedFieldName.substr(1, 2));
    displayMrsLine(displayYPos);
}

std::vector<MultiRecordingSetupLine *> MultiRecordingSetupScreen::getMrsLines()
{
    std::vector<MultiRecordingSetupLine *> result;

    for (auto &mrsLine : mrsLines)
    {
        result.push_back(&mrsLine);
    }

    return result;
}
