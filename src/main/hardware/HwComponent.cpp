#include <hardware/HwComponent.hpp>

#include "PadAndButtonKeyboard.hpp"
#include "Hardware.hpp"

#include <Mpc.hpp>

using namespace mpc::hardware;

HwComponent::HwComponent(mpc::Mpc& mpc, const std::string& label)
        : mpc(mpc), label(label)
{
}

std::string HwComponent::getLabel()
{
    return label;
}

void HwComponent::push()
{
    mpc.getHardware()->getPadAndButtonKeyboard()->pressHardwareComponent(label);
}

void HwComponent::push(int /* velocity */)
{
    mpc.getHardware()->getPadAndButtonKeyboard()->pressHardwareComponent(label);
}