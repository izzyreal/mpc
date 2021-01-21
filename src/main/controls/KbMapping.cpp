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
	labelKeyMap["play-start"] = kh->code("backslash");
	labelKeyMap["main-screen"] = kh->code("escape");
	labelKeyMap["open-window"] = kh->code("i");
	labelKeyMap["prev-step-event"] = kh->code("q");
	labelKeyMap["next-step-event"] = kh->code("w");
	labelKeyMap["go-to"] = kh->code("e");
	labelKeyMap["prev-bar-start"] = kh->code("r");
	labelKeyMap["next-bar-end"] = kh->code("t");
	labelKeyMap["tap"] = kh->code("y");
	labelKeyMap["next-seq"] = kh->code("[");
	labelKeyMap["track-mute"] = kh->code("]");
	labelKeyMap["full-level"] = kh->code("o");
	labelKeyMap["sixteen-levels"] = kh->code("p");
	labelKeyMap["f1"] = kh->code("f1");
	labelKeyMap["f2"] = kh->code("f2");
	labelKeyMap["f3"] = kh->code("f3");
	labelKeyMap["f4"] = kh->code("f4");
	labelKeyMap["f5"] = kh->code("f5");
	labelKeyMap["f6"] = kh->code("f6");
	labelKeyMap["shift"] = kh->code("left shift");
	labelKeyMap["enter"] = kh->code("enter");
	labelKeyMap["undo-seq"] = kh->code("f10");
//	labelKeyMap["erase"] = kh->code("f11"); // These are reserved on OSX.
//	labelKeyMap["after"] = kh->code("f12"); // Find alternatives.
	labelKeyMap["bank-a"] = kh->code("home");
	labelKeyMap["bank-b"] = kh->code("end");
	labelKeyMap["bank-c"] = kh->code("insert");
	labelKeyMap["bank-d"] = kh->code("delete");
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
    labelKeyMap["pad-1"] = kh->code("z");
    labelKeyMap["pad-2"] = kh->code("x");
    labelKeyMap["pad-3"] = kh->code("c");
    labelKeyMap["pad-4"] = kh->code("v");
    labelKeyMap["pad-5"] = kh->code("a");
    labelKeyMap["pad-6"] = kh->code("s");
    labelKeyMap["pad-7"] = kh->code("d");
    labelKeyMap["pad-8"] = kh->code("f");
    labelKeyMap["pad-9"] = kh->code("b");
    labelKeyMap["pad-10"] = kh->code("n");
    labelKeyMap["pad-11"] = kh->code("m");
    labelKeyMap["pad-12"] = kh->code("comma");
    labelKeyMap["pad-13"] = kh->code("g");
    labelKeyMap["pad-14"] = kh->code("h");
    labelKeyMap["pad-15"] = kh->code("j");
    labelKeyMap["pad-16"] = kh->code("k");
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
