#include "KbMapping.hpp"
#include <sys/KeyCodes.hpp>
#include <file/File.hpp>
#include <Paths.hpp>

#include <Logger.hpp>

using namespace mpc::controls;
using namespace WonderRabbitProject::key;
using namespace moduru::sys;
using namespace std;

KbMapping::KbMapping()
{
    importMapping();
}
const key_helper_t* mpc::controls::KbMapping::kh = &key_helper_t::instance();

vector<string> KbMapping::getMappedLabels()
{
    vector<string> result;
    for (auto kv : labelKeyMap)
        result.push_back(kv.first);
    return result;
}

void KbMapping::exportMapping() {
	auto path = mpc::Paths::configPath() + "keys.txt";
	moduru::file::File f(path, nullptr);
	
    if (f.exists())
		f.del();
	else
		f.create();
	
    vector<char> bytes;
	
    for (pair<string, int> x : labelKeyMap)
    {
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

void KbMapping::importMapping()
{
    labelKeyMap.clear();
    auto path = mpc::Paths::configPath() + "keys.txt";
    moduru::file::File f(path, nullptr);
    
    if (!f.exists())
    {
        MLOG("Initializing default key mapping...");
        initializeDefaults();
        return;
    }
    
    vector<char> bytes(f.getLength());
    
    f.getData(&bytes);
    
    string label = "";
    string keyCode = "";
    bool parsingLabel = true;
    std::vector<int> shifts;

    for (int i = 0; i < bytes.size(); i++)
    {
        if (bytes[i] != ' ' && parsingLabel)
        {
            label += bytes[i];
            continue;
        }
        
        if (bytes[i] == ' ')
        {
            parsingLabel = false;
            continue;
        }
        
        if (bytes[i] != ' ' && bytes[i] != '\n' && !parsingLabel)
        {
            keyCode += bytes[i];
            continue;
        }
        
        if (bytes[i] == '\n' || i == bytes.size() - 1)
        {
            int parsedKeyCode = -1;
            
            try
            {
                parsedKeyCode = stoi(keyCode);
            }
            catch (const exception& e)
            {
                // Cancel the whole function?
            }
            
            if (parsedKeyCode != -1)
            {
                labelKeyMap.emplace_back(label, parsedKeyCode);
                if (label == "shift") shifts.push_back(parsedKeyCode);
            }
            
            label = "";
            keyCode = "";
            parsingLabel = true;
        }
    }

    if (shifts.size() == 1)
    {
        if (kh->name(shifts[0]) == "shift" || kh->name(shifts[0]) == "left shift")
        {
            labelKeyMap.emplace_back("shift", kh->code("right shift"));
        }
    }
}

void KbMapping::initializeDefaults()
{
    labelKeyMap.clear();
    labelKeyMap.emplace_back("left", kh->code("left"));
    labelKeyMap.emplace_back("right", kh->code("right"));
    labelKeyMap.emplace_back("up", kh->code("up"));
    labelKeyMap.emplace_back("down", kh->code("down"));
    labelKeyMap.emplace_back("rec", kh->code("l"));
    labelKeyMap.emplace_back("overdub", kh->code("semicolon"));
    labelKeyMap.emplace_back("stop", kh->code("quote"));
    labelKeyMap.emplace_back("play", kh->code("space"));
    labelKeyMap.emplace_back("play-start", kh->code("backslash"));
    labelKeyMap.emplace_back("main-screen", kh->code("escape"));
    labelKeyMap.emplace_back("open-window", kh->code("i"));
    labelKeyMap.emplace_back("prev-step-event", kh->code("q"));
    labelKeyMap.emplace_back("next-step-event", kh->code("w"));
    labelKeyMap.emplace_back("go-to", kh->code("e"));
    labelKeyMap.emplace_back("prev-bar-start", kh->code("r"));
    labelKeyMap.emplace_back("next-bar-end", kh->code("t"));
    labelKeyMap.emplace_back("tap", kh->code("y"));
    labelKeyMap.emplace_back("next-seq", kh->code("["));
    labelKeyMap.emplace_back("track-mute", kh->code("]"));
    labelKeyMap.emplace_back("full-level", kh->code("o"));
    labelKeyMap.emplace_back("sixteen-levels", kh->code("p"));
    labelKeyMap.emplace_back("f1", kh->code("f1"));
    labelKeyMap.emplace_back("f2", kh->code("f2"));
    labelKeyMap.emplace_back("f3", kh->code("f3"));
    labelKeyMap.emplace_back("f4", kh->code("f4"));
    labelKeyMap.emplace_back("f5", kh->code("f5"));
    labelKeyMap.emplace_back("f6", kh->code("f6"));
    labelKeyMap.emplace_back("shift", kh->code("left shift"));
    labelKeyMap.emplace_back("shift", kh->code("right shift"));
    labelKeyMap.emplace_back("enter", kh->code("enter"));
    labelKeyMap.emplace_back("undo-seq", kh->code("f10"));
    labelKeyMap.emplace_back("erase", kh->code("f8"));
    labelKeyMap.emplace_back("after", kh->code("f9"));
    labelKeyMap.emplace_back("bank-a", kh->code("home"));
    labelKeyMap.emplace_back("bank-b", kh->code("end"));
    labelKeyMap.emplace_back("bank-c", kh->code("insert"));
    labelKeyMap.emplace_back("bank-d", kh->code("delete"));
    labelKeyMap.emplace_back("0", kh->code("0"));
    labelKeyMap.emplace_back("1", kh->code("1"));
    labelKeyMap.emplace_back("2", kh->code("2"));
    labelKeyMap.emplace_back("3", kh->code("3"));
    labelKeyMap.emplace_back("4", kh->code("4"));
    labelKeyMap.emplace_back("5", kh->code("5"));
    labelKeyMap.emplace_back("6", kh->code("6"));
    labelKeyMap.emplace_back("7", kh->code("7"));
    labelKeyMap.emplace_back("8", kh->code("8"));
    labelKeyMap.emplace_back("9", kh->code("9"));
    labelKeyMap.emplace_back("pad-1", kh->code("z"));
    labelKeyMap.emplace_back("pad-2", kh->code("x"));
    labelKeyMap.emplace_back("pad-3", kh->code("c"));
    labelKeyMap.emplace_back("pad-4", kh->code("v"));
    labelKeyMap.emplace_back("pad-5", kh->code("a"));
    labelKeyMap.emplace_back("pad-6", kh->code("s"));
    labelKeyMap.emplace_back("pad-7", kh->code("d"));
    labelKeyMap.emplace_back("pad-8", kh->code("f"));
    labelKeyMap.emplace_back("pad-9", kh->code("b"));
    labelKeyMap.emplace_back("pad-10", kh->code("n"));
    labelKeyMap.emplace_back("pad-11", kh->code("m"));
    labelKeyMap.emplace_back("pad-12", kh->code("comma"));
    labelKeyMap.emplace_back("pad-13", kh->code("g"));
    labelKeyMap.emplace_back("pad-14", kh->code("h"));
    labelKeyMap.emplace_back("pad-15", kh->code("j"));
    labelKeyMap.emplace_back("pad-16", kh->code("k"));
    labelKeyMap.emplace_back("datawheel-down", kh->code("minus"));
    labelKeyMap.emplace_back("datawheel-up", kh->code("equals"));
    labelKeyMap.emplace_back("ctrl", kh->code("control"));
#ifdef __APPLE__
    labelKeyMap.emplace_back("alt", kh->code("option"));
#elif defined _WIN32 || defined __linux__
    labelKeyMap.emplace_back("alt", kh->code("alternate"));
#endif
}

int KbMapping::getKeyCodeFromLabel(const std::string& label) {
	for (auto& kv : labelKeyMap)
    {
        if (kv.first == label)
        {
            return kv.second;
        }
    }
	return -1;
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

void KbMapping::setKeyCodeForLabel(const int keyCode, std::string label)
{
    for (auto& kv : labelKeyMap)
    {
        if (kv.first == label) kv.second = keyCode;
    }
}

int KbMapping::getNextKeyCode(int keyCode)
{
    bool wasFound = false;
    int last = -1;

    int result = -1;

    for (auto& kv : KeyCodes::keyCodeNames)
    {
        auto keyCode2 = kv.first;
        if (wasFound)
        {
            result = keyCode2;
            wasFound = false;
        }

        if (keyCode2 == keyCode)
        {
            wasFound = true;
        }

        last = keyCode2;
    }

    if (result != -1)
    {
        return result;
    }
    return last;
}

int KbMapping::getPreviousKeyCode(int keyCode)
{
    int first = -1;

    int previous = -1;
    int counter = 0;
    for (auto& kv : KeyCodes::keyCodeNames)
    {
        auto keyCode2 = kv.first;

        if (first == -1)
        {
            first = keyCode2;
        }

        if (keyCode2 == keyCode && counter > 0)
        {
            return previous;
        }

        previous = keyCode2;
        counter++;
    }

    return first;
}
