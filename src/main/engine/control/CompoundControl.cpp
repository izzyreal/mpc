#include "engine/control/CompoundControl.hpp"
#include "engine/control/Control.hpp"

using namespace mpc::engine::control;

CompoundControl::CompoundControl(int id, const std::string &name)
    : Control(id, name)
{
}

void CompoundControl::add(std::shared_ptr<Control> control)
{
    if (!control)
    {
        return;
    }
    std::string name = control->getName();
    control->setParent(this);
    controls.push_back(control);
}

void CompoundControl::remove(const std::shared_ptr<Control> &c)
{
    auto control = c;

    if (!control)
    {
        return;
    }

    for (int i = 0; i < controls.size(); i++)
    {
        auto currentControl = controls[i];

        if (currentControl == control)
        {
            controls.erase(begin(controls) + i);
            break;
        }
    }
}

std::vector<std::shared_ptr<Control>> CompoundControl::getControls()
{
    return controls;
}

std::shared_ptr<Control> CompoundControl::find(const std::string &name)
{
    for (auto &c : controls)
    {
        if (c->getName() == name)
        {
            return c;
        }
    }
    return {};
}

std::shared_ptr<Control> CompoundControl::deepFind(int controlId)
{
    for (auto &c : controls)
    {

        auto cc = std::dynamic_pointer_cast<CompoundControl>(c);

        if (cc)
        {
            auto c2 = cc->deepFind(controlId);
            if (c2)
            {
                return c2;
            }
        }
        else if (controlId == c->getId())
        {
            return c;
        }
    }
    return {};
}

void CompoundControl::disambiguate(const std::shared_ptr<CompoundControl> &c)
{
    auto original = c->getName();
    if (!find(original))
    {
        return;
    }
    int index = 1;
    std::string str;
    do
    {
        index++;
        str = original + " #" + (std::to_string(index));
    } while (find(str));
    c->setName(str);
}
