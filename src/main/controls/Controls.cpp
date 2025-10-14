#include "Controls.hpp"

#include "KbMapping.hpp"

using namespace mpc::controls;

Controls::Controls(mpc::Mpc& _mpc) : 
	kbMapping (std::make_shared<KbMapping>(_mpc))
{
}

std::weak_ptr<KbMapping> Controls::getKbMapping()
{
    return kbMapping;
}

