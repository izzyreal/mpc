#include <midi/util/VariableLengthInt.hpp>

#include <midi/util/MidiUtil.hpp>

using namespace mpc::midi::util;
using namespace std;

VariableLengthInt::VariableLengthInt(int value) 
{
	setValue(value);
}

VariableLengthInt::VariableLengthInt(moduru::io::InputStream* in)
{
	parseBytes(in);
}

void VariableLengthInt::setValue(int value)
{
    mValue = value;
    buildBytes();
}

int VariableLengthInt::getValue()
{
    return mValue;
}

int VariableLengthInt::getByteCount()
{
    return mSizeInBytes;
}

vector<char> VariableLengthInt::getBytes()
{
    return mBytes;
}

void VariableLengthInt::parseBytes(moduru::io::InputStream* in)
{
	auto ints = vector<int>(4);
	mSizeInBytes = 0;
	mValue = 0;
	int shift = 0;
	auto b = in->read();
	while (mSizeInBytes < 4) {
		mSizeInBytes++;
		auto variable = (b & 128) > 0;
		if (!variable) {
			ints[mSizeInBytes - 1] = (b & 127);
			break;
		}
		ints[mSizeInBytes - 1] = (b & 127);
		b = in->read();
	}
	for (int i = 1; i < mSizeInBytes; i++) {
		shift += 7;
	}
	mBytes = vector<char>(mSizeInBytes);
	for (int i = 0; i < mSizeInBytes; i++) {
		mBytes[i] = static_cast<char>(ints[i]);
		mValue += ints[i] << shift;
		shift -= 7;
	}
}

void VariableLengthInt::buildBytes()
{
	if (mValue == 0) {
		mBytes = vector<char>(1);
		mBytes[0] = 0;
		mSizeInBytes = 1;
		return;
	}
	mSizeInBytes = 0;
	auto vals = vector<int>(4);
	auto tmpVal = mValue;
	while (mSizeInBytes < 4 && tmpVal > 0) {
		vals[mSizeInBytes] = tmpVal & 127;
		mSizeInBytes++;
		tmpVal = tmpVal >> 7;
	}
	for (int i = 1; i < mSizeInBytes; i++) {
		vals[i] |= 128;
	}
	mBytes = vector<char>(mSizeInBytes);
	for (int i = 0; i < mSizeInBytes; i++) {
		mBytes[i] = static_cast<char>(vals[mSizeInBytes - i - 1]);
	}
}

string VariableLengthInt::toString()
{
	return MidiUtil::bytesToHex(mBytes) + " (" + to_string(mValue) + ")";
}
