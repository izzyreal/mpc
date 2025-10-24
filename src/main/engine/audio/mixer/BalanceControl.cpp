#include "BalanceControl.hpp"

#include "engine/control/LinearLaw.hpp"

using namespace mpc::engine::audio::mixer;

BalanceControl::BalanceControl()
    : LCRControl("Balance", mpc::engine::control::LinearLaw::UNITY(), 0.5f)
{
}

float BalanceControl::getLeft()
{
    return left;
}

float BalanceControl::getRight()
{
    return right;
}

void BalanceControl::setValue(float value)
{
    left = value < 0.5f ? 1.0f : 2 * (1 - value);
    right = value > 0.5f ? 1.0f : 2 * value;
    LCRControl::setValue(value);
}
