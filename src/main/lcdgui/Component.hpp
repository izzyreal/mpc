#ifndef COMPONENT
#define COMPONENT

#include <gui/BasicStructs.hpp>

#include <vector>
#include <string>

namespace mpc::lcdgui {

	class Component {

	protected:
		MRECT rect;
		MRECT dirtyRect;
		std::vector<MRECT> clearRects;
		bool hidden = false;
		bool dirty = false;
		std::string name{ "" };

	public:
		virtual void Hide(bool b);
		void SetDirty();

		bool IsHidden();
		bool IsDirty();
		bool NeedsClearing();
		MRECT* getDirtyArea();

		const std::string& getName();

	public:
		virtual void Draw(std::vector<std::vector<bool>>* pixels) {}
		virtual void Clear(std::vector<std::vector<bool>>* pixels);

		MRECT* GetRECT();

	public:
		Component(const std::string& name);
		virtual ~Component() {}

	};
}

#endif // !COMPONENT
