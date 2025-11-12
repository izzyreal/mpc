#pragma once

#include "BasicStructs.hpp"

#include "lcdgui/ReactiveBinding.hpp"

#include <vector>
#include <string>
#include <memory>

namespace mpc::lcdgui
{
    class Label;
    class Field;
    class Parameter;
    class Background;
} // namespace mpc::lcdgui

namespace mpc::lcdgui
{
    class Component
    {
        const std::string name;
        std::vector<ReactiveBinding> reactiveBindings;

    protected:
        Component *parent = nullptr;
        std::vector<std::shared_ptr<Component>> children;

        bool hidden = false;
        bool dirty = false;
        int x = -1;
        int y = -1;
        int w = -1;
        int h = -1;

        bool shouldNotDraw(std::vector<std::vector<bool>> *pixels);

        void addReactiveBinding(ReactiveBinding);

    public:
        virtual ~Component() = default;
        void timerCallback()
        {
            for (auto &b : reactiveBindings)
            {
                b.refresh();
            }

            onTimerCallback();

            for (const auto &c : children)
            {
                c->onTimerCallback();
            }
        }

        virtual void onTimerCallback() {}

        void printTree(int depth = 0) const
        {
            std::string space;
            for (int i = 0; i < depth * 2; i++)
            {
                space.append(" ");
            }
            printf("%s%s\n", space.c_str(), name.c_str());
            for (const auto &c : children)
            {
                c->printTree(depth + 1);
            }
        }

        template <class T>
        std::shared_ptr<T> findChild(const std::string &nameOfChildToFind = "");

        std::shared_ptr<Component> addChild(std::shared_ptr<Component> child);

        template <typename T, typename... Args>
        std::shared_ptr<T> addChildT(Args &...args)
        {
            auto child = std::make_shared<T>(args...);
            addChild(child);
            return child;
        }

        void removeChild(const std::shared_ptr<Component> &child);
        void addChildren(std::vector<std::shared_ptr<Component>> &children);
        std::shared_ptr<Component>
        findChild(const std::string &nameOfChildToFind);
        std::shared_ptr<Label> findLabel(const std::string &name) const;
        std::shared_ptr<Field> findField(const std::string &name) const;
        std::shared_ptr<Parameter> findParameter(const std::string &name) const;
        std::vector<std::shared_ptr<Label>> findLabels() const;
        std::vector<std::shared_ptr<Field>> findFields() const;
        std::vector<std::shared_ptr<Parameter>> findParameters() const;
        std::shared_ptr<Background> findBackground();
        MRECT getRect() const;
        std::vector<std::shared_ptr<Component>> findHiddenChildren() const;
        void deleteChildren(const std::string &nameOfChildrenToDelete);

        virtual void Hide(bool b);
        virtual void SetDirty(bool b = true);
        bool IsHidden() const;
        bool IsDirty() const;
        MRECT getDirtyArea();
        virtual void setSize(int newW, int newH);
        void setLocation(int newX, int newY);
        const std::string &getName() const;
        void sendToBack(const std::shared_ptr<Component> &childToSendBack);
        bool bringToFront(Component *childToBringToFront);
        Component *getParent() const;

        virtual void Draw(std::vector<std::vector<bool>> * /*pixels*/) {}
        void drawRecursive(std::vector<std::vector<bool>> *pixels);
        virtual void Clear(std::vector<std::vector<bool>> *pixels);
        void preDrawClear(std::vector<std::vector<bool>> *pixels);

        MRECT preDrawClearRect;
        Component(const std::string &name);
    };
} // namespace mpc::lcdgui
