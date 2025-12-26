#include "VmpcKeyboardScreen.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "controller/ClientEventController.hpp"

#include "lcdgui/screens/window/VmpcDiscardMappingChangesScreen.hpp"

#include "lcdgui/Parameter.hpp"
#include "lcdgui/Label.hpp"

#include "input/KeyCodeHelper.hpp"
#include "input/KeyboardBindingsWriter.hpp"

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::lcdgui;
using namespace mpc::input;

VmpcKeyboardScreen::VmpcKeyboardScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "vmpc-keyboard", layerIndex)
{
    for (int i = 0; i < 5; i++)
    {
        auto param = std::make_shared<Parameter>(mpc, "                ",
                                                 "row" + std::to_string(i), 2,
                                                 3 + i * 9, 23 * 6);
        addChild(param);
    }
}

void VmpcKeyboardScreen::turnWheel(const int increment)
{
    const auto binding = bindings->getByIndex(row + rowOffset);
    const auto oldKeyCode = static_cast<int>(binding->keyCode);
    const auto newKeyCode =
        std::clamp(oldKeyCode + increment, -1,
                   static_cast<int>(VmpcKeyCode::VMPC_KEY_ENUMERATOR_SIZE) - 1);

    binding->keyCode = static_cast<VmpcKeyCode>(newKeyCode);
    updateRows();
}

void VmpcKeyboardScreen::open()
{
    if (!ls.lock()->isPreviousScreen({ScreenId::PopupScreen,
                                      ScreenId::VmpcDiscardMappingChangesScreen,
                                      ScreenId::VmpcResetKeyboardScreen}))
    {
        bindings = std::make_unique<KeyboardBindings>(
            mpc.clientEventController->getKeyboardBindings()
                ->getKeyboardBindingsData());
    }

    const auto screen =
        mpc.screens->get<ScreenId::VmpcDiscardMappingChangesScreen>();

    screen->saveAndLeave = [this, path = mpc.paths->keyboardBindingsPath()]
    {
        const auto jsonData = KeyboardBindingsWriter::toJson(*bindings);
        set_file_data(path, jsonData.dump());
        mpc.clientEventController->getKeyboardBindings()->setBindingsData(
            bindings->getKeyboardBindingsData());
    };

    screen->discardAndLeave = [this]
    {
        bindings->setBindingsData(
            mpc.clientEventController->getKeyboardBindings()
                ->getKeyboardBindingsData());
    };

    screen->stayScreen = "vmpc-keyboard";

    findChild<Label>("up")->setText("\u00C7");
    findChild<Label>("down")->setText("\u00C6");

    setLearning(false);
    setLearnCandidate(-1);
    updateRows();
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

    const auto bindingCount = bindings->getBindingCount();

    if (row == 4)
    {
        if (rowOffset + 5 >= bindingCount)
        {
            return;
        }

        rowOffset++;
        updateRows();
        return;
    }

    row++;
    updateRows();
}

void VmpcKeyboardScreen::setLearning(const bool b)
{
    learning = b;
    findChild<TextComp>("fk2")->setBlinking(learning);
    findChild<TextComp>("fk3")->setBlinking(learning);
    ls.lock()->setFunctionKeysArrangement(learning ? 1 : 0);
}

bool VmpcKeyboardScreen::hasMappingChanged() const
{
    const auto inMemoryBindings =
        mpc.clientEventController->getKeyboardBindings();

    return !inMemoryBindings->isSameAs(bindings->getKeyboardBindingsData());
}

void VmpcKeyboardScreen::function(const int i)
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
                const auto screen =
                    mpc.screens
                        ->get<ScreenId::VmpcDiscardMappingChangesScreen>();
                screen->setNextScreen("vmpc-settings");
                openScreenById(ScreenId::VmpcDiscardMappingChangesScreen);
                return;
            }

            openScreenById(ScreenId::VmpcSettingsScreen);
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
                const auto screen =
                    mpc.screens
                        ->get<ScreenId::VmpcDiscardMappingChangesScreen>();
                screen->setNextScreen("vmpc-auto-save");
                openScreenById(ScreenId::VmpcDiscardMappingChangesScreen);
                return;
            }

            openScreenById(ScreenId::VmpcAutoSaveScreen);
            break;
        case 3:
            if (learning)
            {
                const auto binding = bindings->getByIndex(row + rowOffset);

                if (const auto oldKeyCode = binding->keyCode;
                    learnCandidate != oldKeyCode)
                {
                    binding->keyCode = learnCandidate;
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

            openScreenById(ScreenId::VmpcResetKeyboardScreen);
            break;
        case 5:
        {
            if (learning)
            {
                return;
            }

            std::string popupMsg;

            if (hasMappingChanged())
            {
                const auto jsonData = KeyboardBindingsWriter::toJson(*bindings);
                set_file_data(mpc.paths->keyboardBindingsPath(),
                              jsonData.dump());

                mpc.clientEventController->getKeyboardBindings()
                    ->setBindingsData(bindings->getKeyboardBindingsData());

                popupMsg = "Keyboard mapping saved";
            }
            else
            {
                popupMsg = "Keyboard mapping unchanged";
            }

            ls.lock()->showPopupForMs(popupMsg, 1000);

            break;
        }
        default:;
    }
}

void VmpcKeyboardScreen::setLearnCandidate(const int platformKeyCode)
{
    learnCandidate = KeyCodeHelper::getVmpcFromPlatformKeyCode(platformKeyCode);
    updateRows();
}

bool VmpcKeyboardScreen::isLearning() const
{
    return learning;
}

void VmpcKeyboardScreen::updateRows()
{
    const auto &bindingsData = bindings->getKeyboardBindingsData();

    for (int i = 0; i < 5; i++)
    {
        constexpr int MAX_LABEL_LENGTH = 15;
        const auto rowLabel = findChild<Label>("row" + std::to_string(i));
        const auto rowField = findChild<Field>("row" + std::to_string(i));

        const auto binding = bindingsData[i + rowOffset];

        auto labelName = binding.componentLabel;

        for (auto &c : labelName)
        {
            if (c == '_' || c == '-')
            {
                c = ' ';
            }
        }

        if (const auto pos = labelName.find(" or"); pos != std::string::npos)
        {
            labelName.erase(pos);
        }

        if (binding.direction == Direction::Negative)
        {
            labelName += " down";
        }
        else if (binding.direction == Direction::Positive)
        {
            labelName += " up";
        }

        labelName = labelName.substr(0, MAX_LABEL_LENGTH);

        auto labelText =
            StrUtil::padRight(labelName, " ", MAX_LABEL_LENGTH) + ":";

        rowLabel->setText(labelText);
        const auto keyCodeDisplayName =
            KeyCodeHelper::getAsciiCompatibleDisplayName(binding.keyCode);
        rowField->setText(keyCodeDisplayName);
        rowField->setInverted(row == i);

        if (learning && i == row)
        {
            rowField->setText(
                KeyCodeHelper::getAsciiCompatibleDisplayName(learnCandidate));
            rowField->setBlinking(true);
        }
        else
        {
            rowField->setBlinking(false);
        }
    }

    displayUpAndDown();
}

void VmpcKeyboardScreen::displayUpAndDown()
{
    const auto bindingCount = bindings->getBindingCount();
    findChild<Label>("up")->Hide(rowOffset == 0);
    findChild<Label>("down")->Hide(rowOffset + 5 >= bindingCount);
}
