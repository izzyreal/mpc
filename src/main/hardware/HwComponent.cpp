#include <hardware/HwComponent.hpp>

#include <Mpc.hpp>

using namespace mpc::hardware;
using namespace std;

HwComponent::HwComponent(mpc::Mpc& mpc, const string& label)
	: mpc(mpc), label(label)
{
}

string HwComponent::getLabel()
{
	return label;
}
