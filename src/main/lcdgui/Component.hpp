#ifndef COMPONENT
#define COMPONENT

#include <gui/BasicStructs.hpp>

#include <vector>
#include <string>
#include <memory>

namespace mpc::lcdgui {
	class Label;
	class Field;
	class Parameter;
	class ScreenComponent;
}

namespace mpc::lcdgui {

	class Component {

	protected:
		std::vector<std::shared_ptr<Component>> children;

	protected:
		std::string name = "";
		bool hidden = false;
		bool dirty = false;
		int x = 0;
		int y = 0;
		int w = 0;
		int h = 0;

	protected:
		bool shouldNotDraw(std::vector<std::vector<bool>>* pixels);

	public:
		std::weak_ptr<Component> addChild(std::shared_ptr<Component> child);
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
		virtual void SetDirty();
		bool IsHidden();
		bool IsDirty();
		MRECT getDirtyArea();
		void setSize(int width, int height);
		void setLocation(int x, int y);
		const std::string& getName();

	public:
		virtual void Draw(std::vector<std::vector<bool>>* pixels);
		virtual void Clear(std::vector<std::vector<bool>>* pixels);

	public:
		Component(const std::string& name);

	};
}

#endif // !COMPONENT
