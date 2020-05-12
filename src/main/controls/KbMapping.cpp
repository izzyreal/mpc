#include "KbMapping.hpp"
#include <thirdp/wrpkey/key.hxx>

#include <file/File.hpp>
#include <Paths.hpp>

#include <Logger.hpp>

using namespace mpc::controls;
using namespace WonderRabbitProject::key;
using namespace std;

KbMapping::KbMapping()
{
	labelKeyMap["left"] = kh->code("left");
	labelKeyMap["right"] = kh->code("right");
	labelKeyMap["up"] = kh->code("up");
	labelKeyMap["down"] = kh->code("down");
	labelKeyMap["rec"] = kh->code("l");
	labelKeyMap["overdub"] = kh->code("semicolon");
	labelKeyMap["stop"] = kh->code("quote");
	labelKeyMap["play"] = kh->code("space");
	labelKeyMap["playstart"] = kh->code("backslash");
	labelKeyMap["mainscreen"] = kh->code("escape");
	labelKeyMap["openwindow"] = kh->code("i");
	labelKeyMap["prevstepevent"] = kh->code("q");
	labelKeyMap["nextstepevent"] = kh->code("w");
	labelKeyMap["goto"] = kh->code("e");
	labelKeyMap["prevbarstart"] = kh->code("r");
	labelKeyMap["nextbarend"] = kh->code("t");
	labelKeyMap["tap"] = kh->code("y");
	labelKeyMap["nextseq"] = kh->code("[");
	labelKeyMap["trackmute"] = kh->code("]");
	labelKeyMap["fulllevel"] = kh->code("o");
	labelKeyMap["sixteenlevels"] = kh->code("p");
	labelKeyMap["f1"] = kh->code("f1");
	labelKeyMap["f2"] = kh->code("f2");
	labelKeyMap["f3"] = kh->code("f3");
	labelKeyMap["f4"] = kh->code("f4");
	labelKeyMap["f5"] = kh->code("f5");
	labelKeyMap["f6"] = kh->code("f6");
	labelKeyMap["shift"] = kh->code("left shift");
	labelKeyMap["enter"] = kh->code("enter");
	labelKeyMap["undoseq"] = kh->code("f10");
	labelKeyMap["erase"] = kh->code("f11");
	labelKeyMap["after"] = kh->code("f12");
	labelKeyMap["banka"] = kh->code("home");
	labelKeyMap["bankb"] = kh->code("end");
	labelKeyMap["bankc"] = kh->code("insert");
	labelKeyMap["bankd"] = kh->code("delete");
	labelKeyMap["0"] = kh->code("0");
	labelKeyMap["1"] = kh->code("1");
	labelKeyMap["2"] = kh->code("2");
	labelKeyMap["3"] = kh->code("3");
	labelKeyMap["4"] = kh->code("4");
	labelKeyMap["5"] = kh->code("5");
	labelKeyMap["6"] = kh->code("6");
	labelKeyMap["7"] = kh->code("7");
	labelKeyMap["8"] = kh->code("8");
	labelKeyMap["9"] = kh->code("9");
	exportMapping();
}
const key_helper_t* mpc::controls::KbMapping::kh = &key_helper_t::instance();

void KbMapping::exportMapping() {
	auto path = mpc::Paths::resPath() + "/keys.txt";
	moduru::file::File f(path, nullptr);
	if (f.exists()) {
		f.del();
	}
	else {
		f.create();
	}
	std::vector<char> bytes;
	for (std::pair<std::string, int> x : labelKeyMap) {
		for (char& c : x.first)
			bytes.push_back(c);
		bytes.push_back(' ');
		string keyCode = to_string(x.second);
		for (char& c : keyCode)
			bytes.push_back(c);
		bytes.push_back('\n');
	}
	f.setData(&bytes);
	f.close();
}

int KbMapping::getKeyCodeFromLabel(std::string label) {
	if (labelKeyMap.find(label) == labelKeyMap.end()) return -1;
	return labelKeyMap[label];
}

std::string KbMapping::getLabelFromKeyCode(int keyCode) {
	for (std::pair<std::string, int> x : labelKeyMap) {
		std::string key = x.first;
		if (keyCode == x.second) return key;
	}
	return "";
}

std::string KbMapping::getKeyCodeString(int keyCode) {
	auto names = kh->names(keyCode);
	if (names.size() > 1) {
		//MLOG("\nKeycode: " + std::to_string(keyCode));
		//MLOG("Button label: " + getLabelFromKeyCode(keyCode));
		//MLOG("Key names:");
		for (auto& s : names) {
			//MLOG(s);
			if (s.length() > 3) return s;
		}
	}
	return kh->name(keyCode);
}

int KbMapping::dataWheelBack()
{
    return kh->code("minus");
}

int KbMapping::dataWheelForward()
{
    
	return kh->code("equals");
}

vector<int> KbMapping::padKeys()
{
    return vector<int>{ kh->code("z"), kh->code("x"), kh->code("c"), kh->code("v"), kh->code("a"), kh->code("s"), kh->code("d"), kh->code("f"), kh->code("b"), kh->code("n"), kh->code("m"), kh->code("comma"), kh->code("g"), kh->code("h"), kh->code("j"), kh->code("k") };
}

vector<int> KbMapping::numPad()
{
    return vector<int>{ kh->code("0"), kh->code("1"), kh->code("2"), kh->code("3"), kh->code("4"), kh->code("5"), kh->code("6"), kh->code("7"), kh->code("8"), kh->code("9") };
}

vector<int> KbMapping::altNumPad()
{
	return vector<int>{ 96, 97, 98, 99, 100, 101, 102, 103, 104, 105 };
}


int KbMapping::ctrl() {
	return kh->code("left control");
}

int KbMapping::alt() {
	return kh->code("left alternate");
}
