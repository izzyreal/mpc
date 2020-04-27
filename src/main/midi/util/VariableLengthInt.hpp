#pragma once

#include <string>
#include <vector>
#include <sstream>

using namespace std;

namespace mpc::midi::util {

	class VariableLengthInt
	{

	private:
		int mValue = 0;
		vector<char> mBytes;
		int mSizeInBytes = 0;

	public:
		void setValue(int value);
		int getValue();
		int getByteCount();
		vector<char> getBytes();
		string toString();

	private:
		void parseBytes(stringstream& in);
		void buildBytes();

	public:
		VariableLengthInt();
		VariableLengthInt(int value);
		VariableLengthInt(stringstream& in);
	};
}
