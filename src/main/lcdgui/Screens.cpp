#include "Screens.hpp"

#include <Paths.hpp>

#include <lcdgui/Component.hpp>
#include <lcdgui/Parameter.hpp>
#include <lcdgui/Info.hpp>
#include <lcdgui/FunctionKeys.hpp>
#include <lcdgui/ScreenComponent.hpp>

#include <lcdgui/screens/SequencerScreen.hpp>
#include <lcdgui/screens/AssignScreen.hpp>
#include <lcdgui/screens/BarCopyScreen.hpp>
#include <lcdgui/screens/EditSequenceScreen.hpp>
#include <lcdgui/screens/NextSeqPadScreen.hpp>
#include <lcdgui/screens/NextSeqScreen.hpp>
#include <lcdgui/screens/SequencerScreen.hpp>
#include <lcdgui/screens/SongScreen.hpp>
#include <lcdgui/screens/StepEditorScreen.hpp>
#include <lcdgui/screens/TrMoveScreen.hpp>
#include <lcdgui/screens/TrMuteScreen.hpp>
#include <lcdgui/screens/UserScreen.hpp>
#include <lcdgui/screens/OthersScreen.hpp>
#include <lcdgui/screens/DrumScreen.hpp>
#include <lcdgui/screens/LoopScreen.hpp>
#include <lcdgui/screens/PgmAssignScreen.hpp>
#include <lcdgui/screens/PgmParamsScreen.hpp>
#include <lcdgui/screens/PurgeScreen.hpp>
#include <lcdgui/screens/SampleScreen.hpp>
#include <lcdgui/screens/SelectDrumScreen.hpp>
#include <lcdgui/screens/SndParamsScreen.hpp>
#include <lcdgui/screens/TrimScreen.hpp>
#include <lcdgui/screens/ZoneScreen.hpp>


#include <lcdgui/screens/window/InitPadAssignScreen.hpp>
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
#include <lcdgui/screens/window/Assign16LevelsScreen.hpp>
#include <lcdgui/screens/window/StepTcScreen.hpp>

#include <lcdgui/screens/dialog/MetronomeSoundScreen.hpp>
#include <lcdgui/screens/dialog/MidiMonitorScreen.hpp>
#include <lcdgui/screens/dialog/CopySequenceScreen.hpp>
#include <lcdgui/screens/dialog/CopyTrackScreen.hpp>
#include <lcdgui/screens/dialog/DeleteTrackScreen.hpp>
#include <lcdgui/screens/dialog/DeleteSequenceScreen.hpp>
#include <lcdgui/screens/dialog/DeleteAllSequencesScreen.hpp>
#include <lcdgui/screens/dialog/DeleteAllTracksScreen.hpp>

#include <file/FileUtil.hpp>

#include <rapidjson/filereadstream.h>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;

using namespace moduru::file;

using namespace rapidjson;

using namespace std;

vector<unique_ptr<Document>> Screens::layerDocuments;
map<string, shared_ptr<ScreenComponent>> Screens::screens;


vector<int> getFunctionKeyTypes(Value& functionKeyTypes)
{
	vector<int> types;
	for (int i = 0; i < 6; i++)
	{
		if (functionKeyTypes[i].IsNull())
		{
			types.push_back(-1);
		}
		else
		{
			types.push_back(functionKeyTypes[i].GetInt());
		}
	}
	return types;
}

