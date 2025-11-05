#include "DirectoryScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/LoadScreen.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"

#include "disk/MpcFile.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/Volume.hpp"

#include "audiomidi/AudioMidiServices.hpp"
#include "audiomidi/SoundPlayer.hpp"
#include "engine/audio/server/NonRealTimeAudioServer.hpp"

#include <Util.hpp>

#include "StrUtil.hpp"
#include "lcdgui/FunctionKeys.hpp"
#include "lcdgui/Label.hpp"

namespace mpc::lcdgui
{
    class FunctionKey;
}

using namespace mpc::disk;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;

DirectoryScreen::DirectoryScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "directory", layerIndex)
{
}

void DirectoryScreen::open()
{
    displayLeftFields();
    displayRightFields();
    drawGraphicsLeft();
    drawGraphicsRight();
    refreshFocus();
}

void DirectoryScreen::setFunctionKeys()
{
    if (getSelectedFile())
    {
        auto ext = fs::path(getSelectedFile()->getName()).extension().string();
        auto playable = StrUtil::eqIgnoreCase(ext, ".snd") ||
                        StrUtil::eqIgnoreCase(ext, ".wav");
        ls->setFunctionKeysArrangement(playable ? 1 : 0);
    }
    else
    {
        ls->setFunctionKeysArrangement(0);
    }

    findBackground()->repaintUnobtrusive(
        findChild<FunctionKey>("fk5")->getRect());
}

void DirectoryScreen::function(int f)
{
    ScreenComponent::function(f);

    auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();
    auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
    auto disk = mpc.getDisk();

    switch (f)
    {
        case 1:
            if (!getSelectedFile())
            {
                return;
            }

            if (getSelectedFile()->isDirectory())
            {
                openScreenById(ScreenId::DeleteFolderScreen);
            }
            else
            {
                openScreenById(ScreenId::DeleteFileScreen);
            }

            break;
        case 2:
        {
            auto file = getSelectedFile();

            if (!file)
            {
                return;
            }

            auto fileName =
                mpc::Util::splitName(getSelectedFile()->getName())[0];

            const auto enterAction =
                [this, fileName, file](std::string &nameScreenName)
            {
                auto ext = mpc::Util::splitName(file->getName())[1];

                if (ext.length() > 0)
                {
                    ext = "." + ext;
                }

                const auto finalNewName =
                    StrUtil::trim(StrUtil::toUpper(nameScreenName)) + ext;
                const bool isDirectory = file->isDirectory();
                const auto success = file->setName(finalNewName);

                if (!success)
                {
                    ls->showPopupAndAwaitInteraction("File name exists !!");
                    return;
                }

                const auto disk = mpc.getDisk();
                disk->flush();

                if (isDirectory && getXPos() == 0)
                {
                    disk->moveBack();
                    disk->initFiles();
                    disk->moveForward(finalNewName);
                    disk->initFiles();

                    auto parentFileNames = disk->getParentFileNames();
                    auto it = find(begin(parentFileNames), end(parentFileNames),
                                   finalNewName);

                    auto index = distance(begin(parentFileNames), it);

                    if (index > 4)
                    {
                        setYOffset0(index - 4);
                        setYPos0(4);
                    }
                    else
                    {
                        setYOffset0(0);
                        setYPos0(index);
                    }
                }

                disk->initFiles();
                openScreenById(ScreenId::DirectoryScreen);
            };

            nameScreen->initialize(getSelectedFile()->getNameWithoutExtension(),
                                   file->isDirectory() ? 8 : 16, enterAction,
                                   "directory");

            openScreenById(ScreenId::NameScreen);

            break;
        }
        case 4:
        {
            if (xPos == 0)
            {
                return;
            }

            auto enterAction =
                [this, disk, loadScreen](std::string &nameScreenName)
            {
                bool success =
                    disk->newFolder(StrUtil::toUpper(nameScreenName));

                if (!success)
                {
                    openScreenById(ScreenId::PopupScreen);

                    std::string msg;

                    if (disk->getVolume().mode == MountMode::READ_ONLY)
                    {
                        msg = "Disk is read only !!";
                    }
                    else
                    {
                        msg = "Folder name exists !!";
                    }

                    ls->showPopupAndThenOpen<ScreenId::NameScreen>(msg, 1000);
                    return;
                }

                disk->flush();
                disk->initFiles();
                auto counter = 0;

                for (int i = 0; i < disk->getFileNames().size(); i++)
                {
                    if (disk->getFileName(i) ==
                        StrUtil::toUpper(nameScreenName))
                    {
                        loadScreen->setFileLoad(counter);

                        if (counter > 4)
                        {
                            yOffset1 = counter - 4;
                        }
                        else
                        {
                            yOffset1 = 0;
                        }

                        break;
                    }
                    counter++;
                }

                openScreenById(ScreenId::DirectoryScreen);
            };

            nameScreen->initialize("NEWFOLDR", 8, enterAction, "directory");
            openScreenById(ScreenId::NameScreen);
            break;
        }
        case 5:
        {
            auto file = loadScreen->getSelectedFile();

            if (!file->isDirectory())
            {
                auto ext = fs::path(file->getName()).extension().string();

                bool isWav = StrUtil::eqIgnoreCase(ext, ".wav");
                bool isSnd = StrUtil::eqIgnoreCase(ext, ".snd");

                if (!isWav && !isSnd)
                {
                    return;
                }

                const auto audioServerSampleRate = mpc.getAudioMidiServices()
                                                       ->getAudioServer()
                                                       ->getSampleRate();

                bool started =
                    mpc.getAudioMidiServices()->getSoundPlayer()->start(
                        file->getInputStream(),
                        isSnd ? audiomidi::SoundPlayerFileFormat::SND
                              : audiomidi::SoundPlayerFileFormat::WAV,
                        audioServerSampleRate);

                auto name = file->getNameWithoutExtension();

                if (started)
                {
                    ls->showPopupAndAwaitInteraction("Playing " + name);
                }
                else
                {
                    ls->showPopupAndAwaitInteraction("Can't play " + name);
                }
            }

            break;
        }
    }
}

