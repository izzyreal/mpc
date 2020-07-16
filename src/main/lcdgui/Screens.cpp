#include "Screens.hpp"

#include <Paths.hpp>

#include <lcdgui/Component.hpp>
#include <lcdgui/Parameter.hpp>
#include <lcdgui/Info.hpp>
#include <lcdgui/FunctionKeys.hpp>
#include <lcdgui/ScreenComponent.hpp>

#include <lcdgui/screens/SequencerScreen.hpp>
#include <lcdgui/screens/AssignScreen.hpp>
#include <lcdgui/screens/BarsScreen.hpp>
#include <lcdgui/screens/EventsScreen.hpp>
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
#include <lcdgui/screens/MixerScreen.hpp>
#include <lcdgui/screens/MixerSetupScreen.hpp>
#include <lcdgui/screens/FxEditScreen.hpp>
#include <lcdgui/screens/SelectMixerDrumScreen.hpp>
#include <lcdgui/screens/LoadScreen.hpp>
#include <lcdgui/screens/FormatScreen.hpp>
#include <lcdgui/screens/SetupScreen.hpp>
#include <lcdgui/screens/SaveScreen.hpp>
#include <lcdgui/screens/VmpcDiskScreen.hpp>
#include <lcdgui/screens/VmpcSettingsScreen.hpp>
#include <lcdgui/screens/VerScreen.hpp>
#include <lcdgui/screens/InitScreen.hpp>
#include <lcdgui/screens/SecondSeqScreen.hpp>
#include <lcdgui/screens/TransScreen.hpp>
#include <lcdgui/screens/PunchScreen.hpp>
#include <lcdgui/screens/SyncScreen.hpp>

#include <lcdgui/screens/window/NumberOfZonesScreen.hpp>
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
#include <lcdgui/screens/window/SoundScreen.hpp>
#include <lcdgui/screens/window/StartFineScreen.hpp>
#include <lcdgui/screens/window/EndFineScreen.hpp>
#include <lcdgui/screens/window/LoopToFineScreen.hpp>
#include <lcdgui/screens/window/LoopEndFineScreen.hpp>
#include <lcdgui/screens/window/ZoneStartFineScreen.hpp>
#include <lcdgui/screens/window/ZoneEndFineScreen.hpp>
#include <lcdgui/screens/window/ChannelSettingsScreen.hpp>
#include <lcdgui/screens/window/EditSoundScreen.hpp>
#include <lcdgui/screens/window/AssignmentViewScreen.hpp>
#include <lcdgui/screens/window/AutoChromaticAssignmentScreen.hpp>
#include <lcdgui/screens/window/CopyNoteParametersScreen.hpp>
#include <lcdgui/screens/window/KeepOrRetryScreen.hpp>
#include <lcdgui/screens/window/MuteAssignScreen.hpp>
#include <lcdgui/screens/window/ProgramScreen.hpp>
#include <lcdgui/screens/window/VelocityModulationScreen.hpp>
#include <lcdgui/screens/window/VeloEnvFilterScreen.hpp>
#include <lcdgui/screens/window/VeloPitchScreen.hpp>
#include <lcdgui/screens/window/DirectoryScreen.hpp>
#include <lcdgui/screens/window/LoadAProgramScreen.hpp>
#include <lcdgui/screens/window/LoadASequenceScreen.hpp>
#include <lcdgui/screens/window/LoadASequenceFromAllScreen.hpp>
#include <lcdgui/screens/window/LoadASoundScreen.hpp>
#include <lcdgui/screens/window/LoadApsFileScreen.hpp>
#include <lcdgui/screens/window/Mpc2000XlAllFileScreen.hpp>
#include <lcdgui/screens/window/SaveAProgramScreen.hpp>
#include <lcdgui/screens/window/SaveASequenceScreen.hpp>
#include <lcdgui/screens/window/SaveASoundScreen.hpp>
#include <lcdgui/screens/window/SaveAllFileScreen.hpp>
#include <lcdgui/screens/window/SaveApsFileScreen.hpp>
#include <lcdgui/screens/window/CantFindFileScreen.hpp>
#include <lcdgui/screens/window/NameScreen.hpp>
#include <lcdgui/screens/window/VmpcDirectToDiskRecorderScreen.hpp>
#include <lcdgui/screens/window/VmpcRecordingFinishedScreen.hpp>
#include <lcdgui/screens/window/TransposePermanentScreen.hpp>

