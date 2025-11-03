#include "Component.hpp"

#include "Label.hpp"
#include "Field.hpp"
#include "Parameter.hpp"
#include "ScreenComponent.hpp"
#include "lcdgui/EventRow.hpp"
#include "lcdgui/FunctionKeys.hpp"
#include "lcdgui/Knob.hpp"
#include "lcdgui/PunchRect.hpp"
#include "lcdgui/Rectangle.hpp"
#include "lcdgui/Underline.hpp"

#include <string>

using namespace mpc::lcdgui;

Component::Component(const std::string &nameToUse) : name(nameToUse) {}

void Component::addReactiveBinding(ReactiveBinding b)
{
    reactiveBindings.push_back(std::move(b));
}

void Component::sendToBack(std::shared_ptr<Component> childToSendBack)
{
    for (int i = 0; i < children.size(); i++)
    {
        if (children[i] == childToSendBack)
        {
            auto placeHolder = children[i];
            children.erase(begin(children) + i);
            children.insert(children.begin(), std::move(placeHolder));
            break;
        }
    }
}

bool Component::bringToFront(Component *childToBringToFront)
{
    if (!childToBringToFront)
    {
        return false;
    }

    for (int i = 0; i < children.size(); ++i)
    {
        if (children[i].get() == childToBringToFront)
        {
            auto ptr = std::move(children[i]);
            children.erase(children.begin() + i);
            children.push_back(std::move(ptr));
            return true;
        }
    }

    for (auto &c : children)
    {
        if (c->bringToFront(childToBringToFront))
        {
            auto it = std::find_if(children.begin(), children.end(),
                                   [&](const auto &ch)
                                   {
                                       return ch.get() == c.get();
                                   });

            if (it != children.end())
            {
                auto ptr = std::move(*it);
                children.erase(it);
                children.push_back(std::move(ptr));
            }
            return true;
        }
    }

    return false;
}

bool Component::shouldNotDraw(std::vector<std::vector<bool>> *pixels)
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

std::shared_ptr<Parameter>
Component::findParameter(const std::string &nameOfParameterToFind)
{
    for (auto &c : children)
    {
        auto candidate = std::dynamic_pointer_cast<Parameter>(c);

        if (candidate && candidate->getName() == nameOfParameterToFind)
        {
            return candidate;
        }

        auto secondCandidate = c->findParameter(nameOfParameterToFind);

        if (secondCandidate)
        {
            return secondCandidate;
        }
    }

    return {};
}

std::shared_ptr<Label>
Component::findLabel(const std::string &nameOfLabelToFind)
{
    for (auto &c : children)
    {
        auto candidate = std::dynamic_pointer_cast<Label>(c);

        if (candidate && candidate->getName() == nameOfLabelToFind)
        {
            return candidate;
        }

        auto secondCandidate = c->findLabel(nameOfLabelToFind);

        if (secondCandidate)
        {
            return secondCandidate;
        }
    }

    return {};
}

std::shared_ptr<Field>
Component::findField(const std::string &nameOfFieldToFind)
{
    for (auto &c : children)
    {
        auto candidate = std::dynamic_pointer_cast<Field>(c);

        if (candidate && candidate->getName() == nameOfFieldToFind)
        {
            return candidate;
        }

        auto secondCandidate = c->findField(nameOfFieldToFind);

        if (secondCandidate)
        {
            return secondCandidate;
        }
    }

    return {};
}

std::vector<std::shared_ptr<Label>> Component::findLabels()
{
    std::vector<std::shared_ptr<Label>> result;

    for (auto &c : children)
    {
        auto candidate = std::dynamic_pointer_cast<Label>(c);
        if (candidate)
        {
            result.push_back(candidate);
        }

        for (auto &Label : c->findLabels())
        {
            result.push_back(Label);
        }
    }

    return result;
}

std::vector<std::shared_ptr<Field>> Component::findFields()
{
    std::vector<std::shared_ptr<Field>> result;

    for (auto &c : children)
    {
        auto candidate = std::dynamic_pointer_cast<Field>(c);

        if (candidate)
        {
            result.push_back(candidate);
        }

        for (auto &field : c->findFields())
        {
            result.push_back(field);
        }
    }

    return result;
}

std::vector<std::shared_ptr<Parameter>> Component::findParameters()
{
    std::vector<std::shared_ptr<Parameter>> result;

    for (auto &c : children)
    {
        if (std::dynamic_pointer_cast<Parameter>(c))
        {
            result.push_back(std::dynamic_pointer_cast<Parameter>(c));
        }

        for (auto &parameter : c->findParameters())
        {
            result.push_back(parameter);
        }
    }

    return result;
}

