#pragma once
#include "Label.hpp"

#include "Component.hpp"

#include <memory>
#include <string>

namespace mpc
{
    class Mpc;
}

namespace mpc::lcdgui
{

    class Info
        : public Component
    {

    public:
        Info(mpc::Mpc &mpc, const std::string &name, int x, int y, int size);
    };
} // namespace mpc::lcdgui
