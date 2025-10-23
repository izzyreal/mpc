#include "UserScreen.hpp"

#include <lcdgui/screens/EventsScreen.hpp>

#include <nvram/NvRam.hpp>

#include <Util.hpp>

using namespace mpc::lcdgui::screens;

UserScreen::UserScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "user", layerIndex)
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
	displayBus();
	displayDeviceNumber();
	displayVelo();
}

void UserScreen::function(int i)
{
    init();
	
	switch (i)
	{
		// Intentional fall-through
		case 0:
		case 1:
		case 2:
		{
			auto eventsScreen = mpc.screens->get<EventsScreen>();
			eventsScreen->tab = i;
			mpc.getLayeredScreen()->openScreen(eventsScreen->tabNames[eventsScreen->tab]);
			break;
		}
    }

}

void UserScreen::turnWheel(int i)
{
	init();
	
    const auto focusedFieldName = getFocusedFieldName();

	if (focusedFieldName == "tempo")
	{
		double newTempo = tempo + (i * 0.1);
		setTempo(newTempo);
	}
	else if (focusedFieldName == "loop")
	{
		setLoop(i > 0);
	}
	else if (focusedFieldName == "tsig")
	{
		if (i > 0)
		{
			timeSig.increase();
		}
		else
		{
			timeSig.decrease();
		}
	
		displayTsig();
	}
	else if (focusedFieldName == "bars")
	{
		setLastBar(lastBar + i);
	}
	else if (focusedFieldName == "pgm")
	{
		setPgm(pgm + i);
	}
	else if (focusedFieldName == "recordingmode")
	{
		setRecordingModeMulti(i > 0);
	}
	else if (focusedFieldName == "bus")
	{
		setBus(bus + i);
	}
	else if (focusedFieldName == "device")
	{
		setDeviceNumber(device + i);
	}
	else if (focusedFieldName == "velo")
	{
		setVelo(velo + i);
	}
}

void UserScreen::displayTempo()
{
	findField("tempo")->setText(Util::tempoString(tempo));
}

void UserScreen::displayLoop()
{
	findField("loop")->setText(loop ? "ON" : "OFF");
}

void UserScreen::displayTsig()
{
	auto numerator = std::to_string(timeSig.getNumerator());
	auto denominator = std::to_string(timeSig.getDenominator());
	findField("tsig")->setText(numerator + "/" + denominator);
}

void UserScreen::displayBars()
{
	findField("bars")->setText(std::to_string(lastBar + 1));
}

void UserScreen::displayPgm()
{
	if (pgm == 0)
	{
		findField("pgm")->setText("OFF");
	}
	else
	{
		findField("pgm")->setText(std::to_string(pgm));
	}
}

void UserScreen::displayRecordingMode()
{
	findField("recordingmode")->setText(recordingModeMulti ? "M" : "S");
}

void UserScreen::displayBus()
{
	findField("bus")->setText(busNames[bus]);
	displayDeviceName();
}

void UserScreen::displayDeviceNumber()
{
	if (device == 0)
	{
		findField("device")->setText("OFF");
		return;
	}

	if (device >= 17)
	{
		findField("device")->setTextPadded(std::to_string(device - 16) + "B", " ");
	}
	else
	{
		findField("device")->setTextPadded(std::to_string(device) + "A", " ");
	}
}

void UserScreen::displayVelo()
{
	findField("velo")->setText(std::to_string(velo));
}

void UserScreen::displayDeviceName()
{
	init();

	if (bus == 0)
	{
		if (device != 0)
		{
			findLabel("devicename")->setText(getDeviceName(device));
		}
		else
		{
			findLabel("devicename")->setText("");
		}
	}
	else
	{
		if (device == 0)
		{
			auto programName = sampler->getProgram(mpc.getDrum(bus - 1).getProgram())->getName();
			findLabel("devicename")->setText(programName);
		}
		else
		{
			findLabel("devicename")->setText(getDeviceName(device));
		}
	}
}

void UserScreen::resetPreferences()
{
	sequenceName = std::string("Sequence");
	bus = 1;
	tempo = 120.0;
	velo = 100;
	pgm = 0;
	recordingModeMulti = false;
	loop = true;
	device = 0;

	trackNames.clear();

	for (int i = 0; i < 64; i++)
	{
		trackNames.push_back(std::string("Track-" + StrUtil::padLeft(std::to_string((int)(i + 1)), "0", 2)));
	}

	lastBar = 1;
	timeSig.setNumerator(4);
	timeSig.setDenominator(4);

	deviceNames.clear();

	deviceNames.push_back(std::string("        "));

	for (int i = 1; i < 33; i++)
	{
		deviceNames.push_back("Device" + StrUtil::padLeft(std::to_string(i), "0", 2));
	}
}

std::string UserScreen::getDeviceName(int i)
{
	return deviceNames[i];
}

void UserScreen::setTempo(const double newTempo)
{
	auto str = std::to_string(newTempo);

	if (newTempo < 30.0)
	{
		tempo = 30.0;
	}
	else if (newTempo > 300.0) {
		tempo = 300.0;
	}
	else
	{
		tempo = newTempo;
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

	displayBus();
	displayDeviceName();
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

std::string UserScreen::getTrackName(int i)
{
	return trackNames[i];
}

int8_t UserScreen::getTrackStatus()
{
	// This number is so magic that I forgot what it's for.
	return 6;
}

void UserScreen::setDeviceName(int i, std::string s)
{
	deviceNames[i] = s;
}

void UserScreen::setSequenceName(std::string name)
{
	sequenceName = name;
}

void UserScreen::setTimeSig(int num, int den)
{
	timeSig.setNumerator(num);
	timeSig.setDenominator(den);
}

void UserScreen::setTrackName(int i, std::string s)
{
	trackNames[i] = s;
}
