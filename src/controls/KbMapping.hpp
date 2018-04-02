#pragma once
#include <cstdint>
#include <vector>
#include <thirdp/wrpkey/key.hxx>

#include <unordered_map>
#include <string>

namespace mpc {
	namespace controls {

		class KbMapping
		{

		private:
			const static WonderRabbitProject::key::key_helper_t* kh;

		private:
			std::unordered_map<std::string, int> labelKeyMap;

		public:
			static int dataWheelBack();
			static int dataWheelForward();
			static std::vector<int> padKeys();
			static std::vector<int> numPad();
			static std::vector<int> altNumPad();

			static int ctrl();
			static int alt();

		public:
			int getKeyCodeFromLabel(std::string label);
			std::string getLabelFromKeyCode(int keyCode);
			std::string getKeyCodeString(int keyCode);

		public:
			void exportMapping();

		public:
			KbMapping();

		};

	}
}