void DirectoryScreen::turnWheel(int i)
{
    if (i > 0)
    {
        down();
    }
    else
    {
        up();
    }
}

void DirectoryScreen::left()
{
    auto disk = mpc.getDisk();
    auto prevDirName = disk->getDirectoryName();

    if (xPos == 1)
    {
        xPos--;
        refreshFocus();
    }
    else
    {
        if (disk->moveBack())
        {
            auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();
            disk->initFiles();

            loadScreen->fileLoad = 0;
            yPos0 = 0;

            for (int i = 0; i < disk->getParentFileNames().size(); i++)
            {
                if (disk->getParentFileNames()[i] == disk->getDirectoryName())
                {
                    yOffset0 = i;
                    break;
                }
            }

            for (int i = 0; i < disk->getFileNames().size(); i++)
            {
                if (disk->getFileNames()[i] == prevDirName)
                {
                    yOffset1 = i;
                    loadScreen->fileLoad = i;
                    break;
                }
            }

            if (yOffset1 + 1 > disk->getFileNames().size())
            {
                yOffset1 = 0;
            }

            if (disk->getParentFileNames().size() == 0)
            {
                yOffset0 = 0;
            }

            displayLeftFields();
            displayRightFields();
            drawGraphicsLeft();
            drawGraphicsRight();

            refreshFocus();
        }
    }

    setFunctionKeys();
}

void DirectoryScreen::right()
{
    if (xPos == 0)
    {
        xPos++;
        refreshFocus();
    }
    else
    {
        auto disk = mpc.getDisk();

        if (!getSelectedFile() || disk->getFileNames().size() == 0 ||
            !getSelectedFile()->isDirectory())
        {
            return;
        }

        auto f = getSelectedFile();

        if (!disk->moveForward(f->getName()))
        {
            return;
        }

        disk->initFiles();

        yPos0 = 0;
        yOffset1 = 0;

        auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();
        loadScreen->fileLoad = 0;

        for (int i = 0; i < disk->getParentFileNames().size(); i++)
        {
            if (disk->getParentFileNames()[i] == f->getName())
            {
                yOffset0 = i;
                break;
            }
        }

        displayLeftFields();
        displayRightFields();
        drawGraphicsLeft();
        drawGraphicsRight();

        refreshFocus();
    }

    setFunctionKeys();
}

