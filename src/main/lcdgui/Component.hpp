#ifndef COMPONENT
#define COMPONENT

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
}

namespace mpc::lcdgui
{
	class Component
	{

	protected:
		Component* parent = nullptr;
		std::vector<std::shared_ptr<Component>> children;

	protected:
		std::string name = "";
		bool hidden = false;
		bool dirty = false;
		int x = -1;
		int y = -1;
		int w = -1;
		int h = -1;

	protected:
		bool shouldNotDraw(std::vector<std::vector<bool>>* pixels);

	public:
		template<class T>
		std::weak_ptr<T> findChild(const std::string& name)
		{
			for (auto& c : children)
			{

				if (name.compare("") == 0 && dynamic_pointer_cast<T>(c))
				{
					return dynamic_pointer_cast<T>(c);
				}

				if (c->getName().compare(name) == 0 && dynamic_pointer_cast<T>(c))
				{
					return dynamic_pointer_cast<T>(c);
				}

				auto candidate = c->findChild<T>(name).lock();

				if (candidate)
				{
					return candidate;
				}
			}

			return {};
		}

		virtual std::weak_ptr<Component> addChild(std::shared_ptr<Component> child);
		void removeChild(std::weak_ptr<Component> child);
		void addChildren(std::vector<std::shared_ptr<Component>> children);
		std::weak_ptr<Component> findChild(const std::string& name);
		std::weak_ptr<Label> findLabel(const std::string& name);
		std::weak_ptr<Field> findField(const std::string& name);
		std::weak_ptr<Parameter> findParameter(const std::string& name);
		std::vector<std::weak_ptr<Label>> findLabels();
		std::vector<std::weak_ptr<Field>> findFields();
		std::vector<std::weak_ptr<Parameter>> findParameters();
		std::weak_ptr<ScreenComponent> findScreenComponent();
		MRECT getRect();
		std::vector<std::weak_ptr<Component>> findHiddenChildren();
		void deleteChildren(const std::string& name);

	public:
		virtual void Hide(bool b);
		virtual void SetDirty(bool b = true);
		bool IsHidden();
		bool IsDirty();
		MRECT getDirtyArea();
		void setSize(int width, int height);
		void setLocation(int x, int y);
		const std::string& getName();
		void sendToBack(std::weak_ptr<Component> childToSendBack);
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

#endif // !COMPONENT
