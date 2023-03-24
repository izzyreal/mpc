#pragma once

#include <vector>
#include <memory>

#include <engine/control/Control.hpp>

namespace mpc::engine::control {
    class CompoundControl : public Control
    {
    protected:
        std::vector<std::shared_ptr<Control>> controls;

    public:
        virtual void add(std::shared_ptr<Control> control);

        void remove(std::shared_ptr<Control> control);

        std::vector<std::shared_ptr<Control>> getControls();

        std::shared_ptr<Control> find(std::string name);

        std::shared_ptr<Control> deepFind(int controlId);

        void disambiguate(std::shared_ptr<CompoundControl> c);

    public:
        CompoundControl(int id, std::string name);
    };
}
