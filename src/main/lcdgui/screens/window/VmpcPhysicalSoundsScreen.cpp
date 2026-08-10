#include "VmpcPhysicalSoundsScreen.hpp"

#include "Mpc.hpp"
#include "engine/EngineHost.hpp"
#include "lcdgui/TextComp.hpp"

#include <array>
#include <string>
#include <utility>

using namespace mpc::engine;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;

namespace
{
    const std::array<std::pair<const char *, PhysicalSoundGroup>, 5>
        GroupFields{{
            {"buttons", PhysicalSoundGroup::Buttons},
            {"pads", PhysicalSoundGroup::Pads},
            {"slider", PhysicalSoundGroup::Slider},
            {"data-wheel", PhysicalSoundGroup::DataWheel},
            {"power", PhysicalSoundGroup::Power},
        }};
}

VmpcPhysicalSoundsScreen::VmpcPhysicalSoundsScreen(Mpc &mpc,
                                                   const int layerIndex)
    : ScreenComponent(mpc, "vmpc-physical-sounds", layerIndex)
{
    const auto title = std::make_shared<TextComp>(mpc, "title");
    title->setLocation(62, 0);
    title->setSize(128, 9);
    addChild(title);
}

void VmpcPhysicalSoundsScreen::open()
{
    const auto title = findChild<TextComp>("title");
    title->setText("Physical sound levels");
    displayLevels();
}

void VmpcPhysicalSoundsScreen::turnWheel(const int increment)
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();
    const auto engineHost = mpc.getEngineHost();
    for (const auto &[fieldName, group] : GroupFields)
    {
        if (focusedFieldName == fieldName)
        {
            engineHost->setPhysicalSoundGroupLevel(
                group,
                engineHost->getPhysicalSoundGroupLevel(group) + increment);
            displayLevels();
            return;
        }
    }
}

void VmpcPhysicalSoundsScreen::displayLevels() const
{
    const auto engineHost = mpc.getEngineHost();
    for (const auto &[fieldName, group] : GroupFields)
    {
        findField(fieldName)->setTextPadded(
            engineHost->getPhysicalSoundGroupLevel(group), " ");
    }
}