void DirectoryScreen::up()
{
    auto disk = mpc.getDisk();
    auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();

    if (xPos == 0)
    {
        if (yOffset0 == 0 && yPos0 == 0)
        {
            return;
        }

        if (yPos0 == 0)
        {
            yOffset0--;

            auto newDirectoryName = disk->getParentFileNames()[yOffset0];

            if (disk->moveBack())
            {
                disk->initFiles();
                disk->moveForward(newDirectoryName);
                disk->initFiles();

                loadScreen->fileLoad = 0;

                yOffset1 = 0;

                displayLeftFields();
                displayRightFields();
                drawGraphicsLeft();
                drawGraphicsRight();
            }

            return;
        }

        auto newDirectoryName =
            disk->getParentFileNames()[yPos0 - 1 + yOffset0];

        if (disk->moveBack())
        {
            disk->initFiles();
            disk->moveForward(newDirectoryName);
            disk->initFiles();

            yPos0--;
            yOffset1 = 0;

            loadScreen->fileLoad = 0;

            displayLeftFields();
            displayRightFields();
            drawGraphicsLeft();
            drawGraphicsRight();
            refreshFocus();
        }
    }
    else
    {
        if (loadScreen->fileLoad == 0)
        {
            return;
        }

        auto yPos = loadScreen->fileLoad - yOffset1;

        if (yPos == 0)
        {
            yOffset1--;
            loadScreen->fileLoad -= 1;
            displayLeftFields();
            displayRightFields();
            drawGraphicsRight();
        }
        else
        {
            loadScreen->fileLoad -= 1;
            refreshFocus();
        }

        setFunctionKeys();
    }
}

void DirectoryScreen::down()
{
    auto disk = mpc.getDisk();
    auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();

    if (xPos == 0)
    {
        if (disk->isRoot())
        {
            return;
        }

        if (yOffset0 + yPos0 + 1 >= disk->getParentFileNames().size())
        {
            return;
        }

        if (yPos0 == 4)
        {
            yOffset0++;
            auto newDirectoryName = disk->getParentFileNames()[4 + yOffset0];

            if (disk->moveBack())
            {
                disk->initFiles();
                disk->moveForward(newDirectoryName);
                disk->initFiles();
                loadScreen->fileLoad = 0;
                yOffset1 = 0;

                displayLeftFields();
                displayRightFields();
                drawGraphicsLeft();
                drawGraphicsRight();
            }
            return;
        }

        auto newDirectoryName =
            disk->getParentFileNames()[yPos0 + 1 + yOffset0];

        if (disk->moveBack())
        {
            disk->initFiles();
            disk->moveForward(newDirectoryName);
            disk->initFiles();

            yPos0++;
            yOffset1 = 0;
            loadScreen->fileLoad = 0;

            displayLeftFields();
            displayRightFields();
            drawGraphicsLeft();
            drawGraphicsRight();

            refreshFocus();
        }
    }
    else
    {
        if (loadScreen->fileLoad + 1 == disk->getFileNames().size())
        {
            return;
        }

        if (disk->getFileNames().size() == 0)
        {
            return;
        }

        auto yPos = loadScreen->fileLoad - yOffset1;

        if (yPos == 4)
        {
            yOffset1++;
            loadScreen->fileLoad += 1;
            displayLeftFields();
            displayRightFields();
            drawGraphicsRight();
        }
        else
        {
            loadScreen->fileLoad += 1;
            refreshFocus();
        }

        setFunctionKeys();
    }
}

std::shared_ptr<MpcFile> DirectoryScreen::getSelectedFile()
{
    auto yPos = yPos0;

    if (xPos == 1)
    {
        auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();
        yPos = loadScreen->fileLoad - yOffset1;
    }

    return getFileFromGrid(xPos, yPos);
}

