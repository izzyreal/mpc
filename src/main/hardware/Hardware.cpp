#include "hardware/Hardware.hpp"

using namespace mpc::hardware;

Hardware::Hardware()
{
    for (int i = CURSOR_LEFT_OR_DIGIT; i <= NUM_9_OR_MIDI_SYNC; i++)
    {
        const auto id = static_cast<ComponentId>(i);
        buttons[id] = std::make_shared<Button>(id);
        components[id] = buttons[id];
    }

    for (uint8_t i = 0; i < 16; ++i)
    {
        pads.push_back(std::make_shared<Pad>(PhysicalPadIndex(i)));
        components[pads.back()->getId()] = pads.back();
    }

    for (int i = FULL_LEVEL_OR_CASE_SWITCH_LED; i <= PLAY_LED; i++)
    {
        const auto id = static_cast<ComponentId>(i);
        leds[id] = std::make_shared<Led>(id);
        components[id] = leds[id];
        if (id == BANK_A_LED)
        {
            leds[id]->setEnabled(true);
        }
    }

    dataWheel = std::make_shared<DataWheel>();
    components[dataWheel->getId()] = dataWheel;

    recPot = std::make_shared<Pot>(REC_GAIN_POT);
    components[recPot->getId()] = recPot;

    volPot = std::make_shared<Pot>(MAIN_VOLUME_POT);
    components[volPot->getId()] = volPot;

    slider = std::make_shared<Slider>();
    components[slider->getId()] = slider;
}

std::vector<std::string> Hardware::getButtonLabels() const
{
    std::vector<std::string> result;

    for (auto &[label, id] : componentLabelToId)
    {
        if (id >= CURSOR_LEFT_OR_DIGIT && id <= NUM_9_OR_MIDI_SYNC)
        {
            result.push_back(label);
        }
    }

    return result;
}

std::shared_ptr<Pad> Hardware::getPad(const int index)
{
    if (index < 0 || index >= static_cast<int>(pads.size()))
    {
        return {};
    }
    return pads[static_cast<size_t>(index)];
}

std::vector<std::shared_ptr<Pad>> &Hardware::getPads()
{
    return pads;
}

std::shared_ptr<Button> Hardware::getButton(const ComponentId id)
{
    if (buttons.count(id) == 0)
    {
        return {};
    }
    return buttons[id];
}

std::shared_ptr<Led> Hardware::getLed(const ComponentId id)
{
    if (leds.count(id) == 0)
    {
        return {};
    }
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

std::shared_ptr<Component> Hardware::getComponent(const ComponentId id)
{
    if (components.count(id) == 0)
    {
        return {};
    }
    return components[id];
}
