#ifndef COMPONENT
#define COMPONENT

#include <gui/BasicStructs.hpp>

#include <vector>
#include <string>
#include <memory>

namespace mpc::lcdgui {
	class Label;
	class Field;
}

namespace mpc::lcdgui {

	class Component {

	private:
		std::vector<std::shared_ptr<Component>> children;

	protected:
		MRECT rect;
		MRECT dirtyRect;
		std::vector<MRECT> clearRects;
		bool hidden = false;
		bool dirty = false;
		std::string name{ "" };

	public:
		std::weak_ptr<Component> addChild(std::shared_ptr<Component> child);
		void addChildren(std::vector<std::shared_ptr<Component>> children);
		std::weak_ptr<Component> findChild(const std::string& name);
		std::weak_ptr<Label> findLabel(const std::string& name);
		std::weak_ptr<Field> findField(const std::string& name);
		std::weak_ptr<Component> getDirtyChildren();

	public:
		virtual void Hide(bool b);
		void SetDirty();

		bool IsHidden();
		bool IsDirty();
		bool NeedsClearing();
		MRECT getDirtyArea();

		const std::string& getName();

	public:
		virtual void Draw(std::vector<std::vector<bool>>* pixels);
		virtual void Clear(std::vector<std::vector<bool>>* pixels);

		MRECT* GetRECT();

	public:
		Component(const std::string& name);
		virtual ~Component() {}

	};
}

#endif // !COMPONENT
