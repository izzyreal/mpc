#include "Screens.hpp"

#include <Paths.hpp>

#include <lcdgui/Component.hpp>
#include <lcdgui/Parameter.hpp>
#include <lcdgui/Info.hpp>
#include <lcdgui/FunctionKeys.hpp>
#include <lcdgui/ScreenComponent.hpp>

#include <lcdgui/screens/SequencerScreen.hpp>
#include <lcdgui/screens/window/SequenceScreen.hpp>
#include <lcdgui/screens/window/TempoChangeScreen.hpp>
#include <lcdgui/screens/window/CountMetronomeScreen.hpp>
#include <lcdgui/screens/window/EditMultipleScreen.hpp>
#include <lcdgui/screens/window/TransmitProgramChangesScreen.hpp>
#include <lcdgui/screens/window/TimingCorrectScreen.hpp>
#include <lcdgui/screens/window/TimeDisplayScreen.hpp>
#include <lcdgui/screens/window/PasteEventScreen.hpp>
#include <lcdgui/screens/window/MultiRecordingSetupScreen.hpp>
#include <lcdgui/screens/window/MidiOutputScreen.hpp>
#include <lcdgui/screens/window/MidiInputScreen.hpp>
#include <lcdgui/screens/window/LoopBarsScreen.hpp>
#include <lcdgui/screens/window/InsertEventScreen.hpp>
#include <lcdgui/screens/window/EraseAllOffTracksScreen.hpp>
#include <lcdgui/screens/window/ChangeTsigScreen.hpp>
#include <lcdgui/screens/window/EditVelocityScreen.hpp>
#include <lcdgui/screens/window/EraseScreen.hpp>
#include <lcdgui/screens/window/ChangeBarsScreen.hpp>
#include <lcdgui/screens/window/ChangeBars2Screen.hpp>
#include <lcdgui/screens/window/TrackScreen.hpp>
#include <lcdgui/screens/window/MidiMonitorScreen.hpp>
#include <lcdgui/screens/window/Assign16LevelsScreen.hpp>

#include <file/FileUtil.hpp>

#include <rapidjson/filereadstream.h>


using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

using namespace moduru::file;

using namespace rapidjson;

using namespace std;

vector<unique_ptr<Document>> Screens::layerDocuments;
map<string, shared_ptr<ScreenComponent>> Screens::screens;

vector<shared_ptr<Component>> Screens::get(const string& screenName, int& foundInLayer)
{
	
	if (Screens::layerDocuments.empty())
	{
		init();
	}

	for (int i = 0; i < 4; i++)
	{
		if (layerDocuments[i]->HasMember(screenName.c_str()))
		{
			foundInLayer = i;
			break;
		}
	}

	if (foundInLayer < 0)
	{
		return {};
	}

	Value& arrangement = layerDocuments[foundInLayer]->GetObject()[screenName.c_str()];

	Value& labels = arrangement["labels"];
	Value& x = arrangement["x"];
	Value& y = arrangement["y"];
	Value& parameters = arrangement["parameters"];
	Value& tfsize = arrangement["tfsize"];

	vector<shared_ptr<Component>> components;

	for (int i = 0; i < labels.Size(); i++)
	{
		components.push_back(make_unique<Parameter>(labels[i].GetString()
			, parameters[i].GetString()
			, x[i].GetInt()
			, y[i].GetInt()
			, tfsize[i].GetInt()
			));
	}

	if (arrangement.HasMember("infowidgets"))
	{
		
		Value& infoNames = arrangement["infowidgets"];
		Value& infoSize = arrangement["infosize"];
		Value& infoX = arrangement["infox"];
		Value& infoY = arrangement["infoy"];
	
		for (int i = 0; i < infoNames.Size(); i++)
		{
			components.push_back(make_shared<Label>(infoNames[i].GetString()
				, ""
				, infoX[i].GetInt()
				, infoY[i].GetInt()
				, infoSize[i].GetInt()));
		}
	}

	auto functionKeysComponent = make_unique<FunctionKeys>();

	if (arrangement.HasMember("fblabels"))
	{
		Value& fklabels = arrangement["fblabels"];
		Value& fktypes = arrangement["fbtypes"];
		functionKeysComponent->Hide(false);
		functionKeysComponent->initialize(fklabels, fktypes);
	}
	else
	{
		functionKeysComponent->Hide(true);
	}

	components.push_back(move(functionKeysComponent));

	return components;
}

