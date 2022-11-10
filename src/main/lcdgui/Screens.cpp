#include "Screens.hpp"

#include <Paths.hpp>

#include <lcdgui/Component.hpp>
#include <lcdgui/Parameter.hpp>
#include <lcdgui/Info.hpp>
#include <lcdgui/FunctionKeys.hpp>
#include <lcdgui/ScreenComponent.hpp>

#include <lcdgui/screens/StaticScreen.hpp>

#include <lcdgui/screens/SequencerScreen.hpp>
#include <lcdgui/screens/AssignScreen.hpp>
#include <lcdgui/screens/BarsScreen.hpp>
#include <lcdgui/screens/EventsScreen.hpp>
#include <lcdgui/screens/NextSeqPadScreen.hpp>
#include <lcdgui/screens/NextSeqScreen.hpp>
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
#include <lcdgui/screens/VerScreen.hpp>
#include <lcdgui/screens/InitScreen.hpp>
#include <lcdgui/screens/SecondSeqScreen.hpp>
#include <lcdgui/screens/TransScreen.hpp>
#include <lcdgui/screens/PunchScreen.hpp>
#include <lcdgui/screens/SyncScreen.hpp>
#include <lcdgui/screens/MidiSwScreen.hpp>

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
#include <lcdgui/screens/window/TransposePermanentScreen.hpp>
#include <lcdgui/screens/window/SoundMemoryScreen.hpp>
#include <lcdgui/screens/window/SongWindow.hpp>
#include <lcdgui/screens/window/IgnoreTempoChangeScreen.hpp>
#include <lcdgui/screens/window/LoopSongScreen.hpp>
#include <lcdgui/screens/window/VmpcConvertAndLoadWavScreen.hpp>
#include <lcdgui/screens/window/StepEditOptionsScreen.hpp>
#include <lcdgui/screens/window/VmpcWarningSettingsIgnoredScreen.hpp>

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
#include <lcdgui/screens/dialog/FileExistsScreen.hpp>
#include <lcdgui/screens/dialog/DeleteAllSongScreen.hpp>
#include <lcdgui/screens/dialog/DeleteSongScreen.hpp>
#include <lcdgui/screens/dialog/CopySongScreen.hpp>
#include <lcdgui/screens/dialog/VmpcRecordJamScreen.hpp>
#include <lcdgui/screens/dialog/VmpcFileInUseScreen.hpp>

#include <lcdgui/screens/dialog2/DeleteAllFilesScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>


#include <lcdgui/screens/VmpcDisksScreen.hpp>
#include <lcdgui/screens/VmpcSettingsScreen.hpp>
#include <lcdgui/screens/VmpcKeyboardScreen.hpp>
#include <lcdgui/screens/VmpcMidiScreen.hpp>
#include <lcdgui/screens/VmpcAutoSaveScreen.hpp>

#include <lcdgui/screens/window/VmpcDirectToDiskRecorderScreen.hpp>
#include <lcdgui/screens/window/VmpcRecordingFinishedScreen.hpp>
#include <lcdgui/screens/window/VmpcResetKeyboardScreen.hpp>
#include <lcdgui/screens/window/VmpcDiscardMappingChangesScreen.hpp>
#include <lcdgui/screens/window/VmpcMidiPresetsScreen.hpp>

#include <lang/StrUtil.hpp>

#include <cmrc/cmrc.hpp>
#include <string_view>

#include <rapidjson/document.h>

CMRC_DECLARE(mpc);

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::dialog2;
using namespace moduru::lang;
using namespace rapidjson;
using namespace std;

Screens::Screens(mpc::Mpc& mpc)
	: mpc(mpc)
{
}

std::vector<std::unique_ptr<rapidjson::Document>>& layerDocuments()
{
    static std::vector<std::unique_ptr<rapidjson::Document>> result;
    
    if (result.empty()) {
        auto fs = cmrc::mpc::get_filesystem();

        for (int i = 0; i < 4; i++)
        {
            auto file = fs.open("screens/layer" + to_string(i + 1) + ".json");
            char* data = (char*) string_view(file.begin(), file.end() - file.begin()).data();

            auto panelDoc = make_unique<Document>();
            panelDoc->Parse(data, file.size());
            result.push_back(std::move(panelDoc));
        }
    }
    
    return result;
}