#include <lcdgui/screens/dialog/MetronomeSoundScreen.hpp>
#include <lcdgui/screens/dialog/MidiMonitorScreen.hpp>
#include <lcdgui/screens/dialog/CopySequenceScreen.hpp>
#include <lcdgui/screens/dialog/CopyTrackScreen.hpp>
#include <lcdgui/screens/dialog/DeleteTrackScreen.hpp>
#include <lcdgui/screens/dialog/DeleteSequenceScreen.hpp>
#include <lcdgui/screens/dialog/DeleteAllSequencesScreen.hpp>
#include <lcdgui/screens/dialog/DeleteAllTracksScreen.hpp>
#include <lcdgui/screens/dialog/ConvertSoundScreen.hpp>
#include <lcdgui/screens/dialog/CopySoundScreen.hpp>
#include <lcdgui/screens/dialog/DeleteSoundScreen.hpp>
#include <lcdgui/screens/dialog/MonoToStereoScreen.hpp>
#include <lcdgui/screens/dialog/ResampleScreen.hpp>
#include <lcdgui/screens/dialog/StereoToMonoScreen.hpp>
#include <lcdgui/screens/dialog/CreateNewProgramScreen.hpp>
#include <lcdgui/screens/dialog/CopyProgramScreen.hpp>
#include <lcdgui/screens/dialog/DeleteAllProgramsScreen.hpp>
#include <lcdgui/screens/dialog/DeleteAllSoundScreen.hpp>
#include <lcdgui/screens/dialog/DeleteProgramScreen.hpp>
#include <lcdgui/screens/dialog/DeleteFileScreen.hpp>
#include <lcdgui/screens/dialog/DeleteFolderScreen.hpp>
#include <lcdgui/screens/dialog/FileAlreadyExistsScreen.hpp>
#include <lcdgui/screens/dialog/VmpcRecordJamScreen.hpp>

#include <lcdgui/screens/dialog2/DeleteAllFilesScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>

#include <file/FileUtil.hpp>
#include <lang/StrUtil.hpp>

#include <rapidjson/filereadstream.h>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::dialog2;
using namespace moduru::file;
using namespace moduru::lang;
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

