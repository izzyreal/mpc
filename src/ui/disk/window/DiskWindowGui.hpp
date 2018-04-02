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
					int loadInto{ 0 };
				
				public:
					void setLoadInto(int i);
					int getLoadInto();
					void setDelete(int i);
					int getDelete();

					DiskWindowGui();
					~DiskWindowGui();

				};

			}
		}
	}
}
