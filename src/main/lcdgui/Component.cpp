#include "Component.hpp"

#include "Label.hpp"
#include "Field.hpp"

#include <Mpc.hpp>

#include <string>

using namespace std;
using namespace mpc::lcdgui;

Component::Component(const string& name)
{
	this->name = name;
}

std::weak_ptr<Label> Component::findLabel(const std::string& name)
{
	for (auto& c : children)
	{
		auto candidate = dynamic_pointer_cast<Label>(c);

		if (candidate && candidate->getName().compare(name) == 0)
		{
			return candidate;
		}

		auto secondCandidate = c->findLabel(name).lock();

		if (secondCandidate)
		{
			return secondCandidate;
		}
	}

	return {};
}

std::weak_ptr<Field> Component::findField(const std::string& name)
{
	for (auto& c : children)
	{
		auto candidate = dynamic_pointer_cast<Field>(c);

		if (candidate && candidate->getName().compare(name) == 0)
		{
			return candidate;
		}

		auto secondCandidate = c->findField(name).lock();

		if (secondCandidate)
		{
			return secondCandidate;
		}
	}

	return {};
}

weak_ptr<Component> Component::addChild(std::shared_ptr<Component> child)
{
	children.push_back(move(child));
	return children.back();
}

void Component::addChildren(std::vector<std::shared_ptr<Component>> children)
{
	for (auto& c : children)
	{
		this->children.push_back(std::move(c));
	}
}

std::weak_ptr<Component> Component::findChild(const std::string& name)
{
	for (auto& c : children)
	{
		if (c->getName().compare(name) == 0)
		{
			return c;
		}
	}
	return {};
}

void Component::Draw(std::vector<std::vector<bool>>* pixels)
{
	if (hidden || !IsDirty())
	{
		return;
	}

	for (auto& c : children)
	{
		c->Draw(pixels);
	}
}

const string& Component::getName()
{
	return name;
}

void Component::Hide(bool b) 
{ 
	if (hidden != b) { 
		hidden = b;
	} 
}

void Component::setSize(int w, int h) {
	this->w = w;
	this->h = h;
	SetDirty();
	if (248 < x + w)
	{
		MLOG("hey");
	}
}

void Component::setLocation(int x, int y) {
	this->x = x;
	this->y = y;
	SetDirty();
}

MRECT Component::getDirtyArea() {
	MRECT res;
	for (auto c : children)
	{
		res = res.Union(&c->getDirtyArea());
	}

	if (dirty && !hidden) {
		auto rect = getRect();
		res = res.Union(&rect);
	}

	return res;
}

void Component::SetDirty() 
{ 
	dirty = true; 
}

bool Component::IsHidden()
{ 
	return hidden; 
}

bool Component::IsDirty()
{ 
	auto dirtyChild = false;

	for (auto& c : children)
	{
		if (c->IsDirty())
		{
			dirtyChild = true;
			break;
		}

	}

	if (dirtyChild)
	{
		return true;
	}

	if (dirty && !hidden) {
			//MLOG(getName() + " is dirty");
	}
	return dirty && !hidden;
}

MRECT Component::getRect() {
	return MRECT(x, y, x + w, y + h);
}

void Component::Clear(std::vector<std::vector<bool>>* pixels) {
	auto r = getRect();
	for (int i = r.L; i < r.R + 1; i++) {
		for (int j = r.T; j < r.B + 1; j++) {
			if (i < 0 || i > 247 || j < 0 || j > 59) continue;
			(*pixels)[i][j] = false;
		}
	}
}