std::shared_ptr<MpcFile> DirectoryScreen::getFileFromGrid(int x, int y)
{
    auto disk = mpc.getDisk();

    if (x == 0 && disk->getParentFileNames().size() > y + yOffset0)
    {
        return disk->getParentFile(y + yOffset0);
    }
    else if (x == 1 && disk->getFileNames().size() > y + yOffset1)
    {
        return disk->getFile(y + yOffset1);
    }

    return {};
}

void DirectoryScreen::displayLeftFields()
{
    auto disk = mpc.getDisk();
    auto names = disk->getParentFileNames();

    int size = names.size();

    for (int i = 0; i < 5; i++)
    {
        if (i + yOffset0 > size - 1)
        {
            findField("a" + std::to_string(i))->setText(" ");
        }
        else
        {
            findField("a" + std::to_string(i))->setText(names[i + yOffset0]);
        }
    }

    if (disk->isRoot())
    {
        findField("a0")->setText("ROOT");
    }
}

void DirectoryScreen::displayRightFields()
{
    auto disk = mpc.getDisk();
    int size = disk->getFileNames().size();

    for (int i = 0; i < 5; i++)
    {
        if (i + yOffset1 > size - 1)
        {
            findField("b" + std::to_string(i))->setText(" ");
        }
        else
        {
            auto fileName = fs::path(disk->getFileName(i + yOffset1));
            auto name = StrUtil::padRight(fileName.stem().string(), " ", 16);
            auto ext = fileName.extension().string();

            findField("b" + std::to_string(i))->setText(name + ext);
        }
    }
}

void DirectoryScreen::refreshFocus()
{
    if (xPos == 0)
    {
        ls->setFocus("a" + std::to_string(yPos0));
    }
    else if (xPos == 1)
    {
        auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();
        ls->setFocus("b" + std::to_string(loadScreen->fileLoad - yOffset1));
    }
}

std::vector<std::string> DirectoryScreen::getFirstColumn()
{
    return mpc.getDisk()->getParentFileNames();
}

std::vector<std::string> DirectoryScreen::getSecondColumn()
{
    return mpc.getDisk()->getFileNames();
}

int DirectoryScreen::getXPos()
{
    return xPos;
}

void DirectoryScreen::findYOffset0()
{
    auto disk = mpc.getDisk();
    auto names = disk->getParentFileNames();
    auto dirName = disk->getDirectoryName();
    auto size = names.size();

    for (int i = 0; i < size; i++)
    {
        if (names[i] == dirName)
        {
            yOffset0 = i;
            yPos0 = 0;
            break;
        }
    }
}

void DirectoryScreen::setYOffset0(int i)
{
    yOffset0 = i;
}

void DirectoryScreen::setYOffset1(int i)
{
    if (i < 0)
    {
        return;
    }

    yOffset1 = i;
}

void DirectoryScreen::setYPos0(int i)
{
    yPos0 = i;
}