std::shared_ptr<Component> Component::addChild(std::shared_ptr<Component> child)
{
    if (dynamic_cast<ScreenComponent *>(this))
    {
        auto background = findBackground();

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

void Component::removeChild(std::shared_ptr<Component> child)
{
    if (!child)
    {
        return;
    }

    for (auto &c : children)
    {
        if (c == child)
        {
            children.erase(find(begin(children), end(children), child));
            return;
        }
    }

    for (auto &c : children)
    {
        c->removeChild(child);
    }
}

void Component::addChildren(
    std::vector<std::shared_ptr<Component>> &childrenToAdd)
{
    for (auto &c : childrenToAdd)
    {
        addChild(c);
    }
}

std::shared_ptr<Component>
Component::findChild(const std::string &nameOfChildToFind)
{
    for (auto &c : children)
    {
        if (c->getName() == nameOfChildToFind)
        {
            return c;
        }

        auto candidate = c->findChild(nameOfChildToFind);

        if (candidate)
        {
            return candidate;
        }
    }
    return {};
}

void Component::drawRecursive(std::vector<std::vector<bool>> *pixels)
{
    if (shouldNotDraw(pixels))
    {
        return;
    }

    //	MLOG("Drawing " + name);

    if (hidden || !IsDirty())
    {
        return;
    }

    Draw(pixels);

    for (auto &c : children)
    {
        c->drawRecursive(pixels);
    }

    dirty = false;
}

const std::string &Component::getName()
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

    for (auto &c : children)
    {
        c->Hide(b);
    }
}

void Component::setSize(int newW, int newH)
{
    if (newW == w && newH == h)
    {
        return;
    }

    if (!(w == -1 && h == -1))
    {
        auto rect = getRect();
        preDrawClearRect = preDrawClearRect.Union(&rect);
    }

    w = newW;
    h = newH;
    SetDirty();
}

void Component::setLocation(int newX, int newY)
{
    if (newX == x && newY == y)
    {
        return;
    }

    if (!(x == -1 && y == -1))
    {
        auto rect = getRect();
        preDrawClearRect = preDrawClearRect.Union(&rect);
    }

    x = newX;
    y = newY;
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
        {
            res = res.Union(&preDrawClearRect);
        }
    }

    return res;
}

void Component::SetDirty(bool b)
{
    if (hidden)
    {
        return;
    }

    for (auto &c : children)
    {
        c->SetDirty(b);
    }

    dirty = b;
}

bool Component::IsHidden()
{
    return hidden;
}

bool Component::IsDirty()
{
    auto dirtyChild = false;

    for (auto &c : children)
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

    // if (dirty)
    // MLOG(name + " is dirty");

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

void Component::Clear(std::vector<std::vector<bool>> *pixels)
{
    auto r = getRect();

    for (int i = r.L; i < r.R; i++)
    {
        if (i < 0)
        {
            continue;
        }

        for (int j = r.T; j < r.B; j++)
        {
            (*pixels)[i][j] = false;
        }
    }
}

void Component::preDrawClear(std::vector<std::vector<bool>> *pixels)
{
    auto r = preDrawClearRect;

    for (auto &c : children)
    {
        c->preDrawClear(pixels);
    }

    if (r.Empty())
    {
        return;
    }

    for (int i = r.L; i < r.R; i++)
    {
        if (i < 0)
        {
            continue;
        }

        for (int j = r.T; j < r.B; j++)
        {
            (*pixels)[i][j] = false;
        }
    }

    preDrawClearRect.Clear();
}

std::vector<std::shared_ptr<Component>> Component::findHiddenChildren()
{
    std::vector<std::shared_ptr<Component>> result;

    for (auto &c : children)
    {
        if (c->IsHidden() && c->IsDirty())
        {
            result.push_back(c);
        }

        for (auto &c1 : c->findHiddenChildren())
        {
            result.push_back(c1);
        }
    }

    return result;
}

std::shared_ptr<Background> Component::findBackground()
{
    return findChild<Background>("");
}

void Component::deleteChildren(const std::string &nameOfChildrenToDelete)
{
    for (int i = children.size() - 1; i >= 0; i--)
    {
        if (children[i]->getName() == nameOfChildrenToDelete)
        {
            children.erase(begin(children) + i);
        }
    }
}

Component *Component::getParent()
{
    return parent;
}

template <class T>
std::shared_ptr<T> Component::findChild(const std::string &nameOfChildToFind)
{
    for (auto &c : children)
    {
        auto asT = std::dynamic_pointer_cast<T>(c);
        if (asT)
        {
            if (nameOfChildToFind.empty() || c->getName() == nameOfChildToFind)
            {
                return asT;
            }
        }

        if (auto candidate = c->findChild<T>(nameOfChildToFind))
        {
            return candidate;
        }
    }

    return {};
}

#define INSTANTIATE_FINDCHILD(T)                                               \
    template std::shared_ptr<T> Component::findChild<T>(const std::string &);

// Explicit instantiations for all used types
INSTANTIATE_FINDCHILD(mpc::lcdgui::Background)
INSTANTIATE_FINDCHILD(mpc::lcdgui::Rectangle)
INSTANTIATE_FINDCHILD(mpc::lcdgui::TextComp)
INSTANTIATE_FINDCHILD(mpc::lcdgui::FunctionKey)
INSTANTIATE_FINDCHILD(mpc::lcdgui::FunctionKeys)
INSTANTIATE_FINDCHILD(mpc::lcdgui::ScreenComponent)
INSTANTIATE_FINDCHILD(mpc::lcdgui::Wave)
INSTANTIATE_FINDCHILD(mpc::lcdgui::EnvGraph)
INSTANTIATE_FINDCHILD(mpc::lcdgui::Field)
INSTANTIATE_FINDCHILD(mpc::lcdgui::Label)
INSTANTIATE_FINDCHILD(mpc::lcdgui::Knob)
INSTANTIATE_FINDCHILD(mpc::lcdgui::PunchRect)
INSTANTIATE_FINDCHILD(mpc::lcdgui::EventRow)
INSTANTIATE_FINDCHILD(mpc::lcdgui::Underline)
