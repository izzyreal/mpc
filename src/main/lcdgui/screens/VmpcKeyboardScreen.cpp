#include "VmpcKeyboardScreen.hpp"

#include <Mpc.hpp>

#include "lcdgui/screens/window/VmpcDiscardMappingChangesScreen.hpp"

#include "lcdgui/Parameter.hpp"
#include "lcdgui/Label.hpp"
#include "lcdgui/Field.hpp"

#include "controller/ClientEventController.hpp"
#include "input/KeyCodeHelper.hpp"

#include <StrUtil.hpp>

#include "input/KeyCodeHelper.hpp"

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::lcdgui;
using namespace mpc::input;

VmpcKeyboardScreen::VmpcKeyboardScreen(mpc::Mpc &mpc, int layerIndex)
    : ScreenComponent(mpc, "vmpc-keyboard", layerIndex)
{
    for (int i = 0; i < 5; i++)
    {
        auto param = std::make_shared<Parameter>(mpc, "                ",
                                                 "row" + std::to_string(i), 2,
                                                 3 + (i * 9), 23 * 6);
        addChild(param);
    }
}

void VmpcKeyboardScreen::turnWheel(int i)
{
    /*
    auto kbMapping =
    mpc.clientEventController->clientHardwareEventController->getKbMapping();
    auto label = kbMapping->getLabelKeyMap()[row + rowOffset].first;
    auto oldKeyCode = kbMapping->getKeyCodeFromLabel(label);
    auto newKeyCode = i > 0 ? KbMapping::getNextKeyCode(oldKeyCode) :
    KbMapping::getPreviousKeyCode(oldKeyCode);

    kbMapping->setKeyCodeForLabel(newKeyCode, label);
    updateRows();
    */
}

void VmpcKeyboardScreen::open()
{
    /*
    auto kbMapping =
    mpc.clientEventController->clientHardwareEventController->getKbMapping();
    auto screen = mpc.screens->get<VmpcDiscardMappingChangesScreen>();
    screen->saveAndLeave = [kbMapping =
    kbMapping](){kbMapping->exportMapping();}; screen->discardAndLeave =
    [kbMapping = kbMapping](){kbMapping->importMapping();}; screen->stayScreen =
    "vmpc-keyboard";

    findChild<Label>("up")->setText("\u00C7");
    findChild<Label>("down")->setText("\u00C6");

    setLearning(false);
    setLearnCandidate(-1);
    updateRows();
    */
}

void VmpcKeyboardScreen::up()
{
    if (learning)
    {
        return;
    }

    if (row == 0)
    {
        if (rowOffset == 0)
        {
            return;
        }

        rowOffset--;
        updateRows();
        return;
    }

    row--;
    updateRows();
}

void VmpcKeyboardScreen::down()
{
    if (learning)
    {
        return;
    }
    /*

    const auto kbMapping =
    mpc.clientEventController->clientHardwareEventController->getKbMapping();

    if (row == 4)
    {
        if (rowOffset + 5 >= kbMapping->getLabelKeyMap().size())
        {
            return;
        }

        rowOffset++;
        updateRows();
        return;
    }

    row++;
    updateRows();
    */
}

void VmpcKeyboardScreen::setLearning(bool b)
{
    learning = b;
    findChild<TextComp>("fk2")->setBlinking(learning);
    findChild<TextComp>("fk3")->setBlinking(learning);
    ls->setFunctionKeysArrangement(learning ? 1 : 0);
}

bool VmpcKeyboardScreen::hasMappingChanged()
{
    // auto persisted = KbMapping(mpc.paths->configPath());
    // auto inMem =
    // mpc.clientEventController->clientHardwareEventController->getKbMapping();
    /*
        for (auto& mapping : inMem->getLabelKeyMap())
        {
            if (inMem->getKeyCodeFromLabel(mapping.first) !=
       persisted.getKeyCodeFromLabel(mapping.first))
            {
                return true;
            }
        }

        for (auto& mapping : persisted.getLabelKeyMap())
        {
            if (inMem->getKeyCodeFromLabel(mapping.first) !=
       persisted.getKeyCodeFromLabel(mapping.first))
            {
                return true;
            }
        }
    */
    return false;
}

