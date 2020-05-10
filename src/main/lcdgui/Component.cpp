#include "Component.hpp"

#include "Label.hpp"
#include "Field.hpp"
#include "Parameter.hpp"

#include <Mpc.hpp>

#include <string>

using namespace std;
using namespace mpc::lcdgui;

Component::Component(const string& name)
{
	this->name = name;
}

weak_ptr<Parameter> Component::findParameter(const string& name)
{
	for (auto& c : children)
	{
		auto candidate = dynamic_pointer_cast<Parameter>(c);

		if (candidate && candidate->getName().compare(name) == 0)
		{
			return candidate;
		}

		auto secondCandidate = c->findParameter(name).lock();

		if (secondCandidate)
		{
			return secondCandidate;
		}
	}

	return {};
}

weak_ptr<Label> Component::findLabel(const string& name)
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

weak_ptr<Field> Component::findField(const string& name)
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

vector<weak_ptr<Label>> Component::findLabels()
{
	vector<weak_ptr<Label>> result;
	for (auto& c : children)
	{
		auto candidate = dynamic_pointer_cast<Label>(c);
		if (candidate)
		{
			result.push_back(candidate);
		}

		for (auto& Label : c->findLabels())
		{
			result.push_back(Label);
		}
	}
	return result;
}

vector<weak_ptr<Field>> Component::findFields()
{
	vector<weak_ptr<Field>> result;
	for (auto& c : children)
	{
		auto candidate = dynamic_pointer_cast<Field>(c);
		if (candidate)
		{
			result.push_back(candidate);
		}

		for (auto& field : c->findFields())
		{
			result.push_back(field);
		}
	}
	return result;
}

vector<weak_ptr<Parameter>> Component::findParameters()
{
	vector<weak_ptr<Parameter>> result;
	for (auto& c : children)
	{
		for (auto& childParameter : c->findParameters())
		{
			auto candidate = dynamic_pointer_cast<Parameter>(childParameter.lock());
			if (candidate)
			{
				result.push_back(candidate);
			}
		}
	}
	return result;
}

weak_ptr<Component> Component::addChild(shared_ptr<Component> child)
{
	children.push_back(move(child));
	return children.back();
}

void Component::addChildren(vector<shared_ptr<Component>> children)
{
	for (auto& c : children)
	{
		this->children.push_back(move(c));
	}
}

weak_ptr<Component> Component::findChild(const string& name)
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

void Component::Draw(vector<vector<bool>>* pixels)
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

void Component::Clear(vector<vector<bool>>* pixels) {
	auto r = getRect();
	for (int i = r.L; i < r.R + 1; i++) {
		for (int j = r.T; j < r.B + 1; j++) {
			if (i < 0 || i > 247 || j < 0 || j > 59) continue;
			(*pixels)[i][j] = false;
		}
	}
}
