#include "UserScreen.hpp"

#include <sequencer/TimeSignature.hpp>

#include <nvram/NvRam.hpp>

#include <Util.hpp>

using namespace mpc::lcdgui::screens;
using namespace moduru::lang;
using namespace std;

UserScreen::UserScreen(const int layerIndex) 
	: ScreenComponent("user", layerIndex)
{
	resetPreferences();
}

void UserScreen::open()
{
	displayTempo();
	displayLoop();
	displayTsig();
	displayBars();
	displayPgm();
	displayRecordingMode();
	displayTrackType();
	displayDeviceNumber();
	displayVelo();
}

void UserScreen::function(int i)
{
    init();
	
	switch (i)
	{
    case 0:
        ls.lock()->openScreen("edit");
        break;
    case 1:
        ls.lock()->openScreen("bar-copy");
        break;
    case 2:
        ls.lock()->openScreen("tr-move");
        break;
    }

}

void UserScreen::turnWheel(int i)
{
	init();
	
	if (param.compare("tempo") == 0)
	{
		double oldTempo = tempo.toDouble();
		double newTempo = oldTempo + (i * 0.1);
		setTempo(BCMath(newTempo));
	}
	else if (param.compare("loop") == 0)
	{
		setLoop(i > 0);
	}
	else if (param.compare("tsig") == 0)
	{
		if (i > 0)
		{
			timeSig.increase();
		}
		else
		{
			timeSig.decrease();
		}
	}
	else if (param.compare("bars") == 0)
	{
		setLastBar(lastBar + i);
	}
	else if (param.compare("pgm") == 0)
	{
		setPgm(pgm + i);
	}
	else if (param.compare("recordingmode") == 0)
	{
		setRecordingModeMulti(i > 0);
	}
	else if (param.compare("tracktype") == 0)
	{
		setBus(bus + i);
	}
	else if (param.compare("device") == 0)
	{
		setDeviceNumber(device + i);
	}
	else if (param.compare("velo") == 0)
	{
		setVelo(velo + i);
	}
}

void UserScreen::displayTempo()
{
	auto tempoStr = StrUtil::padLeft(tempo.toString(), " ", 5);
	tempoStr = mpc::Util::replaceDotWithSmallSpaceDot(tempoStr);
	findField("tempo").lock()->setText(tempoStr);
}

void UserScreen::displayLoop()
{
	findField("loop").lock()->setText(loop ? "ON" : "OFF");
}

void UserScreen::displayTsig()
{
	auto numerator = to_string(timeSig.getNumerator());
	auto denominator = to_string(timeSig.getDenominator());
	findField("tsig").lock()->setText(numerator + "/" + denominator);
}

void UserScreen::displayBars()
{
	findField("bars").lock()->setText(to_string(lastBar + 1));
}

void UserScreen::displayPgm()
{
	if (pgm == 0)
	{
		findField("pgm").lock()->setText("OFF");
	}
	else
	{
		findField("pgm").lock()->setText(to_string(pgm));
	}
}

void UserScreen::displayRecordingMode()
{
	findField("recordingmode").lock()->setText(recordingModeMulti ? "M" : "S");
}

void UserScreen::displayTrackType()
{
	findField("tracktype").lock()->setText(busNames[bus]);
	displayDeviceName();
}

void UserScreen::displayDeviceNumber()
{
	if (device == 0)
	{
		findField("device").lock()->setText("OFF");
		return;
	}

	if (device >= 17)
	{
		findField("device").lock()->setTextPadded(to_string(device - 16) + "B", " ");
	}
	else
	{
		findField("device").lock()->setTextPadded(to_string(device) + "A", " ");
	}
}

void UserScreen::displayVelo()
{
	findField("velo").lock()->setText(to_string(velo));
}

void UserScreen::displayDeviceName()
{
	init();

	if (bus == 0)
	{
		if (device != 0)
		{
			findLabel("devicename").lock()->setText(getDeviceName(device));
		}
		else
		{
			findLabel("devicename").lock()->setText("");
		}
	}
	else
	{
		if (device == 0)
		{
			findLabel("devicename").lock()->setText(program.lock()->getName());
		}
		else
		{
			findLabel("devicename").lock()->setText(getDeviceName(device));
		}
	}
}

void UserScreen::resetPreferences()
{
	sequenceName = string("Sequence");
	bus = 1;
	tempo = BCMath("120.0");
	velo = 100;
	pgm = 0;
	recordingModeMulti = false;
	loop = true;
	device = 0;

	trackNames.clear();

	for (int i = 0; i < 64; i++)
	{
		trackNames.push_back(string("Track-" + moduru::lang::StrUtil::padLeft(to_string((int)(i + 1)), "0", 2)));
	}

	lastBar = 1;
	timeSig.setNumerator(4);
	timeSig.setDenominator(4);

	deviceNames.clear();

	deviceNames.push_back(string("        "));

	for (int i = 1; i < 33; i++)
	{
		deviceNames.push_back("Device" + moduru::lang::StrUtil::padLeft(to_string(i), "0", 2));
	}
}

string UserScreen::getDeviceName(int i)
{
	return deviceNames[i];
}

void UserScreen::setTempo(BCMath bd)
{
	auto str = to_string(bd.toDouble());

	if (str.find(".") == string::npos)
	{
		str += ".0";
	}

	auto length = (int)(str.find(".")) + 2;
	tempo = BCMath(str.substr(0, length));

	if (tempo.toDouble() < 30.0) {
		tempo = BCMath("30.0");
	}
	else if (tempo.toDouble() > 300.0) {
		tempo = BCMath("300.0");
	}

	displayTempo();
}

void UserScreen::setLoop(bool b)
{
	if (loop == b)
	{
		return;
	}

	loop = b;
	displayLoop();
}

void UserScreen::setBus(int i)
{
	if (i < 0 || i > 4)
	{
		return;
	}

	bus = i;
	displayTrackType();
}

void UserScreen::setDeviceNumber(int i)
{
	if (i < 0 || i > 33)
	{
		return;
	}

	device = i;
	displayDeviceNumber();
	displayDeviceName();
}

void UserScreen::setRecordingModeMulti(bool b)
{
	if (recordingModeMulti == b)
	{
		return;
	}

	recordingModeMulti = b;
	displayRecordingMode();
}

void UserScreen::setLastBar(int i)
{
	if (i < 0 || i > 998)
	{
		return;
	}

	lastBar = i;
	displayBars();
}

void UserScreen::setPgm(int i)
{
	if (i < 0 || i > 128)
	{
		return;
	}

	pgm = i;
	displayPgm();
}

void UserScreen::setVelo(int i)
{
	if (i < 1)
	{
		i = 1;
	}
	else if (i > 200)
	{
		i = 200;
	}

	velo = i;
	displayVelo();
}

string UserScreen::getTrackName(int i)
{
	return trackNames[i];
}

int8_t UserScreen::getTrackStatus()
{
	// This number is so magic that I forgot what it's for.
	return 6;
}

void UserScreen::setDeviceName(int i, string s)
{
	deviceNames[i] = s;
}

void UserScreen::setSequenceName(string name)
{
	sequenceName = name;
}

void UserScreen::setTimeSig(int num, int den)
{
	timeSig.setNumerator(num);
	timeSig.setDenominator(den);
}

void UserScreen::setTrackName(int i, string s)
{
	trackNames[i] = s;
}