void DirectoryScreen::drawGraphicsLeft()
{
    auto topLeft = findLabel("topleft");
    auto a0 = findLabel("a0i");
    auto a1 = findLabel("a1i");
    auto a2 = findLabel("a2i");
    auto a3 = findLabel("a3i");
    auto a4 = findLabel("a4i");
    topLeft->setText(" ");
    a0->setText(" ");
    a1->setText(" ");
    a2->setText(" ");
    a3->setText(" ");
    a4->setText(" ");

    auto fc = getFirstColumn();
    std::vector<std::string> currentDirIcons{u8"\u00EF", u8"\u00F1",
                                             u8"\u00F0"};
    std::vector<std::string> dirIcons{u8"\u00EA", u8"\u00EB", u8"\u00EC"};

    std::string notRootDash = u8"\u00ED";
    std::string onlyDirIcon = u8"\u00F3";
    std::string rootIcon = u8"\u00EE";

    int size = fc.size();

    if (size == 0)
    {
        a0->setText(rootIcon);
        return;
    }

    topLeft->setText(notRootDash);

    if (size - yOffset0 == 1)
    {
        if (size > 1)
        {
            a0->setText(currentDirIcons[2]);
        }
        else
        {
            a0->setText(onlyDirIcon);
        }

        return;
    }

    int bottomVisibleFileIndex = size - yOffset0 - 1;
    auto firstVisibleFile = getFileFromGrid(0, 0);
    std::shared_ptr<MpcFile> lastVisibleFile;

    if (bottomVisibleFileIndex > 0)
    {
        if (bottomVisibleFileIndex > 4)
        {
            bottomVisibleFileIndex = 4;
        }

        lastVisibleFile = getFileFromGrid(0, bottomVisibleFileIndex);
    }

    auto disk = mpc.getDisk();
    auto dirName = disk->getDirectoryName();
    int visibleListLength = bottomVisibleFileIndex + 1;

    if ((size - yOffset0) == 2)
    {
        if (firstVisibleFile->getName() == dirName)
        {
            a0->setText(currentDirIcons[0]);
        }
        else
        {
            a0->setText(dirIcons[0]);
        }

        if (lastVisibleFile->getName() == dirName)
        {
            a1->setText(currentDirIcons[2]);
        }
        else
        {
            a1->setText(dirIcons[2]);
        }

        return;
    }

    std::vector<std::shared_ptr<mpc::lcdgui::Label>> aLabels{a0, a1, a2, a3,
                                                             a4};

    if (size - yOffset0 <= 4)
    {
        if (firstVisibleFile->getName() == dirName)
        {
            if (firstVisibleFile->getName() == disk->getParentFileNames()[0])
            {
                a0->setText(currentDirIcons[0]);
            }
            else
            {
                a0->setText(currentDirIcons[1]);
            }
        }
        else
        {
            if (firstVisibleFile->getName() == disk->getParentFileNames()[0])
            {
                a0->setText(dirIcons[0]);
            }
            else
            {
                a0->setText(dirIcons[1]);
            }
        }

        for (int i = 1; i < visibleListLength - 1; i++)
        {
            if (getFileFromGrid(0, i)->getName() == dirName)
            {
                aLabels[i]->setText(currentDirIcons[1]);
            }
            else
            {
                aLabels[i]->setText(dirIcons[1]);
            }
        }

        if (lastVisibleFile->getName() == dirName)
        {
            aLabels[visibleListLength - 1]->setText(currentDirIcons[2]);
        }
        else
        {
            aLabels[visibleListLength - 1]->setText(dirIcons[2]);
        }

        return;
    }

    if (size - yOffset0 >= 5)
    {
        if (firstVisibleFile->getName() == dirName)
        {
            if (firstVisibleFile->getName() == fc[0])
            {
                a0->setText(currentDirIcons[0]);
            }
            else
            {
                a0->setText(currentDirIcons[1]);
            }
        }
        else
        {
            if (firstVisibleFile->getName() == fc[0])
            {
                a0->setText(dirIcons[0]);
            }
            else
            {
                a0->setText(dirIcons[1]);
            }
        }

        for (int i = 1; i < visibleListLength - 1; i++)
        {
            if (fc[i + yOffset0] == dirName)
            {
                aLabels[i]->setText(currentDirIcons[1]);
            }
            else
            {
                aLabels[i]->setText(dirIcons[1]);
            }

            if (i == 3)
            {
                break;
            }
        }

        if (lastVisibleFile->getName() == dirName)
        {
            if (lastVisibleFile->getName() == fc[(int)(size)-1])
            {
                a4->setText(currentDirIcons[2]);
            }
            else
            {
                a4->setText(currentDirIcons[1]);
            }
        }
        else
        {
            if (lastVisibleFile->getName() == fc[(int)(size)-1])
            {
                a4->setText(dirIcons[2]);
            }
            else
            {
                a4->setText(dirIcons[1]);
            }
        }
    }
}