vector<int> getFunctionKeyTypes(Value& functionKeyTypes)
{
	vector<int> types;

	for (int i = 0; i < 6; i++)
	{
		if (functionKeyTypes[i].IsNull())
			types.push_back(-1);
		else
			types.push_back(functionKeyTypes[i].GetInt());
	}

	return types;
}

vector<string> getFunctionKeyLabels(Value& functionKeyLabels)
{
	vector<string> labels;

	for (int i = 0; i < 6; i++)
	{
		if (functionKeyLabels[i].IsNull())
			labels.push_back("");
		else
			labels.push_back(functionKeyLabels[i].GetString());
	}

	return labels;
}

pair<vector<shared_ptr<Component>>, map<string, vector<string>>> Screens::get(const string& screenName, int& foundInLayer, string& firstField)
{	
	for (int i = 0; i < 4; i++)
	{
		if (layerDocuments()[i]->HasMember(screenName.c_str()))
		{
			foundInLayer = i;
			break;
		}
	}

	if (foundInLayer < 0)
		return {};

	Value& arrangement = layerDocuments()[foundInLayer]->GetObject()[screenName.c_str()];

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
        
		auto functionKeysComponent = make_unique<FunctionKeys>(mpc, "function-keys", allLabels, allTypes);
		components.push_back(std::move(functionKeysComponent));
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
					parameterTransferMap.push_back(parameters[i][j].GetString());
				
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
				make_unique<Parameter>(mpc, labels[i - skipCounter].GetString(),
				parameterName,
				x[i - skipCounter].GetInt(),
				y[i - skipCounter].GetInt(),
				tfsize[i - skipCounter].GetInt()
				));

			auto parameter = components.back();
			auto field = parameter->findField(parameterName).lock();

			if (i == 0)
				firstField = parameterName;

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
			components.push_back(make_shared<Label>(mpc, infoNames[i].GetString()
				, ""
				, infoX[i].GetInt()
				, infoY[i].GetInt()
				, infoSize[i].GetInt()));
		}
	}

	return { components, transferMap };
}

