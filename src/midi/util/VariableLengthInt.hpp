#pragma once

#include <io/InputStream.hpp>

#include <string>
#include <vector>

namespace mpc {
	namespace midi {
		namespace util {

			class VariableLengthInt
			{

			private:
				int mValue{};
				std::vector<char> mBytes{};
				int mSizeInBytes{};

			public:
				void setValue(int value);
				int getValue();
				int getByteCount();
				std::vector<char> getBytes();
				std::string toString();

			private:
				void parseBytes(moduru::io::InputStream* in);
				void buildBytes();

			public:
				VariableLengthInt(int value);
				VariableLengthInt(moduru::io::InputStream* in);
			};

		}
	}
}
