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
}

namespace mpc::lcdgui {

	class Component {

	private:
		std::vector<std::shared_ptr<Component>> children;

	protected:
		std::string name{ "" };
		bool hidden = false;
		bool dirty = false;
		unsigned int x{ 0 };
		unsigned int y{ 0 };
		unsigned int w{ 0 };
		unsigned int h{ 0 };

	public:
		std::weak_ptr<Component> addChild(std::shared_ptr<Component> child);
		void addChildren(std::vector<std::shared_ptr<Component>> children);
		std::weak_ptr<Component> findChild(const std::string& name);
		std::weak_ptr<Label> findLabel(const std::string& name);
		std::weak_ptr<Field> findField(const std::string& name);
		std::vector<std::weak_ptr<Label>> findLabels();
		std::vector<std::weak_ptr<Field>> findFields();
		std::vector<std::weak_ptr<Parameter>> findParameters();
		MRECT getRect();

	public:
		virtual void Hide(bool b);
		void SetDirty();
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
		virtual ~Component() {}

	};
}

#endif // !COMPONENT
