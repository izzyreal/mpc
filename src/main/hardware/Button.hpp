#pragma once

#include <Observer.hpp>

#include <hardware/HwComponent.hpp>

#include <string>
#include <memory>

namespace mpc {
class Mpc;
}

namespace mpc::controls {
class BaseControls;
}

namespace mpc::hardware {

class Button : public HwComponent, public Observable
{
    
private:
    std::weak_ptr<mpc::controls::BaseControls> controls;
    
public:
    void push(int) override;
    void release() override;
    
    Button(mpc::Mpc& mpc, const std::string& label);
    
};
}