pair<vector<shared_ptr<Component>>, map<string, vector<string>>> Screens::get(const string& screenName, int& foundInLayer, string& firstField)
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

	map<string, vector<string>> transferMap;

	if (arrangement.HasMember("parameters"))
	{
		Value& parameters = arrangement["parameters"];
		Value& labels = arrangement["labels"];
		Value& x = arrangement["x"];
		Value& y = arrangement["y"];
		Value& tfsize = arrangement["tfsize"];

		vector<string> row;

		int skipCounter = 0;

		string previous = "";
		for (int i = 0; i < parameters.Size(); i++)
		{
			if (parameters[i].IsArray())
			{
				vector<string> parameterTransferMap;
				for (int j = 0; j < parameters[i].Size(); j++)
				{
					parameterTransferMap.push_back(parameters[i][j].GetString());
				}
				transferMap[previous] = parameterTransferMap;
				skipCounter++;
				continue;
			}

			auto parameterName = string(parameters[i].GetString());
			auto parameterNames = StrUtil::split(parameterName, ',');
			string nextFocus = "_";

			if (parameterNames.size() > 1)
			{
				parameterName = parameterNames[0];
				nextFocus = parameterNames[1];
			}

			row.push_back(parameterName);

			components.push_back(
				make_unique<Parameter>(labels[i - skipCounter].GetString(),
				parameterName,
				x[i - skipCounter].GetInt(),
				y[i - skipCounter].GetInt(),
				tfsize[i - skipCounter].GetInt()
				));

			auto parameter = components.back();
			auto field = parameter->findField(parameterName).lock();

			if (i == 0)
			{
				firstField = parameterName;
			}

			field->setNextFocus(nextFocus);

			previous = parameterName;
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

	return { components, transferMap };
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
	string firstField = "";
	auto arrangement = get(screenName, layerIndex, firstField);
	auto children = arrangement.first;
	auto transferMap = arrangement.second;

	if (screenName.compare("sequencer") == 0)
	{
		screen = make_shared<SequencerScreen>(layerIndex);
	}
	else if (screenName.compare("sequence") == 0)
	{
		screen = make_shared<SequenceScreen>(layerIndex);
	}
	else if (screenName.compare("tempo-change") == 0)
	{
		screen = make_shared<TempoChangeScreen>(layerIndex);
	}
	else if (screenName.compare("count-metronome") == 0)
	{
		screen = make_shared<CountMetronomeScreen>(layerIndex);
	}
	else if (screenName.compare("edit-multiple") == 0)
	{
		screen = make_shared<EditMultipleScreen>(layerIndex);
	}
	else if (screenName.compare("transmit-program-changes") == 0)
	{
		screen = make_shared<TransmitProgramChangesScreen>(layerIndex);
	}
	else if (screenName.compare("timing-correct") == 0)
	{
		screen = make_shared<TimingCorrectScreen>(layerIndex);
	}
	else if (screenName.compare("time-display") == 0)
	{
		screen = make_shared<TimeDisplayScreen>(layerIndex);
	}
	else if (screenName.compare("paste-event") == 0)
	{
		screen = make_shared<PasteEventScreen>(layerIndex);
	}
	else if (screenName.compare("multi-recording-setup") == 0)
	{
		screen = make_shared<MultiRecordingSetupScreen>(layerIndex);
	}
	else if (screenName.compare("midi-output") == 0)
	{
		screen = make_shared<MidiOutputScreen>(layerIndex);
	}
	else if (screenName.compare("midi-input") == 0)
	{
		screen = make_shared<MidiInputScreen>(layerIndex);
	}
	else if (screenName.compare("loop-bars-window") == 0)
	{
		screen = make_shared<LoopBarsScreen>(layerIndex);
	}
	else if (screenName.compare("insert-event") == 0)
	{
		screen = make_shared<InsertEventScreen>(layerIndex);
	}
	else if (screenName.compare("erase-all-off-tracks") == 0)
	{
		screen = make_shared<EraseAllOffTracksScreen>(layerIndex);
	}
	else if (screenName.compare("change-tsig") == 0)
	{
		screen = make_shared<ChangeTsigScreen>(layerIndex);
	}
	else if (screenName.compare("edit-velocity") == 0)
	{
		screen = make_shared<EditVelocityScreen>(layerIndex);
	}
	else if (screenName.compare("erase") == 0)
	{
		screen = make_shared<EraseScreen>(layerIndex);
	}
	else if (screenName.compare("change-bars") == 0)
	{
		screen = make_shared<ChangeBarsScreen>(layerIndex);
	}
	else if (screenName.compare("change-bars-2") == 0)
	{
		screen = make_shared<ChangeBars2Screen>(layerIndex);
	}
	else if (screenName.compare("track") == 0)
	{
		screen = make_shared<TrackScreen>(layerIndex);
	}
	else if (screenName.compare("assign-16-levels") == 0)
	{
		screen = make_shared<Assign16LevelsScreen>(layerIndex);
	}
	else if (screenName.compare("midi-monitor") == 0)
	{
		screen = make_shared<MidiMonitorScreen>(layerIndex);
	}
	else if (screenName.compare("metronome-sound") == 0)
	{
		screen = make_shared<MetronomeSoundScreen>(layerIndex);
	}
	else if (screenName.compare("copy-sequence") == 0)
	{
		screen = make_shared<CopySequenceScreen>(layerIndex);
	}
	else if (screenName.compare("copy-track") == 0)
	{
		screen = make_shared<CopyTrackScreen>(layerIndex);
	}
	else if (screenName.compare("delete-sequence") == 0)
	{
		screen = make_shared<DeleteSequenceScreen>(layerIndex);
	}
	else if (screenName.compare("delete-track") == 0)
	{
		screen = make_shared<DeleteTrackScreen>(layerIndex);
	}
	else if (screenName.compare("delete-all-sequences") == 0)
	{
		screen = make_shared<DeleteAllSequencesScreen>(layerIndex);
	}
	else if (screenName.compare("delete-all-tracks") == 0)
	{
		screen = make_shared<DeleteAllTracksScreen>(layerIndex);
	}
	else if (screenName.compare("next-seq-pad") == 0)
	{
		screen = make_shared<NextSeqPadScreen>(layerIndex);
	}
	else if (screenName.compare("next-seq") == 0)
	{
		screen = make_shared<NextSeqScreen>(layerIndex);
	}
	else if (screenName.compare("song") == 0)
	{
		screen = make_shared<SongScreen>(layerIndex);
	}
	else if (screenName.compare("track-mute") == 0)
	{
		screen = make_shared<TrMuteScreen>(layerIndex);
	}
	else if (screenName.compare("step-editor") == 0)
	{
		screen = make_shared<StepEditorScreen>(layerIndex);
	}
	else if (screenName.compare("events") == 0)
	{
		screen = make_shared<EventsScreen>(layerIndex);
	}
	else if (screenName.compare("bars") == 0)
	{
		screen = make_shared<BarsScreen>(layerIndex);
	}
	else if (screenName.compare("tr-move") == 0)
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
	else if (screenName.compare("zone") == 0)
	{
		screen = make_shared<ZoneScreen>(layerIndex);
	}
	else if (screenName.compare("number-of-zones") == 0)
	{
		screen = make_shared<NumberOfZonesScreen>(layerIndex);
	}
	else if (screenName.compare("params") == 0)
	{
		screen = make_shared<SndParamsScreen>(layerIndex);
	}
	else if (screenName.compare("program-params") == 0)
	{
		screen = make_shared<PgmParamsScreen>(layerIndex);
	}
	else if (screenName.compare("program-assign") == 0)
	{
		screen = make_shared<PgmAssignScreen>(layerIndex);
	}
	else if (screenName.compare("select-drum") == 0)
	{
		screen = make_shared<SelectDrumScreen>(layerIndex);
	}
	else if (screenName.compare("sample") == 0)
	{
		screen = make_shared<SampleScreen>(layerIndex);
	}
	else if (screenName.compare("sound") == 0)
	{
		screen = make_shared<SoundScreen>(layerIndex);
	}
	else if (screenName.compare("convert-sound") == 0)
	{
		screen = make_shared<ConvertSoundScreen>(layerIndex);
	}
	else if (screenName.compare("copy-sound") == 0)
	{
		screen = make_shared<CopySoundScreen>(layerIndex);
	}
	else if (screenName.compare("delete-sound") == 0)
	{
		screen = make_shared<DeleteSoundScreen>(layerIndex);
	}
	else if (screenName.compare("mono-to-stereo") == 0)
	{
		screen = make_shared<MonoToStereoScreen>(layerIndex);
	}
	else if (screenName.compare("resample") == 0)
	{
		screen = make_shared<ResampleScreen>(layerIndex);
	}
	else if (screenName.compare("stereo-to-mono") == 0)
	{
		screen = make_shared<StereoToMonoScreen>(layerIndex);
	}
	else if (screenName.compare("mixer") == 0)
	{
		screen = make_shared<MixerScreen>(layerIndex);
	}
	else if (screenName.compare("mixer-setup") == 0)
	{
		screen = make_shared<MixerSetupScreen>(layerIndex);
	}
	else if (screenName.compare("select-mixer-drum") == 0)
	{
		screen = make_shared<SelectMixerDrumScreen>(layerIndex);
	}
	else if (screenName.compare("fx-edit") == 0)
	{
		screen = make_shared<FxEditScreen>(layerIndex);
	}
	else if (screenName.compare("start-fine") == 0)
	{
		screen = make_shared<StartFineScreen>(layerIndex);
	}
	else if (screenName.compare("end-fine") == 0)
	{
		screen = make_shared<EndFineScreen>(layerIndex);
	}
	else if (screenName.compare("loop-to-fine") == 0)
	{
		screen = make_shared<LoopToFineScreen>(layerIndex);
	}
	else if (screenName.compare("loop-end-fine") == 0)
	{
		screen = make_shared<LoopEndFineScreen>(layerIndex);
	}
	else if (screenName.compare("zone-start-fine") == 0)
	{
		screen = make_shared<ZoneStartFineScreen>(layerIndex);
	}
	else if (screenName.compare("zone-end-fine") == 0)
	{
		screen = make_shared<ZoneEndFineScreen>(layerIndex);
	}
	else if (screenName.compare("channel-settings") == 0)
	{
		screen = make_shared<ChannelSettingsScreen>(layerIndex);
	}
	else if (screenName.compare("edit-sound") == 0)
	{
		screen = make_shared<EditSoundScreen>(layerIndex);
	}
	else if (screenName.compare("assignment-view") == 0)
	{
		screen = make_shared<AssignmentViewScreen>(layerIndex);
	}
	else if (screenName.compare("auto-chromatic-assignment") == 0)
	{
		screen = make_shared<AutoChromaticAssignmentScreen>(layerIndex);
	}
	else if (screenName.compare("copy-note-parameters") == 0)
	{
		screen = make_shared<CopyNoteParametersScreen>(layerIndex);
	}
	else if (screenName.compare("keep-or-retry") == 0)
	{
		screen = make_shared<KeepOrRetryScreen>(layerIndex);
	}
	else if (screenName.compare("mute-assign") == 0)
	{
		screen = make_shared<MuteAssignScreen>(layerIndex);
	}
	else if (screenName.compare("program") == 0)
	{
		screen = make_shared<ProgramScreen>(layerIndex);
	}
	else if (screenName.compare("velocity-modulation") == 0)
	{
		screen = make_shared<VelocityModulationScreen>(layerIndex);
	}
	else if (screenName.compare("velo-env-filter") == 0)
	{
		screen = make_shared<VeloEnvFilterScreen>(layerIndex);
	}
	else if (screenName.compare("velo-pitch") == 0)
	{
		screen = make_shared<VeloPitchScreen>(layerIndex);
	}
	else if (screenName.compare("init-pad-assign") == 0)
	{
		screen = make_shared<InitPadAssignScreen>(layerIndex);
	}
	else if (screenName.compare("create-new-program") == 0)
	{
		screen = make_shared<CreateNewProgramScreen>(layerIndex);
	}
	else if (screenName.compare("copy-program") == 0)
	{
		screen = make_shared<CopyProgramScreen>(layerIndex);
	}
	else if (screenName.compare("delete-all-programs") == 0)
	{
		screen = make_shared<DeleteAllProgramsScreen>(layerIndex);
	}
	else if (screenName.compare("delete-all-sound") == 0)
	{
		screen = make_shared<DeleteAllSoundScreen>(layerIndex);
	}
	else if (screenName.compare("delete-program") == 0)
	{
		screen = make_shared<DeleteProgramScreen>(layerIndex);
	}
	else if (screenName.compare("load") == 0)
	{
		screen = make_shared<LoadScreen>(layerIndex);
	}
	else if (screenName.compare("format") == 0)
	{
		screen = make_shared<FormatScreen>(layerIndex);
	}
	else if (screenName.compare("setup") == 0)
	{
		screen = make_shared<SetupScreen>(layerIndex);
	}
	else if (screenName.compare("save") == 0)
	{
		screen = make_shared<SaveScreen>(layerIndex);
	}
	else if (screenName.compare("directory") == 0)
	{
		screen = make_shared<DirectoryScreen>(layerIndex);
	}
	else if (screenName.compare("load-a-program") == 0)
	{
		screen = make_shared<LoadAProgramScreen>(layerIndex);
	}
	else if (screenName.compare("load-a-sequence") == 0)
	{
		screen = make_shared<LoadASequenceScreen>(layerIndex);
	}
	else if (screenName.compare("load-a-sequence-from-all") == 0)
	{
		screen = make_shared<LoadASequenceFromAllScreen>(layerIndex);
	}
	else if (screenName.compare("load-a-sound") == 0)
	{
		screen = make_shared<LoadASoundScreen>(layerIndex);
	}
	else if (screenName.compare("load-aps-file") == 0)
	{
		screen = make_shared<LoadApsFileScreen>(layerIndex);
	}
	else if (screenName.compare("mpc2000xl-all-file") == 0)
	{
		screen = make_shared<Mpc2000XlAllFileScreen>(layerIndex);
	}
	else if (screenName.compare("save-a-program") == 0)
	{
		screen = make_shared<SaveAProgramScreen>(layerIndex);
	}
	else if (screenName.compare("save-a-sequence") == 0)
	{
		screen = make_shared<SaveASequenceScreen>(layerIndex);
	}
	else if (screenName.compare("save-a-sound") == 0)
	{
		screen = make_shared<SaveASoundScreen>(layerIndex);
	}
	else if (screenName.compare("save-all-file") == 0)
	{
		screen = make_shared<SaveAllFileScreen>(layerIndex);
	}
	else if (screenName.compare("save-aps-file") == 0)
	{
		screen = make_shared<SaveApsFileScreen>(layerIndex);
	}	
	else if (screenName.compare("delete-all-files") == 0)
	{
		screen = make_shared<DeleteAllFilesScreen>(layerIndex);
	}	
	else if (screenName.compare("delete-file") == 0)
	{
		screen = make_shared<DeleteFileScreen>(layerIndex);
	}	
	else if (screenName.compare("delete-folder") == 0)
	{
		screen = make_shared<DeleteFolderScreen>(layerIndex);
	}	
	else if (screenName.compare("cant-find-file") == 0)
	{
		screen = make_shared<CantFindFileScreen>(layerIndex);
	}	
	else if (screenName.compare("file-already-exists") == 0)
	{
		screen = make_shared<FileAlreadyExistsScreen>(layerIndex);
	}	
	else if (screenName.compare("name") == 0)
	{
		screen = make_shared<NameScreen>(layerIndex);
	}	
	else if (screenName.compare("vmpc-disk") == 0)
	{
		screen = make_shared<VmpcDiskScreen>(layerIndex);
	}	
	else if (screenName.compare("vmpc-settings") == 0)
	{
		screen = make_shared<VmpcSettingsScreen>(layerIndex);
	}	
	else if (screenName.compare("vmpc-direct-to-disk-recorder") == 0)
	{
		screen = make_shared<VmpcDirectToDiskRecorderScreen>(layerIndex);
	}	
	else if (screenName.compare("vmpc-record-jam") == 0)
	{
		screen = make_shared<VmpcRecordJamScreen>(layerIndex);
	}	
	else if (screenName.compare("vmpc-recording-finished") == 0)
	{
		screen = make_shared<VmpcRecordingFinishedScreen>(layerIndex);
	}	
	else if (screenName.compare("ver") == 0)
	{
		screen = make_shared<VerScreen>(layerIndex);
	}	
	else if (screenName.compare("init") == 0)
	{
		screen = make_shared<InitScreen>(layerIndex);
	}	
	else if (screenName.compare("second-seq") == 0)
	{
		screen = make_shared<SecondSeqScreen>(layerIndex);
	}	
	else if (screenName.compare("trans") == 0)
	{
		screen = make_shared<TransScreen>(layerIndex);
	}	
	else if (screenName.compare("transpose-permanent") == 0)
	{
		screen = make_shared<TransposePermanentScreen>(layerIndex);
	}	
	else if (screenName.compare("punch") == 0)
	{
		screen = make_shared<PunchScreen>(layerIndex);
	}
	else if (screenName.compare("sync") == 0)
	{
		screen = make_shared<SyncScreen>(layerIndex);
	}
	else if (screenName.compare("popup") == 0)
	{
		screen = make_shared<PopupScreen>();
	}

	if (screen)
	{
		screen->addChildren(children);
		screen->setTransferMap(transferMap);
		screen->setFirstField(firstField);
		screens[screenName] = screen;
	}

	return screen;
}
