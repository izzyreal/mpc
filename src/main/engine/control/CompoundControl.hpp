#pragma once

#include <vector>
#include <memory>

#include <engine/control/Control.hpp>

namespace mpc::engine::control {
    class CompoundControl : public Control
    {
    protected:
        std::vector<std::shared_ptr<Control>> controls;
        std::vector<std::shared_ptr<Control>> weakControls;

    public:
        std::vector<std::string> getControlNamesRecursive(int generation = 0);

    public:

        virtual void add(std::shared_ptr<Control> control);

        virtual void remove(std::shared_ptr<Control> control);

    public:
        virtual std::vector<std::shared_ptr<Control>> getControls();

        virtual std::shared_ptr<Control> find(std::string name);

        virtual std::shared_ptr<Control> deepFind(int controlId);

    public:
        virtual void disambiguate(std::shared_ptr<CompoundControl> c);

    public:
        CompoundControl(int id, std::string name);
    };
}
