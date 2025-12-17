#include "engine/control/BooleanControl.hpp"

using namespace mpc::engine::control;
using namespace std;

BooleanControl::BooleanControl(const int id, const string &name)
    : Control(id, name)
{
}

void BooleanControl::setValue(const bool valueToUse)
{
    if (valueToUse != this->value)
    {
        this->value.store(valueToUse, std::memory_order_relaxed);
        notifyParent(this);
    }
}

bool BooleanControl::getValue() const
{
    return value.load(std::memory_order_relaxed);
}
