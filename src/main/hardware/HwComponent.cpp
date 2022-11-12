#include <hardware/HwComponent.hpp>

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
