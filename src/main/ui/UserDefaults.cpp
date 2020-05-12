#include <ui/UserDefaults.hpp>

#include <nvram/NvRam.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::ui;
using namespace std;

UserDefaults::UserDefaults() 
{
    initialize();
}

UserDefaults::~UserDefaults()
{
    mpc::nvram::NvRam::saveUserDefaults();
}

void UserDefaults::initialize()
{
    sequenceName = string("Sequence");
    bus = 1;
    tempo = BCMath("120.0");
    velo = 100;
    pgm = 0;
    recordingModeMulti = false;
    loop = true;
    deviceNumber = 0;

    trackNames.clear();

    for (int i = 0; i < 64; i++) {
        trackNames.push_back(string("Track-" + moduru::lang::StrUtil::padLeft(to_string((int)(i + 1)), "0", 2)));
    }

    lastBar = 1;
    timeSig.setNumerator(4);
    timeSig.setDenominator(4);

    deviceNames.clear();

    deviceNames.push_back(string("        "));

    for (int i = 1; i < 33; i++) {
        deviceNames.push_back("Device" + moduru::lang::StrUtil::padLeft(to_string(i), "0", 2));
    }

    autoLoadType = 0;
    diskDevice = 0;
}

string UserDefaults::getDeviceName(int i)
{
    return deviceNames[i];
}

void UserDefaults::setDiskDevice(int i)
{
    if (i < 0 || i > 8) {
        return;
    }

    diskDevice = i;
    setChanged();
    notifyObservers(string("diskdevice"));
}

void UserDefaults::setAutoLoadType(int i)
{
    if (i < 0 || i > 3) {
        return;
    }

    autoLoadType = i;
    setChanged();
    notifyObservers(string("autoloadtype"));
}

void UserDefaults::setTempo(BCMath bd)
{
	auto str = to_string(bd.toDouble());
    
    if (str.find(".") == string::npos) {
        str += ".0";
    }

	auto length = (int)(str.find(".")) + 2;
	tempo = BCMath(str.substr(0, length));
	
    if (tempo.toDouble() < 30.0) {
        tempo = BCMath("30.0");
    }

    if (tempo.toDouble() > 300.0) {
        tempo = BCMath("300.0");
    }

	setChanged();
	notifyObservers(string("tempo"));
}

void UserDefaults::setLoop(bool b)
{
    loop = b;
    setChanged();
    notifyObservers(string("loop"));
}

BCMath UserDefaults::getTempo()
{
    return tempo;
}

int UserDefaults::getDiskDevice()
{
    return diskDevice;
}

int UserDefaults::getAutoLoadType()
{
    return autoLoadType;
}

bool UserDefaults::isRecordingModeMulti()
{
    return recordingModeMulti;
}

void UserDefaults::setBus(int i)
{
    if (i < 0 || i > 4) {
        return;
    }

    bus = i;
    setChanged();
    notifyObservers(string("tracktype"));
}

void UserDefaults::setDeviceNumber(int i)
{
    if (i < 0 || i > 33) {
        return;
    }

    deviceNumber = i;
    setChanged();
    notifyObservers(string("devicenumber"));
}

void UserDefaults::setRecordingModeMulti(bool b)
{
    recordingModeMulti = b;
    setChanged();
    notifyObservers(string("recordingmode"));
}

bool UserDefaults::isLoopEnabled()
{
    return loop;
}

int UserDefaults::getBus()
{
    return bus;
}

int UserDefaults::getDeviceNumber()
{
    return deviceNumber;
}

int UserDefaults::getPgm()
{
    return pgm;
}

int UserDefaults::getVeloRatio()
{
    return velo;
}

void UserDefaults::setLastBar(int i)
{
    if (i < 0 || i > 998) {
        return;
    }

    lastBar = i;
    setChanged();
    notifyObservers(string("bars"));
}

void UserDefaults::setPgm(int i)
{
    if (i < 0 || i > 128) {
        return;
    }

    pgm = i;
    setChanged();
    notifyObservers(string("pgm"));
}

void UserDefaults::setVelo(int i)
{
    if (i < 1) {
        i = 1;
    }

    if (i > 200) {
        i = 200;
    }

    velo = i;
    setChanged();
    notifyObservers(string("velo"));
}

string UserDefaults::getSequenceName()
{
    return sequenceName;
}

string UserDefaults::getTrackName(int i)
{
    return trackNames[i];
}

int UserDefaults::getLastBarIndex()
{
    return lastBar;
}

mpc::sequencer::TimeSignature& UserDefaults::getTimeSig()
{
    return timeSig;
}

int8_t UserDefaults::getTrackStatus()
{
    return 6;
}

void UserDefaults::setDeviceName(int i, string s)
{
    deviceNames[i] = s;
}

void UserDefaults::setSequenceName(string name)
{
    sequenceName = name;
}

void UserDefaults::setTimeSig(int num, int den)
{
    timeSig.setNumerator(num);
    timeSig.setDenominator(den);
}

void UserDefaults::setTrackName(int i, string s)
{
	trackNames[i] = s;
}
