#include <midi/util/MidiUtil.hpp>

using namespace mpc::midi::util;
using namespace std;

int MidiUtil::ticksToMs(int ticks, int mpqn, int resolution)
{
    return ((ticks * mpqn) / resolution) / 1000;
}

int MidiUtil::ticksToMs(int ticks, float bpm, int resolution)
{   
    return ticksToMs(ticks, bpmToMpqn(bpm), resolution);
}

double MidiUtil::msToTicks(int ms, int mpqn, int ppq)
{
    
    return ((ms * 1000.0) * ppq) / mpqn;
}

double MidiUtil::msToTicks(int ms, float bpm, int ppq)
{
    
    return msToTicks(ms, bpmToMpqn(bpm), ppq);
}

int MidiUtil::bpmToMpqn(float bpm)
{

	return (int) (bpm * 60000000);
}

float MidiUtil::mpqnToBpm(int mpqn)
{
    
    return mpqn / 6.0E7f;
}

int MidiUtil::bytesToInt(vector<char> buff, int off, int len)
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

	auto buffer = vector<char>(byteCount);
	auto ints = vector<char>(byteCount);
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

bool MidiUtil::bytesEqual(vector<char> buf1, vector<char> buf2, int off, int len)
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

vector<char> MidiUtil::extractBytes(vector<char> buffer, int off, int len)
{

	auto ret = vector<char>(len);
	for (int i = 0; i < len; i++) {
		ret[i] = buffer[off + i];
	}
	return ret;
}

string MidiUtil::HEX = "0123456789ABCDEF";

string MidiUtil::byteToHex(char b)
{

	int high = (b & 240) >> 4;
	int low = (b & 15);
	string res = "";
	res.push_back(HEX[high]);
	res.push_back(HEX[low]);
	return res;
}

string MidiUtil::bytesToHex(vector<char> b)
{
	string res = "";
	for (int i = 0; i < b.size(); i++) {
		res += byteToHex(b[i]) + " ";
	}
	return res;
}
