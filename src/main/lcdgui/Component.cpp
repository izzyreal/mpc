#include "Component.hpp"

#include "Label.hpp"
#include "Field.hpp"
#include "Parameter.hpp"
#include "ScreenComponent.hpp"

#include <string>

#include <cmath>

using namespace mpc::lcdgui;

Component::Component(const std::string& name)
{
	this->name = name;
}

void Component::sendToBack(std::weak_ptr<Component> childToSendBack)
{
	for (int i = 0; i < children.size(); i++)
	{
		if (children[i] == childToSendBack.lock())
		{
			auto placeHolder = children[i];
			children.erase(begin(children) + i);
			children.insert(children.begin(), std::move(placeHolder));
			break;
		}
	}
}

bool Component::bringToFront(Component* childToBringToFront)
{	
	if (childToBringToFront == nullptr)
	{
		return false;
	}

	for (int i = 0; i < children.size(); i++)
	{
		if (children[i].get() == childToBringToFront)
		{
			auto placeHolder = children[i];
			children.erase(begin(children) + i);
			children.push_back(std::move(placeHolder));

			if (parent != nullptr)
				parent->bringToFront(this);

			return true;
		}
	}

	for (auto& c : children)
	{
		if (c->bringToFront(childToBringToFront))
			return true;
	}

	return false;
}

bool Component::shouldNotDraw(std::vector<std::vector<bool>>* pixels)
{
	if (!IsDirty())
	{
		return true;
	}

	if (hidden)
	{
		Clear(pixels);
		dirty = false;
		return true;
	}

	return false;
}

