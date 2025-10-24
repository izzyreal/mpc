#include "midi/util/MidiUtil.hpp"

using namespace mpc::midi::util;
using namespace std;

int MidiUtil::bytesToInt(std::vector<char> buff, int off, int len)
{
	int num = 0;
	int shift = 0;
	for (int i = off + len - 1; i >= off; i--) {
		num += (buff[i] & 255) << shift;
		shift += 8;
	}
	return num;
}

vector<char> MidiUtil::intToBytes(int val, int byteCount)
{

	auto buffer = std::vector<char>(byteCount);
	auto ints = std::vector<char>(byteCount);
	for (int i = 0; i < byteCount; i++) {
		ints[i] = val & 255;
		buffer[byteCount - i - 1] = ints[i];
		val = val >> 8;
		if (val == 0) {
			break;
		}
	}
	return buffer;
}

bool MidiUtil::bytesEqual(std::vector<char> buf1, std::vector<char> buf2, int off, int len)
{
	for (int i = off; i < off + len; i++) {
		if (i >= buf1.size() || i >= buf2.size()) {
			return false;
		}
		if (buf1[i] != buf2[i]) {
			return false;
		}
	}
	return true;
}

string MidiUtil::HEX = "0123456789ABCDEF";

string MidiUtil::byteToHex(char b)
{

	int high = (b & 240) >> 4;
	int low = (b & 15);
	string res;
	res.push_back(HEX[high]);
	res.push_back(HEX[low]);
	return res;
}

string MidiUtil::bytesToHex(std::vector<char> b)
{
	string res = "";
	for (int i = 0; i < b.size(); i++) {
		res += byteToHex(b[i]) + " ";
	}
	return res;
}
