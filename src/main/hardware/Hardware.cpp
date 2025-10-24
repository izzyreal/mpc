#include "hardware/Hardware.hpp"

using namespace mpc::hardware;

Hardware::Hardware()
{
    for (int i = ComponentId::CURSOR_LEFT_OR_DIGIT; i <= ComponentId::NUM_9_OR_MIDI_SYNC; i++)
    {
        const auto id = static_cast<ComponentId>(i);
        buttons[id] = std::make_shared<Button>(id);
        components[id] = buttons[id];
    }

    for (int i = 0; i < 16; ++i)
    {
        pads.push_back(std::make_shared<Pad>(i));
        components[pads.back()->getId()] = pads.back();
    }

    for (int i = ComponentId::FULL_LEVEL_OR_CASE_SWITCH_LED; i <= ComponentId::PLAY_LED; i++)
    {
        const auto id = static_cast<ComponentId>(i);
        leds[id] = std::make_shared<Led>(id);
        components[id] = leds[id];
        if (id == ComponentId::BANK_A_LED) leds[id]->setEnabled(true);
    }

    dataWheel = std::make_shared<DataWheel>();
    components[dataWheel->getId()] = dataWheel;

    recPot = std::make_shared<Pot>(ComponentId::REC_GAIN_POT);
    components[recPot->getId()] = recPot;

    volPot = std::make_shared<Pot>(ComponentId::MAIN_VOLUME_POT);
    components[volPot->getId()] = volPot;

    slider = std::make_shared<Slider>();
    components[slider->getId()] = slider;
}

std::vector<std::string> Hardware::getButtonLabels()
{
    std::vector<std::string> result;

    for (auto &[label, id] : componentLabelToId)
    {
        if (id >= ComponentId::CURSOR_LEFT_OR_DIGIT && id <= ComponentId::NUM_9_OR_MIDI_SYNC)
        {
            result.push_back(label);
        }
    }

    return result;
}

std::shared_ptr<Pad> Hardware::getPad(int index)
{
    if (index < 0 || index >= static_cast<int>(pads.size())) return {};
    return pads[static_cast<size_t>(index)];
}

std::vector<std::shared_ptr<Pad>>& Hardware::getPads()
{
    return pads;
}

std::shared_ptr<Button> Hardware::getButton(ComponentId id)
{
    if (buttons.count(id) == 0) return {};
    return buttons[id];
}

std::shared_ptr<Led> Hardware::getLed(ComponentId id)
{
    if (leds.count(id) == 0) return {};
    return leds[id];
}

std::shared_ptr<DataWheel> Hardware::getDataWheel()
{
    return dataWheel;
}

std::shared_ptr<Pot> Hardware::getRecPot()
{
    return recPot;
}

std::shared_ptr<Pot> Hardware::getVolPot()
{
    return volPot;
}

std::shared_ptr<Slider> Hardware::getSlider()
{
    return slider;
}

std::shared_ptr<Component> Hardware::getComponent(ComponentId id)
{
    if (components.count(id) == 0) return {};
    return components[id];
}
