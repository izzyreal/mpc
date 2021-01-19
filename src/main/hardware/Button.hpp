#pragma once

#include <observer/Observable.hpp>

#include <hardware/HwComponent.hpp>

#include <string>

namespace mpc {
class Mpc;
}

namespace mpc::controls {
class BaseControls;
}

namespace mpc::hardware {

class Button
: public mpc::hardware::HwComponent
, public moduru::observer::Observable
{
    
private:
    std::weak_ptr<mpc::controls::BaseControls> controls;
    
public:
    void push() override;
    void release() override;
    
    Button(mpc::Mpc& mpc, const std::string& label);
    
};
}
