#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <set>

namespace mpc {

	class Mpc;

	namespace controls {

		class AbstractControls;
		class GlobalReleaseControls;

		class Controls
		{
		private:
			bool ctrlPressed{ false };
			bool altPressed{ false };

		public:
			void setCtrlPressed(bool b);
			void setAltPressed(bool b);
			bool isCtrlPressed();
			bool isAltPressed();

		private:
			bool shiftPressed{ false };
			bool recPressed{ false };
			bool overDubPressed{ false };
			bool tapPressed{ false };
			bool goToPressed{ false };
			bool erasePressed{ false };
			bool f3Pressed{ false };
			bool f4Pressed{ false };
			bool f5Pressed{ false };
			bool f6Pressed{ false };
			std::set<int> pressedPads;
			std::vector<int> pressedPadVelos;

		public:
			void setErasePressed(bool b);
			void setRecPressed(bool b);
			void setOverDubPressed(bool b);
			void setTapPressed(bool b);
			void setGoToPressed(bool b);
			void setShiftPressed(bool b);
			void setF3Pressed(bool b);
			void setF4Pressed(bool b);
			void setF5Pressed(bool b);
			void setF6Pressed(bool b);

			bool isErasePressed();
			bool isRecPressed();
			bool isOverDubPressed();
			bool isTapPressed();
			bool isGoToPressed();
			bool isShiftPressed();
			bool isF3Pressed();
			bool isF4Pressed();
			bool isF5Pressed();
			bool isF6Pressed();

			std::set<int>* getPressedPads();
			std::vector<int>* getPressedPadVelos();

		private:
			std::unordered_map<std::string, controls::AbstractControls*> controls{};

		public:
			mpc::controls::AbstractControls* getControls(std::string s);
			mpc::controls::GlobalReleaseControls* getReleaseControls();

		public:
			Controls(Mpc* mpc);
			~Controls();
		};
	}
}