vector<string> getFunctionKeyLabels(Value& functionKeyLabels)
{
	vector<string> labels;
	for (int i = 0; i < 6; i++)
	{
		if (functionKeyLabels[i].IsNull())
		{
			labels.push_back("");
		}
		else
		{
			labels.push_back(functionKeyLabels[i].GetString());
		}
	}
	return labels;
}

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

	vector<shared_ptr<Component>> components;

	if (arrangement.HasMember("fblabels"))
	{
		Value& functionLabels = arrangement["fblabels"];
		Value& functionTypes = arrangement["fbtypes"];


		vector<vector<string>> allLabels;
		vector<vector<int>> allTypes;

		if (!functionLabels[0].IsArray())
		{
			auto labels = getFunctionKeyLabels(functionLabels);
			auto types = getFunctionKeyTypes(functionTypes);
			allLabels.push_back(labels);
			allTypes.push_back(types);
		}
		else
		{
			for (int i = 0; i < functionLabels.Size(); i++)
			{
				auto labels = getFunctionKeyLabels(functionLabels[i]);
				auto types = getFunctionKeyTypes(functionTypes[i]);
				allLabels.push_back(labels);
				allTypes.push_back(types);
			}
		}

		auto functionKeysComponent = make_unique<FunctionKeys>("function-keys", allLabels, allTypes);
		components.push_back(move(functionKeysComponent));
	}

	if (arrangement.HasMember("labels"))
	{
		Value& x = arrangement["x"];
		Value& y = arrangement["y"];
		Value& parameters = arrangement["parameters"];
		Value& tfsize = arrangement["tfsize"];
		Value& labels = arrangement["labels"];

		for (int i = 0; i < labels.Size(); i++)
		{
			components.push_back(make_unique<Parameter>(labels[i].GetString()
				, parameters[i].GetString()
				, x[i].GetInt()
				, y[i].GetInt()
				, tfsize[i].GetInt()
				));
		}
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

	return components;
}