void VmpcKeyboardScreen::function(int i)
{
    switch (i)
    {
        case 0:
            if (learning)
            {
                return;
            }

            if (hasMappingChanged())
            {
                auto screen =
                    mpc.screens->get<VmpcDiscardMappingChangesScreen>();
                screen->nextScreen = "vmpc-settings";
                mpc.getLayeredScreen()
                    ->openScreen<VmpcDiscardMappingChangesScreen>();
                return;
            }

            mpc.getLayeredScreen()->openScreen<VmpcSettingsScreen>();
            break;
        case 2:
            if (learning)
            {
                setLearning(false);
                setLearnCandidate(-1);
                updateRows();
                return;
            }

            if (hasMappingChanged())
            {
                auto screen =
                    mpc.screens->get<VmpcDiscardMappingChangesScreen>();
                screen->nextScreen = "vmpc-auto-save";
                mpc.getLayeredScreen()
                    ->openScreen<VmpcDiscardMappingChangesScreen>();
                return;
            }

            mpc.getLayeredScreen()->openScreen<VmpcAutoSaveScreen>();
            break;
        case 3:
            if (learning)
            {
                // const auto kbMapping =
                // mpc.clientEventController->clientHardwareEventController->getKbMapping();
                // const auto mapping = kbMapping->getLabelKeyMap()[row +
                // rowOffset]; const auto oldKeyCode = mapping.second;

                // if (learnCandidate != oldKeyCode)
                {
                    // kbMapping->setKeyCodeForLabel(learnCandidate,
                    // mapping.first);
                }
            }

            setLearning(!learning);

            setLearnCandidate(-1);
            updateRows();
            break;
        case 4:
            if (learning)
            {
                return;
            }

            mpc.getLayeredScreen()->openScreen<VmpcResetKeyboardScreen>();
            break;
        case 5:
            if (learning)
            {
                return;
            }

            std::string popupMsg;

            if (hasMappingChanged())
            {
                // mpc.clientEventController->clientHardwareEventController->getKbMapping()->exportMapping();
                popupMsg = "Keyboard mapping saved";
            }
            else
            {
                popupMsg = "Keyboard mapping unchanged";
            }

            ls->showPopupForMs(popupMsg, 1000);

            break;
    }
}

void VmpcKeyboardScreen::setLearnCandidate(const int rawKeyCode)
{
    learnCandidate = KeyCodeHelper::getVmpcFromPlatformKeyCode(rawKeyCode);
    updateRows();
}

bool VmpcKeyboardScreen::isLearning()
{
    return learning;
}

void VmpcKeyboardScreen::updateRows()
{
    // auto kbMapping =
    // mpc.clientEventController->clientHardwareEventController->getKbMapping();
    // auto& labelKeyMap = kbMapping->getLabelKeyMap();
    const int MAX_LABEL_LENGTH = 15;
    /*
        for (int i = 0; i < 5; i++)
        {
            auto l = findChild<Label>("row" + std::to_string(i));
            auto f = findChild<Field>("row" + std::to_string(i));

            auto mapping = labelKeyMap[i + rowOffset];
            auto labelText = StrUtil::padRight(mapping.first, " ",
       MAX_LABEL_LENGTH) + ":";

            for (auto& c : labelText)
            {
                if (c == '_' || c == '-') c = ' ';
            }

            l->setText(labelText);
            const auto keyCodeDisplayName =
       KeyCodeHelper::getAsciiCompatibleDisplayName(mapping.second);
            f->setText(keyCodeDisplayName);
            f->setInverted(row == i);

            if (learning && i == row)
            {
                f->setText(KeyCodeHelper::getAsciiCompatibleDisplayName(learnCandidate));
                f->setBlinking(true);
            }
            else
            {
                f->setBlinking(false);
            }
        }
     */
    displayUpAndDown();
}

void VmpcKeyboardScreen::displayUpAndDown()
{
    // auto labelKeyMapSize =
    // mpc.clientEventController->clientHardwareEventController->getKbMapping()->getLabelKeyMap().size();
    findChild<Label>("up")->Hide(rowOffset == 0);
    // findChild<Label>("down")->Hide(rowOffset + 5 >= labelKeyMapSize);
}
