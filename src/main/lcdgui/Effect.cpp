#include "Effect.hpp"

#include <Mpc.hpp>

using namespace mpc::lcdgui;

Effect::Effect(MRECT rect)
	: Component("effect")
{
	setSize(rect.W(), rect.H());
	setLocation(rect.L, rect.T);
}

void Effect::setFilled(bool b)
{
	this->filled = b;
	SetDirty();
}

void Effect::Draw(std::vector<std::vector<bool>>* pixels) {
	if (shouldNotDraw(pixels))
	{
		return;
	}

	auto rect = getRect();
	int x = rect.L;
	int y = rect.T;

	for (int i = x + 1; i < x + rect.W() - 1; i++)
		(*pixels)[i][y] = true;

	for (int i = x + 1; i < x + rect.W(); i++)
		(*pixels)[i][y + rect.H() - 1] = true;

	for (int i = x + 2; i < x + rect.W() - 1; i++)
		(*pixels)[i][y + rect.H()] = true;

	for (int i = y + 1; i < y + rect.H() - 1; i++)
		(*pixels)[x][i] = true;

	for (int i = y + 1; i < y + rect.H(); i++)
		(*pixels)[x + rect.W() - 1][i] = true;

	for (int i = y + 2; i < y + rect.H() - 1; i++)
		(*pixels)[x + rect.W()][i] = true;

	for (int i = x + 1; i < x + rect.W() - 1; i++) {
		for (int j = y + 1; j < y + rect.H() - 1; j++) {
			(*pixels)[i][j] = filled;
		}
	}

	dirty = false;
}