std::weak_ptr<Parameter> Component::findParameter(const std::string& name)
{
	for (auto& c : children)
	{
		auto candidate = std::dynamic_pointer_cast<Parameter>(c);

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



std::weak_ptr<Label> Component::findLabel(const std::string& name)
{
	for (auto& c : children)
	{
		auto candidate = std::dynamic_pointer_cast<Label>(c);

		if (candidate && candidate->getName().compare(name) == 0)
			return candidate;

		auto secondCandidate = c->findLabel(name).lock();

		if (secondCandidate)
			return secondCandidate;
	}

	return {};
}

std::weak_ptr<Field> Component::findField(const std::string& name)
{
	for (auto& c : children)
	{
		auto candidate = std::dynamic_pointer_cast<Field>(c);

		if (candidate && candidate->getName().compare(name) == 0)
			return candidate;

		auto secondCandidate = c->findField(name).lock();

		if (secondCandidate)
			return secondCandidate;
	}

	return {};
}

std::vector<std::weak_ptr<Label>> Component::findLabels()
{
    std::vector<std::weak_ptr<Label>> result;

	for (auto& c : children)
	{
		auto candidate = std::dynamic_pointer_cast<Label>(c);
		if (candidate)
			result.push_back(candidate);

		for (auto& Label : c->findLabels())
			result.push_back(Label);
	}

	return result;
}

std::vector<std::weak_ptr<Field>> Component::findFields()
{
    std::vector<std::weak_ptr<Field>> result;

	for (auto& c : children)
	{
		auto candidate = std::dynamic_pointer_cast<Field>(c);

		if (candidate)
			result.push_back(candidate);

		for (auto& field : c->findFields())
			result.push_back(field);
	}
	
	return result;
}

std::vector<std::weak_ptr<Parameter>> Component::findParameters()
{
    std::vector<std::weak_ptr<Parameter>> result;
	
	for (auto& c : children)
	{
		if (dynamic_pointer_cast<Parameter>(c))
			result.push_back(dynamic_pointer_cast<Parameter>(c));
		
		for (auto& parameter : c->findParameters())
			result.push_back(parameter);
	}

	return result;
}

std::weak_ptr<Component> Component::addChild(std::shared_ptr<Component> child)
{
    if (dynamic_cast<ScreenComponent*>(this))
    {
        auto background = findBackground().lock();

        if (background)
        {
            return background->addChild(child);
        }
    }

	child->parent = this;
	children.push_back(std::move(child));
	SetDirty();
	return children.back();
}

void Component::removeChild(std::weak_ptr<Component> child)
{
	if (!child.lock())
		return;

	for (auto& c : children)
	{
		if (c == child.lock())
		{
			children.erase(find(begin(children), end(children), child.lock()));
			return;
		}
	}

	for (auto& c : children)
		c->removeChild(child);
}

void Component::addChildren(std::vector<std::shared_ptr<Component>> children)
{
	for (auto& c : children)
		addChild(c);
}

std::weak_ptr<Component> Component::findChild(const std::string& name)
{
	for (auto& c : children)
	{
		if (c->getName().compare(name) == 0)
			return c;

		auto candidate = c->findChild(name).lock();

		if (candidate)
			return candidate;
	}
	return {};
}

void Component::Draw(std::vector<std::vector<bool>>* pixels)
{
	if (shouldNotDraw(pixels))
		return;

	//MLOG("Drawing " + name);

	if (hidden || !IsDirty())
		return;

	for (auto& c : children)
		c->Draw(pixels);

	dirty = false;
}

const std::string& Component::getName()
{
	return name;
}

void Component::Hide(bool b) 
{ 
	if (hidden != b)
	{
		hidden = b;
		dirty = true;
	}

	for (auto& c : children)
		c->Hide(b);
}

void Component::setSize(int w, int h)
{
	if (w == this->w && h == this->h)
		return;

	if (!(this->w == -1 && this->h == -1))
	{
		auto rect = getRect();
		preDrawClearRect = preDrawClearRect.Union(&rect);
	}

	this->w = w;
	this->h = h;
	SetDirty();
}

void Component::setLocation(int x, int y)
{
	if (x == this->x && y == this->y)
		return;

	if (!(this->x == -1 && this->y == -1))
	{
		auto rect = getRect();
		preDrawClearRect = preDrawClearRect.Union(&rect);
	}

	this->x = x;
	this->y = y;
	SetDirty();
}

MRECT Component::getDirtyArea()
{
	MRECT res;

	for (auto c : children)
	{
		auto rect = c->getDirtyArea();
		res = res.Union(&rect);
	}

	if (dirty)
	{
		auto rect = getRect();
		res = res.Union(&rect);

		if (!preDrawClearRect.Empty())
			res = res.Union(&preDrawClearRect);
	}

	return res;
}

void Component::SetDirty(bool b) 
{ 
	if (hidden)
		return;

	for (auto& c : children)
		c->SetDirty(b);
	
	dirty = b;
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
		return true;

	//if (dirty)
		//MLOG(name + " is dirty");

	return dirty;
}

MRECT Component::getRect()
{
	auto x1 = std::max(0, x);
	auto x2 = std::min(248, x + w);
	auto y1 = std::max(0, y);
	auto y2 = std::min(60, y + h);
	return MRECT(x1, y1, x2, y2);
}

void Component::Clear(std::vector<std::vector<bool>>* pixels)
{
	auto r = getRect();

	for (int i = r.L; i < r.R; i++)
	{
		if (i < 0)
			continue;

		for (int j = r.T; j < r.B; j++)
			(*pixels)[i][j] = false;
	}
}

void Component::preDrawClear(std::vector<std::vector<bool>>* pixels)
{
	auto r = preDrawClearRect;

	for (auto& c : children)
		c->preDrawClear(pixels);

	if (r.Empty())
		return;

	for (int i = r.L; i < r.R; i++)
	{
		if (i < 0)
			continue;

		for (int j = r.T; j < r.B; j++)
			(*pixels)[i][j] = false;
	}

	preDrawClearRect.Clear();
}

std::vector<std::weak_ptr<Component>> Component::findHiddenChildren()
{
    std::vector<std::weak_ptr<Component>> result;

	for (auto& c : children)
	{
		if (c->IsHidden() && c->IsDirty())
			result.push_back(c);

		for (auto& c1 : c->findHiddenChildren())
			result.push_back(c1);
	}

	return result;
}

std::weak_ptr<ScreenComponent> Component::findScreenComponent()
{
	for (auto& c : children)
	{
		auto candidate = std::dynamic_pointer_cast<ScreenComponent>(c);
		
		if (candidate)
			return candidate;

		auto childCandidate = c->findScreenComponent().lock();

		if (childCandidate)
			return childCandidate;
	}
	
	return {};
}

std::weak_ptr<Background> Component::findBackground()
{
    return findChild<Background>("");
}

void Component::deleteChildren(const std::string& name)
{
	for (int i = children.size() - 1; i >= 0; i--)
	{
		if (children[i]->getName().compare(name) == 0)
			children.erase(begin(children) + i);
	}
}

Component* Component::getParent()
{
	return parent;
}
