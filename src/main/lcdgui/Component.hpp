#pragma once

#include <gui/BasicStructs.hpp>

#include <vector>
#include <string>
#include <memory>

namespace mpc::lcdgui
{
	class Label;
	class Field;
	class Parameter;
	class ScreenComponent;
    class Background;
}

namespace mpc::lcdgui
{
	class Component
	{

	protected:
		Component* parent = nullptr;
		std::vector<std::shared_ptr<Component>> children;

		std::string name = "";
		bool hidden = false;
		bool dirty = false;
		int x = -1;
		int y = -1;
		int w = -1;
		int h = -1;

		bool shouldNotDraw(std::vector<std::vector<bool>>* pixels);

	public:
		template<class T>
		std::shared_ptr<T> findChild(const std::string& nameOfChildToFind = "")
		{
			for (auto& c : children)
			{
				if (nameOfChildToFind.empty() && std::dynamic_pointer_cast<T>(c))
					return std::dynamic_pointer_cast<T>(c);

				if (c->getName() == nameOfChildToFind && std::dynamic_pointer_cast<T>(c))
					return std::dynamic_pointer_cast<T>(c);

				auto candidate = c->findChild<T>(nameOfChildToFind);

				if (candidate)
					return candidate;
			}

			return {};
		}

		std::shared_ptr<Component> addChild(std::shared_ptr<Component> child);

        template<typename T, typename... Args>
        std::shared_ptr<T> addChildT(Args&... args)
        {
            auto child = std::make_shared<T>(args...);
            addChild(child);
            return child;
        }

        void removeChild(std::shared_ptr<Component> child);
		void addChildren(std::vector<std::shared_ptr<Component>> children);
		std::shared_ptr<Component> findChild(const std::string& nameOfChildToFind);
		std::shared_ptr<Label> findLabel(const std::string& name);
		std::shared_ptr<Field> findField(const std::string& name);
		std::shared_ptr<Parameter> findParameter(const std::string& name);
		std::vector<std::shared_ptr<Label>> findLabels();
		std::vector<std::shared_ptr<Field>> findFields();
		std::vector<std::shared_ptr<Parameter>> findParameters();
		std::shared_ptr<ScreenComponent> findScreenComponent();
        std::shared_ptr<Background> findBackground();
        MRECT getRect();
		std::vector<std::shared_ptr<Component>> findHiddenChildren();
		void deleteChildren(const std::string& nameOfChildrenToDelete);

	public:
		virtual void Hide(bool b);
		virtual void SetDirty(bool b = true);
		bool IsHidden();
		bool IsDirty();
		MRECT getDirtyArea();
		virtual void setSize(int newW, int newH);
		void setLocation(int newX, int newY);
		const std::string& getName();
		void sendToBack(std::shared_ptr<Component> childToSendBack);
		bool bringToFront(Component* childToBringToFront);
		Component* getParent();

	public:
		virtual void Draw(std::vector<std::vector<bool>>* pixels);
		virtual void Clear(std::vector<std::vector<bool>>* pixels);
		void preDrawClear(std::vector<std::vector<bool>>* pixels);

	public:
		MRECT preDrawClearRect;
		Component(const std::string& name);

	};
}