shared_ptr<ScreenComponent> Screens::getScreenComponent(const string& screenName)
{
	auto candidate = screens[screenName];

	if (candidate)
		return candidate;

	shared_ptr<ScreenComponent> screen;

	int layerIndex = -1;
	string firstField = "";
	auto arrangement = get(screenName, layerIndex, firstField);
	auto children = arrangement.first;
	auto transferMap = arrangement.second;

	if (screenName == "sequencer")
	{
		screen = make_shared<SequencerScreen>(mpc, layerIndex);
	}
	else if (screenName == "sequence")
	{
		screen = make_shared<SequenceScreen>(mpc, layerIndex);
	}
	else if (screenName == "tempo-change")
	{
		screen = make_shared<TempoChangeScreen>(mpc, layerIndex);
	}
	else if (screenName == "count-metronome")
	{
		screen = make_shared<CountMetronomeScreen>(mpc, layerIndex);
	}
	else if (screenName == "edit-multiple")
	{
		screen = make_shared<EditMultipleScreen>(mpc, layerIndex);
	}
	else if (screenName == "transmit-program-changes")
	{
		screen = make_shared<TransmitProgramChangesScreen>(mpc, layerIndex);
	}
	else if (screenName == "timing-correct")
	{
		screen = make_shared<TimingCorrectScreen>(mpc, layerIndex);
	}
	else if (screenName == "time-display")
	{
		screen = make_shared<TimeDisplayScreen>(mpc, layerIndex);
	}
	else if (screenName == "paste-event")
	{
		screen = make_shared<PasteEventScreen>(mpc, layerIndex);
	}
	else if (screenName == "multi-recording-setup")
	{
		screen = make_shared<MultiRecordingSetupScreen>(mpc, layerIndex);
	}
	else if (screenName == "midi-output")
	{
		screen = make_shared<MidiOutputScreen>(mpc, layerIndex);
	}
	else if (screenName == "midi-input")
	{
		screen = make_shared<MidiInputScreen>(mpc, layerIndex);
	}
	else if (screenName == "loop-bars-window")
	{
		screen = make_shared<LoopBarsScreen>(mpc, layerIndex);
	}
	else if (screenName == "insert-event")
	{
		screen = make_shared<InsertEventScreen>(mpc, layerIndex);
	}
	else if (screenName == "erase-all-off-tracks")
	{
		screen = make_shared<EraseAllOffTracksScreen>(mpc, layerIndex);
	}
	else if (screenName == "change-tsig")
	{
		screen = make_shared<ChangeTsigScreen>(mpc, layerIndex);
	}
	else if (screenName == "edit-velocity")
	{
		screen = make_shared<EditVelocityScreen>(mpc, layerIndex);
	}
	else if (screenName == "erase")
	{
		screen = make_shared<EraseScreen>(mpc, layerIndex);
	}
	else if (screenName == "change-bars")
	{
		screen = make_shared<ChangeBarsScreen>(mpc, layerIndex);
	}
	else if (screenName == "change-bars-2")
	{
		screen = make_shared<ChangeBars2Screen>(mpc, layerIndex);
	}
	else if (screenName == "track")
	{
		screen = make_shared<TrackScreen>(mpc, layerIndex);
	}
	else if (screenName == "assign-16-levels")
	{
		screen = make_shared<Assign16LevelsScreen>(mpc, layerIndex);
	}
	else if (screenName == "midi-input-monitor")
	{
		screen = make_shared<MidiMonitorScreen>(mpc, screenName, layerIndex);
	}
	else if (screenName == "midi-output-monitor")
	{
		screen = make_shared<MidiMonitorScreen>(mpc, screenName, layerIndex);
	}
	else if (screenName == "metronome-sound")
	{
		screen = make_shared<MetronomeSoundScreen>(mpc, layerIndex);
	}
	else if (screenName == "copy-sequence")
	{
		screen = make_shared<CopySequenceScreen>(mpc, layerIndex);
	}
	else if (screenName == "copy-track")
	{
		screen = make_shared<CopyTrackScreen>(mpc, layerIndex);
	}
	else if (screenName == "delete-sequence")
	{
		screen = make_shared<DeleteSequenceScreen>(mpc, layerIndex);
	}
	else if (screenName == "delete-track")
	{
		screen = make_shared<DeleteTrackScreen>(mpc, layerIndex);
	}
	else if (screenName == "delete-all-sequences")
	{
		screen = make_shared<DeleteAllSequencesScreen>(mpc, layerIndex);
	}
	else if (screenName == "delete-all-tracks")
	{
		screen = make_shared<DeleteAllTracksScreen>(mpc, layerIndex);
	}
	else if (screenName == "next-seq-pad")
	{
		screen = make_shared<NextSeqPadScreen>(mpc, layerIndex);
	}
	else if (screenName == "next-seq")
	{
		screen = make_shared<NextSeqScreen>(mpc, layerIndex);
	}
	else if (screenName == "song")
	{
		screen = make_shared<SongScreen>(mpc, layerIndex);
	}
	else if (screenName == "track-mute")
	{
		screen = make_shared<TrMuteScreen>(mpc, layerIndex);
	}
	else if (screenName == "step-editor")
	{
		screen = make_shared<StepEditorScreen>(mpc, layerIndex);
	}
	else if (screenName == "events")
	{
		screen = make_shared<EventsScreen>(mpc, layerIndex);
	}
	else if (screenName == "bars")
	{
		screen = make_shared<BarsScreen>(mpc, layerIndex);
	}
	else if (screenName == "tr-move")
	{
		screen = make_shared<TrMoveScreen>(mpc, layerIndex);
	}
	else if (screenName == "user")
	{
		screen = make_shared<UserScreen>(mpc, layerIndex);
	}
	else if (screenName == "assign")
	{
		screen = make_shared<AssignScreen>(mpc, layerIndex);
	}
	else if (screenName == "step-timing-correct")
	{
		screen = make_shared<StepTcScreen>(mpc, layerIndex);
	}
	else if (screenName == "others")
	{
		screen = make_shared<OthersScreen>(mpc, layerIndex);
	}
	else if (screenName == "purge")
	{
		screen = make_shared<PurgeScreen>(mpc, layerIndex);
	}
	else if (screenName == "drum")
	{
		screen = make_shared<DrumScreen>(mpc, layerIndex);
	}
	else if (screenName == "trim")
	{
		screen = make_shared<TrimScreen>(mpc, layerIndex);
	}
	else if (screenName == "loop")
	{
		screen = make_shared<LoopScreen>(mpc, layerIndex);
	}
	else if (screenName == "zone")
	{
		screen = make_shared<ZoneScreen>(mpc, layerIndex);
	}
	else if (screenName == "number-of-zones")
	{
		screen = make_shared<NumberOfZonesScreen>(mpc, layerIndex);
	}
	else if (screenName == "params")
	{
		screen = make_shared<SndParamsScreen>(mpc, layerIndex);
	}
	else if (screenName == "program-params")
	{
		screen = make_shared<PgmParamsScreen>(mpc, layerIndex);
	}
	else if (screenName == "program-assign")
	{
		screen = make_shared<PgmAssignScreen>(mpc, layerIndex);
	}
	else if (screenName == "select-drum")
	{
		screen = make_shared<SelectDrumScreen>(mpc, layerIndex);
	}
	else if (screenName == "sample")
	{
		screen = make_shared<SampleScreen>(mpc, layerIndex);
	}
	else if (screenName == "sound")
	{
		screen = make_shared<SoundScreen>(mpc, layerIndex);
	}
	else if (screenName == "convert-sound")
	{
		screen = make_shared<ConvertSoundScreen>(mpc, layerIndex);
	}
	else if (screenName == "copy-sound")
	{
		screen = make_shared<CopySoundScreen>(mpc, layerIndex);
	}
	else if (screenName == "delete-sound")
	{
		screen = make_shared<DeleteSoundScreen>(mpc, layerIndex);
	}
	else if (screenName == "mono-to-stereo")
	{
		screen = make_shared<MonoToStereoScreen>(mpc, layerIndex);
	}
	else if (screenName == "resample")
	{
		screen = make_shared<ResampleScreen>(mpc, layerIndex);
	}
	else if (screenName == "stereo-to-mono")
	{
		screen = make_shared<StereoToMonoScreen>(mpc, layerIndex);
	}
	else if (screenName == "mixer")
	{
		screen = make_shared<MixerScreen>(mpc, layerIndex);
	}
	else if (screenName == "mixer-setup")
	{
		screen = make_shared<MixerSetupScreen>(mpc, layerIndex);
	}
	else if (screenName == "select-mixer-drum")
	{
		screen = make_shared<SelectMixerDrumScreen>(mpc, layerIndex);
	}
	else if (screenName == "fx-edit")
	{
		screen = make_shared<FxEditScreen>(mpc, layerIndex);
	}
	else if (screenName == "start-fine")
	{
		screen = make_shared<StartFineScreen>(mpc, layerIndex);
	}
	else if (screenName == "end-fine")
	{
		screen = make_shared<EndFineScreen>(mpc, layerIndex);
	}
	else if (screenName == "loop-to-fine")
	{
		screen = make_shared<LoopToFineScreen>(mpc, layerIndex);
	}
	else if (screenName == "loop-end-fine")
	{
		screen = make_shared<LoopEndFineScreen>(mpc, layerIndex);
	}
	else if (screenName == "zone-start-fine")
	{
		screen = make_shared<ZoneStartFineScreen>(mpc, layerIndex);
	}
	else if (screenName == "zone-end-fine")
	{
		screen = make_shared<ZoneEndFineScreen>(mpc, layerIndex);
	}
	else if (screenName == "channel-settings")
	{
		screen = make_shared<ChannelSettingsScreen>(mpc, layerIndex);
	}
	else if (screenName == "edit-sound")
	{
		screen = make_shared<EditSoundScreen>(mpc, layerIndex);
	}
	else if (screenName == "assignment-view")
	{
		screen = make_shared<AssignmentViewScreen>(mpc, layerIndex);
	}
	else if (screenName == "auto-chromatic-assignment")
	{
		screen = make_shared<AutoChromaticAssignmentScreen>(mpc, layerIndex);
	}
	else if (screenName == "copy-note-parameters")
	{
		screen = make_shared<CopyNoteParametersScreen>(mpc, layerIndex);
	}
	else if (screenName == "keep-or-retry")
	{
		screen = make_shared<KeepOrRetryScreen>(mpc, layerIndex);
	}
	else if (screenName == "mute-assign")
	{
		screen = make_shared<MuteAssignScreen>(mpc, layerIndex);
	}
	else if (screenName == "program")
	{
		screen = make_shared<ProgramScreen>(mpc, layerIndex);
	}
	else if (screenName == "velocity-modulation")
	{
		screen = make_shared<VelocityModulationScreen>(mpc, layerIndex);
	}
	else if (screenName == "velo-env-filter")
	{
		screen = make_shared<VeloEnvFilterScreen>(mpc, layerIndex);
	}
	else if (screenName == "velo-pitch")
	{
		screen = make_shared<VeloPitchScreen>(mpc, layerIndex);
	}
	else if (screenName == "init-pad-assign")
	{
		screen = make_shared<InitPadAssignScreen>(mpc, layerIndex);
	}
	else if (screenName == "create-new-program")
	{
		screen = make_shared<CreateNewProgramScreen>(mpc, layerIndex);
	}
	else if (screenName == "copy-program")
	{
		screen = make_shared<CopyProgramScreen>(mpc, layerIndex);
	}
	else if (screenName == "delete-all-programs")
	{
		screen = make_shared<DeleteAllProgramsScreen>(mpc, layerIndex);
	}
	else if (screenName == "delete-all-sound")
	{
		screen = make_shared<DeleteAllSoundScreen>(mpc, layerIndex);
	}
	else if (screenName == "delete-program")
	{
		screen = make_shared<DeleteProgramScreen>(mpc, layerIndex);
	}
	else if (screenName == "load")
	{
		screen = make_shared<LoadScreen>(mpc, layerIndex);
	}
	else if (screenName == "format")
	{
		screen = make_shared<FormatScreen>(mpc, layerIndex);
	}
	else if (screenName == "setup")
	{
		screen = make_shared<SetupScreen>(mpc, layerIndex);
	}
	else if (screenName == "save")
	{
		screen = make_shared<SaveScreen>(mpc, layerIndex);
	}
	else if (screenName == "directory")
	{
		screen = make_shared<DirectoryScreen>(mpc, layerIndex);
	}
	else if (screenName == "load-a-program")
	{
		screen = make_shared<LoadAProgramScreen>(mpc, layerIndex);
	}
	else if (screenName == "load-a-sequence")
	{
		screen = make_shared<LoadASequenceScreen>(mpc, layerIndex);
	}
	else if (screenName == "load-a-sequence-from-all")
	{
		screen = make_shared<LoadASequenceFromAllScreen>(mpc, layerIndex);
	}
	else if (screenName == "load-a-sound")
	{
		screen = make_shared<LoadASoundScreen>(mpc, layerIndex);
	}
	else if (screenName == "load-aps-file")
	{
		screen = make_shared<LoadApsFileScreen>(mpc, layerIndex);
	}
	else if (screenName == "mpc2000xl-all-file")
	{
		screen = make_shared<Mpc2000XlAllFileScreen>(mpc, layerIndex);
	}
	else if (screenName == "save-a-program")
	{
		screen = make_shared<SaveAProgramScreen>(mpc, layerIndex);
	}
	else if (screenName == "save-a-sequence")
	{
		screen = make_shared<SaveASequenceScreen>(mpc, layerIndex);
	}
	else if (screenName == "save-a-sound")
	{
		screen = make_shared<SaveASoundScreen>(mpc, layerIndex);
	}
	else if (screenName == "save-all-file")
	{
		screen = make_shared<SaveAllFileScreen>(mpc, layerIndex);
	}
	else if (screenName == "save-aps-file")
	{
		screen = make_shared<SaveApsFileScreen>(mpc, layerIndex);
	}
	else if (screenName == "delete-all-files")
	{
		screen = make_shared<DeleteAllFilesScreen>(mpc, layerIndex);
	}
	else if (screenName == "delete-file")
	{
		screen = make_shared<DeleteFileScreen>(mpc, layerIndex);
	}
	else if (screenName == "delete-folder")
	{
		screen = make_shared<DeleteFolderScreen>(mpc, layerIndex);
	}
	else if (screenName == "cant-find-file")
	{
		screen = make_shared<CantFindFileScreen>(mpc, layerIndex);
	}
	else if (screenName == "file-exists")
	{
		screen = make_shared<FileExistsScreen>(mpc, layerIndex);
	}
	else if (screenName == "name")
	{
		screen = make_shared<NameScreen>(mpc, layerIndex);
	}
	else if (screenName == "vmpc-disks")
	{
		screen = make_shared<VmpcDisksScreen>(mpc, layerIndex);
	}
	else if (screenName == "vmpc-settings")
	{
		screen = make_shared<VmpcSettingsScreen>(mpc, layerIndex);
	}
	else if (screenName == "vmpc-direct-to-disk-recorder")
	{
		screen = make_shared<VmpcDirectToDiskRecorderScreen>(mpc, layerIndex);
	}
	else if (screenName == "vmpc-record-jam")
	{
		screen = make_shared<VmpcRecordJamScreen>(mpc, layerIndex);
	}
	else if (screenName == "vmpc-recording-finished")
	{
		screen = make_shared<VmpcRecordingFinishedScreen>(mpc, layerIndex);
	}
	else if (screenName == "ver")
	{
		screen = make_shared<VerScreen>(mpc, layerIndex);
	}
	else if (screenName == "init")
	{
		screen = make_shared<InitScreen>(mpc, layerIndex);
	}
	else if (screenName == "second-seq")
	{
		screen = make_shared<SecondSeqScreen>(mpc, layerIndex);
	}
	else if (screenName == "trans")
	{
		screen = make_shared<TransScreen>(mpc, layerIndex);
	}
	else if (screenName == "transpose-permanent")
	{
		screen = make_shared<TransposePermanentScreen>(mpc, layerIndex);
	}
	else if (screenName == "punch")
	{
		screen = make_shared<PunchScreen>(mpc, layerIndex);
	}
	else if (screenName == "sync")
	{
		screen = make_shared<SyncScreen>(mpc, layerIndex);
	}
	
	// We break up the else-if chain due to "C1061 Compiler limit: blocks nested too deeply" on Visual Studio
	
	if (screenName == "popup")
	{
		screen = make_shared<PopupScreen>(mpc);
	}
	else if (screenName == "mpc2000xl" || screenName == "black" || screenName == "half-black" || screenName == "empty")
	{
		screen = make_shared<StaticScreen>(mpc);
		screen->findChild<Background>("").lock()->setName(screenName);
	}
	else if (screenName == "sound-memory")
	{
		screen = make_shared<SoundMemoryScreen>(mpc, layerIndex);
	}
	else if (screenName == "song-window")
	{
		screen = make_shared<SongWindow>(mpc, layerIndex);
	}
	else if (screenName == "vmpc-file-in-use")
	{
		screen = make_shared<VmpcFileInUseScreen>(mpc, layerIndex);
	}
	else if (screenName == "ignore-tempo-change")
	{
		screen = make_shared<IgnoreTempoChangeScreen>(mpc, layerIndex);
	}
	else if (screenName == "loop-song")
	{
		screen = make_shared<LoopSongScreen>(mpc, layerIndex);
	}
	else if (screenName == "delete-song")
	{
		screen = make_shared<DeleteSongScreen>(mpc, layerIndex);
	}
	else if (screenName == "delete-all-song")
	{
		screen = make_shared<DeleteAllSongScreen>(mpc, layerIndex);
	}
	else if (screenName == "copy-song")
	{
		screen = make_shared<CopySongScreen>(mpc, layerIndex);
	}
    else if (screenName == "vmpc-keyboard")
    {
        screen = make_shared<VmpcKeyboardScreen>(mpc, layerIndex);
    }
    else if (screenName == "vmpc-midi")
    {
        screen = make_shared<VmpcMidiScreen>(mpc, layerIndex);
    }
    else if (screenName == "vmpc-reset-keyboard")
    {
        screen = make_shared<VmpcResetKeyboardScreen>(mpc, layerIndex);
    }
    else if (screenName == "vmpc-discard-mapping-changes")
    {
        screen = make_shared<VmpcDiscardMappingChangesScreen>(mpc, layerIndex);
    }
    else if (screenName == "vmpc-auto-save")
    {
        screen = make_shared<VmpcAutoSaveScreen>(mpc, layerIndex);
    }
    else if (screenName == "midi-sw")
    {
        screen = make_shared<MidiSwScreen>(mpc, layerIndex);
    }
    else if (screenName == "vmpc-convert-and-load-wav")
    {
        screen = make_shared<VmpcConvertAndLoadWavScreen>(mpc, layerIndex);
    }
    else if (screenName == "step-edit-options")
    {
        screen = make_shared<StepEditOptionsScreen>(mpc, layerIndex);
    }
    else if (screenName == "vmpc-midi-presets")
    {
        screen = make_shared<VmpcMidiPresetsScreen>(mpc, layerIndex);
    }
    else if (screenName == "vmpc-warning-settings-ignored")
    {
        screen = make_shared<VmpcWarningSettingsIgnoredScreen>(mpc, layerIndex);
    }

	if (screen)
	{
		screen->findChild<Background>("").lock()->addChildren(children);
		screen->setTransferMap(transferMap);
		screen->setFirstField(firstField);
		screens[screenName] = screen;
	}

	return screen;
}