void DirectoryScreen::drawGraphicsRight()
{
    auto b0 = findLabel("b0i");
    auto b1 = findLabel("b1i");
    auto b2 = findLabel("b2i");
    auto b3 = findLabel("b3i");
    auto b4 = findLabel("b4i");

    b0->setText(" ");
    b1->setText(" ");
    b2->setText(" ");
    b3->setText(" ");
    b4->setText(" ");

    auto firstColumn = getFirstColumn();
    auto secondColumn = getSecondColumn();

    auto disk = mpc.getDisk();

    auto parentNames = disk->getParentFileNames();
    auto dirName = disk->getDirectoryName();

    auto a0 = findField("a0");
    auto a1 = findField("a1");
    auto a2 = findField("a2");
    auto a3 = findField("a3");
    auto a4 = findField("a4");

    if (parentNames.size() == 0)
    {
        a0->setText(padFileName(a0->getText(), u8"\u00DF"));
        b0->setText(u8"\u00E0");
    }

    int size = firstColumn.size();

    auto file00 = getFileFromGrid(0, 0);
    auto file01 = getFileFromGrid(0, 1);
    auto file02 = getFileFromGrid(0, 2);
    auto file03 = getFileFromGrid(0, 3);
    auto file04 = getFileFromGrid(0, 4);

    if (size - yOffset0 > 0 && file00->getName() == dirName)
    {
        a0->setText(padFileName(a0->getText(), u8"\u00DF"));
        b0->setText(u8"\u00E0");
    }

    if (size - yOffset0 > 1 && file01->getName() == dirName)
    {
        a1->setText(padFileName(a1->getText(), u8"\u00DF"));
        b1->setText(u8"\u00E0");
    }

    if (size - yOffset0 > 2 && file02->getName() == dirName)
    {
        a2->setText(padFileName(a2->getText(), u8"\u00DF"));
        b2->setText(u8"\u00E0");
    }

    if (size - yOffset0 > 3 && file03->getName() == dirName)
    {
        a3->setText(padFileName(a3->getText(), u8"\u00DF"));
        b3->setText(u8"\u00E0");
    }

    if (size - yOffset0 > 4 && file04->getName() == dirName)
    {
        a4->setText(padFileName(a4->getText(), u8"\u00DF"));
        b4->setText(u8"\u00E0");
    }

    auto c0 = findLabel("c0i");
    auto c1 = findLabel("c1i");
    auto c2 = findLabel("c2i");
    auto c3 = findLabel("c3i");
    auto c4 = findLabel("c4i");

    auto file10 = getFileFromGrid(1, 0);

    if (file10 && file10->isDirectory())
    {
        if (yOffset1 == 0)
        {
            // Set first folder icon
            c0->setText(u8"\u00E1");
        }
        else
        {
            // Set not first folder icon
            c0->setText(u8"\u00E6");
        }
    }
    else
    {
        if (yOffset1 == 0)
        {
            // Set first file icon
            c0->setText(u8"\u00E5");
        }
        else
        {
            // Set not first file icon
            c0->setText(u8"\u00E3");
        }
    }

    auto file11 = getFileFromGrid(1, 1);

    if (file11 && file11->isDirectory())
    {
        c1->setText(u8"\u00E6");
    }
    else
    {
        c1->setText(u8"\u00E3");
    }

    auto file12 = getFileFromGrid(1, 2);

    if (file12 && file12->isDirectory())
    {
        c2->setText(u8"\u00E6");
    }
    else
    {
        c2->setText(u8"\u00E3");
    }

    auto file13 = getFileFromGrid(1, 3);

    if (file13 && file13->isDirectory())
    {
        c3->setText(u8"\u00E6");
    }
    else
    {
        c3->setText(u8"\u00E3");
    }

    auto file14 = getFileFromGrid(1, 4);

    if (file14 && file14->isDirectory())
    {
        if (yOffset1 + 5 == secondColumn.size())
        {
            c4->setText(u8"\u00E2");
        }
        else
        {
            c4->setText(u8"\u00E6");
        }
    }
    else
    {
        if (yOffset1 + 5 == secondColumn.size() || secondColumn.size() <= 5)
        {
            c4->setText(u8"\u00E4");
        }
        else
        {
            c4->setText(u8"\u00E3");
        }
    }
}

std::string DirectoryScreen::padFileName(std::string s, std::string pad)
{
    return StrUtil::padRight(StrUtil::trim(s), pad, 8);
}