void Screens::init()
{
	auto path0 = string(mpc::Paths::resPath() + "mainpanel-mod.json");
	auto path1 = string(mpc::Paths::resPath() + "windowpanel-mod.json");
	auto path2 = string(mpc::Paths::resPath() + "dialogpanel-mod.json");
	auto path3 = string(mpc::Paths::resPath() + "dialog2panel-mod.json");

	vector<string> paths = { path0, path1, path2, path3 };

	for (int i = 0; i < 4; i++)
	{
		
		char readBuffer[256];

		auto fp = FileUtil::fopenw(paths[i], "r");;
		FileReadStream is(fp, readBuffer, sizeof(readBuffer));
		auto panelDoc = make_unique<Document>();
		panelDoc->ParseStream(is);
		fclose(fp);

		/*
		This can be uncommented to list all screen names that are encountered in the json files.
		for (Value::ConstMemberIterator itr = panelDoc->MemberBegin();
			itr != panelDoc->MemberEnd(); ++itr)
		{
			MLOG("\"" + string(itr->name.GetString()) + "\", ");
		}
		*/

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
	else if (screenName.compare("editmultiple") == 0)
	{
		screen = make_shared<EditMultipleScreen>(layerIndex);
	}
	else if (screenName.compare("transmitprogramchanges") == 0)
	{
		screen = make_shared<TransmitProgramChangesScreen>(layerIndex);
	}
	else if (screenName.compare("timingcorrect") == 0)
	{
		screen = make_shared<TimingCorrectScreen>(layerIndex);
	}
	else if (screenName.compare("timedisplay") == 0)
	{
		screen = make_shared<TimeDisplayScreen>(layerIndex);
	}
	else if (screenName.compare("pasteevent") == 0)
	{
		screen = make_shared<PasteEventScreen>(layerIndex);
	}
	else if (screenName.compare("multirecordingsetup") == 0)
	{
		screen = make_shared<MultiRecordingSetupScreen>(layerIndex);
	}
	else if (screenName.compare("midioutput") == 0)
	{
		screen = make_shared<MidiOutputScreen>(layerIndex);
	}
	else if (screenName.compare("midiinput") == 0)
	{
		screen = make_shared<MidiInputScreen>(layerIndex);
	}
	else if (screenName.compare("loopbarswindow") == 0)
	{
		screen = make_shared<LoopBarsScreen>(layerIndex);
	}
	else if (screenName.compare("insertevent") == 0)
	{
		screen = make_shared<InsertEventScreen>(layerIndex);
	}
	else if (screenName.compare("eraseallofftracks") == 0)
	{
		screen = make_shared<EraseAllOffTracksScreen>(layerIndex);
	}
	else if (screenName.compare("changetsig") == 0)
	{
		screen = make_shared<ChangeTsigScreen>(layerIndex);
	}
	else if (screenName.compare("editvelocity") == 0)
	{
		screen = make_shared<EditVelocityScreen>(layerIndex);
	}
	else if (screenName.compare("erase") == 0)
	{
		screen = make_shared<EraseScreen>(layerIndex);
	}
	else if (screenName.compare("changebars") == 0)
	{
		screen = make_shared<ChangeBarsScreen>(layerIndex);
	}
	else if (screenName.compare("changebars2") == 0)
	{
		screen = make_shared<ChangeBars2Screen>(layerIndex);
	}
	else if (screenName.compare("track") == 0)
	{
		screen = make_shared<TrackScreen>(layerIndex);
	}
	else if (screenName.compare("assign16levels") == 0)
	{
		screen = make_shared<Assign16LevelsScreen>(layerIndex);
	}
	else if (screenName.compare("midimonitor") == 0)
	{
		screen = make_shared<MidiMonitorScreen>(layerIndex);
	}
	else if (screenName.compare("metronomesound") == 0)
	{
		screen = make_shared<MetronomeSoundScreen>(layerIndex);
	}
	else if (screenName.compare("copysequence") == 0)
	{
		screen = make_shared<CopySequenceScreen>(layerIndex);
	}
	else if (screenName.compare("copytrack") == 0)
	{
		screen = make_shared<CopyTrackScreen>(layerIndex);
	}
	else if (screenName.compare("deletesequence") == 0)
	{
		screen = make_shared<DeleteSequenceScreen>(layerIndex);
	}
	else if (screenName.compare("deletetrack") == 0)
	{
		screen = make_shared<DeleteTrackScreen>(layerIndex);
	}
	else if (screenName.compare("deleteallsequences") == 0)
	{
		screen = make_shared<DeleteAllSequencesScreen>(layerIndex);
	}
	else if (screenName.compare("deletealltracks") == 0)
	{
		screen = make_shared<DeleteAllTracksScreen>(layerIndex);
	}
	else if (screenName.compare("nextseqpad") == 0)
	{
		screen = make_shared<NextSeqPadScreen>(layerIndex);
	}
	else if (screenName.compare("nextseq") == 0)
	{
		screen = make_shared<NextSeqScreen>(layerIndex);
	}
	else if (screenName.compare("song") == 0)
	{
		screen = make_shared<SongScreen>(layerIndex);
	}
	else if (screenName.compare("trackmute") == 0)
	{
		screen = make_shared<TrMuteScreen>(layerIndex);
	}
	else if (screenName.compare("step") == 0)
	{
		screen = make_shared<StepEditorScreen>(layerIndex);
	}
	else if (screenName.compare("edit") == 0)
	{
		screen = make_shared<EditSequenceScreen>(layerIndex);
	}
	else if (screenName.compare("barcopy") == 0)
	{
		screen = make_shared<BarCopyScreen>(layerIndex);
	}
	else if (screenName.compare("trmove") == 0)
	{
		screen = make_shared<TrMoveScreen>(layerIndex);
	}
	else if (screenName.compare("user") == 0)
	{
		screen = make_shared<UserScreen>(layerIndex);
	}
	else if (screenName.compare("assign") == 0)
	{
		screen = make_shared<AssignScreen>(layerIndex);
	}
	else if (screenName.compare("step-timing-correct") == 0)
	{
		screen = make_shared<StepTcScreen>(layerIndex);
	}
	else if (screenName.compare("others") == 0)
	{
		screen = make_shared<OthersScreen>(layerIndex);
	}
	else if (screenName.compare("purge") == 0)
	{
		screen = make_shared<PurgeScreen>(layerIndex);
	}
	else if (screenName.compare("drum") == 0)
	{
		screen = make_shared<DrumScreen>(layerIndex);
	}
	else if (screenName.compare("trim") == 0)
	{
		screen = make_shared<TrimScreen>(layerIndex);
	}
	else if (screenName.compare("loop") == 0)
	{
		screen = make_shared<LoopScreen>(layerIndex);
	}
	else if (screenName.compare("zone") == 0 || screenName.compare("numberofzones") == 0)
	{
		screen = make_shared<ZoneScreen>(layerIndex);
	}
	else if (screenName.compare("params") == 0)
	{
		screen = make_shared<SndParamsScreen>(layerIndex);
	}
	else if (screenName.compare("programparams") == 0)
	{
		screen = make_shared<PgmParamsScreen>(layerIndex);
	}
	else if (screenName.compare("programassign") == 0)
	{
		screen = make_shared<PgmAssignScreen>(layerIndex);
	}
	else if (screenName.compare("selectdrum") == 0)
	{
		screen = make_shared<SelectDrumScreen>(layerIndex);
	}

	if (screen)
	{
		screen->addChildren(children);
		screens[screenName] = screen;
	}

	return screen;
}
