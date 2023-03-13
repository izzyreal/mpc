#include "BalanceControl.hpp"

#include <engine/control/LinearLaw.hpp>

using namespace ctoot::audio::mixer;

BalanceControl::BalanceControl() 
	: LCRControl("Balance", ctoot::control::LinearLaw::UNITY(), 0.01f, 0.5f)
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

BalanceControl::~BalanceControl() {
}

