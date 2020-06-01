#pragma once
#include <observer/Observable.hpp>

namespace mpc {

	namespace sequencer {
		class Sequence;
	}

	namespace ui {
		namespace disk {
			namespace window {

				class DiskWindowGui
					: public moduru::observer::Observable
				{

				private:
					int deleteIndex{ 0 };

				public:
					void setDelete(int i);
					int getDelete();

					DiskWindowGui();
					~DiskWindowGui();

				};

			}
		}
	}
}
