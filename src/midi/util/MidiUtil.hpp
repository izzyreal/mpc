#pragma once

#include <vector>
#include <string>

namespace mpc {
	namespace midi {
		namespace util {

			class MidiUtil
			{

			public:
				static int ticksToMs(int ticks, int mpqn, int resolution);
				static int ticksToMs(int ticks, float bpm, int resolution);
				static double msToTicks(int ms, int mpqn, int ppq);
				static double msToTicks(int ms, float bpm, int ppq);
				static int bpmToMpqn(float bpm);
				static float mpqnToBpm(int mpqn);
				static int bytesToInt(std::vector<char> buff, int off, int len);
				static std::vector<char> intToBytes(int val, int byteCount);
				static bool bytesEqual(std::vector<char> buf1, std::vector<char> buf2, int off, int len);
				static std::vector<char> extractBytes(std::vector<char> buffer, int off, int len);

			private:
				static std::string HEX;

			public:
				static std::string byteToHex(char b);
				static std::string bytesToHex(std::vector<char> b);

			};

		}
	}
}
