#ifndef COMPONENT
#define COMPONENT

#include <gui/BasicStructs.hpp>

#include <vector>

namespace mpc {
	namespace lcdgui {
		class Component {

		protected:
			MRECT rect;
			MRECT dirtyRect;
			std::vector<MRECT> clearRects;
			bool hidden = false;

		protected:
			bool dirty = false;

		public:
			virtual void Hide(bool b);
			void SetDirty();

			bool IsHidden();
			bool IsDirty();
			bool NeedsClearing();
			MRECT* getDirtyArea();

		public:
			virtual void Draw(std::vector<std::vector<bool> >* pixels) {}
			virtual void Clear(std::vector<std::vector<bool> >* pixels);

			MRECT* GetRECT();

		public:
			virtual ~Component() {}

		};

	}
}


#endif // !COMPONENT
