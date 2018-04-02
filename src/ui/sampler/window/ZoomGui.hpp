#pragma once
#include <observer/Observable.hpp>

namespace mpc {
	namespace ui {
		namespace sampler {
			namespace window {

				class ZoomGui
					: public moduru::observer::Observable
				{

				private:
					bool smplLngthFix{ false };
					bool loopLngthFix{ false };
					int zoomLevel{ 6 };

				public:
					//void setZoomLevel(int i);
					//int getZoomLevel();
					void setSmplLngthFix(bool b);
					bool isSmplLngthFix();
					void setLoopLngthFix(bool b);
					bool isLoopLngthFix();

				public:
					ZoomGui();
				};

			}
		}
	}
}