void Screens::init()
{
	auto path0 = string(mpc::Paths::resPath() + "mainpanel-mod.json");
	auto path1 = string(mpc::Paths::resPath() + "windowpanel-mod.json");
	auto path2 = string(mpc::Paths::resPath() + "dialogpanel.json");
	auto path3 = string(mpc::Paths::resPath() + "dialog2panel.json");

	vector<string> paths = { path0, path1, path2, path3 };

	for (int i = 0; i < 4; i++)
	{
		
		char readBuffer[256];

		auto fp = FileUtil::fopenw(paths[i], "r");;
		FileReadStream is(fp, readBuffer, sizeof(readBuffer));
		auto panelDoc = make_unique<Document>();
		panelDoc->ParseStream(is);

		fclose(fp);

		layerDocuments.push_back(move(panelDoc));
	}
}

shared_ptr<ScreenComponent> Screens::getScreenComponent(const string& screenName)
{
	auto candidate = screens[screenName];

	if (candidate)
	{
		return candidate;
	}

	shared_ptr<ScreenComponent> screen;

	int layerIndex = -1;
	auto children = get(screenName, layerIndex);

	if (screenName.compare("sequencer") == 0)
	{
		screen = make_shared<SequencerScreen>(layerIndex);
	}
	else if (screenName.compare("sequence") == 0)
	{
		screen = make_shared<SequenceScreen>(layerIndex);
	}
	else if (screenName.compare("tempochange") == 0)
	{
		screen = make_shared<TempoChangeScreen>(layerIndex);
	}
	else if (screenName.compare("countmetronome") == 0)
	{
		screen = make_shared<CountMetronomeScreen>(layerIndex);
	}

	else if (screenName.compare("countmetronome") == 0)
	{
		screen = make_shared<EditMultipleScreen>(layerIndex);
	}
	else if (screenName.compare("countmetronome") == 0)
	{
		screen = make_shared<TransmitProgramChangesScreen>(layerIndex);
	}
	else if (screenName.compare("countmetronome") == 0)
	{
		screen = make_shared<TimingCorrectScreen>(layerIndex);
	}
	else if (screenName.compare("countmetronome") == 0)
	{
		screen = make_shared<TimeDisplayScreen>(layerIndex);
	}
	else if (screenName.compare("countmetronome") == 0)
	{
		screen = make_shared<PasteEventScreen>(layerIndex);
	}
	else if (screenName.compare("countmetronome") == 0)
	{
		screen = make_shared<MultiRecordingSetupScreen>(layerIndex);
	}
	else if (screenName.compare("countmetronome") == 0)
	{
		screen = make_shared<MidiOutputScreen>(layerIndex);
	}
	else if (screenName.compare("countmetronome") == 0)
	{
		screen = make_shared<MidiInputScreen>(layerIndex);
	}
	else if (screenName.compare("countmetronome") == 0)
	{
		screen = make_shared<LoopBarsScreen>(layerIndex);
	}
	else if (screenName.compare("countmetronome") == 0)
	{
		screen = make_shared<InsertEventScreen>(layerIndex);
	}
	else if (screenName.compare("countmetronome") == 0)
	{
		screen = make_shared<EraseAllOffTracksScreen>(layerIndex);
	}
	else if (screenName.compare("countmetronome") == 0)
	{
		screen = make_shared<ChangeTsigScreen>(layerIndex);
	}
	else if (screenName.compare("countmetronome") == 0)
	{
		screen = make_shared<EditVelocityScreen>(layerIndex);
	}
	else if (screenName.compare("countmetronome") == 0)
	{
		screen = make_shared<EraseScreen>(layerIndex);
	}
	else if (screenName.compare("countmetronome") == 0)
	{
		screen = make_shared<ChangeBarsScreen>(layerIndex);
	}
	else if (screenName.compare("countmetronome") == 0)
	{
		screen = make_shared<ChangeBars2Screen>(layerIndex);
	}
	else if (screenName.compare("countmetronome") == 0)
	{
		screen = make_shared<TrackScreen>(layerIndex);
	}
	else if (screenName.compare("countmetronome") == 0)
	{
		screen = make_shared<MidiMonitorScreen>(layerIndex);
	}
	else if (screenName.compare("countmetronome") == 0)
	{
		screen = make_shared<Assign16LevelsScreen>(layerIndex);
	}

	if (screen)
	{
		screen->addChildren(children);
		screens[screenName] = screen;
	}

	return screen;
}
