#include "Screens.hpp"

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
#include <lcdgui/screens/window/VmpcKnownControllerDetectedScreen.hpp>
#include <lcdgui/screens/window/VmpcContinuePreviousSessionScreen.hpp>
#include <lcdgui/screens/window/ConvertSongToSeqScreen.hpp>
#include <lcdgui/screens/window/LocateScreen.hpp>

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

#include <StrUtil.hpp>

#include "MpcResourceUtil.hpp"

#include <rapidjson/document.h>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::dialog2;
using namespace rapidjson;

Screens::Screens(mpc::Mpc &mpc)
        : mpc(mpc)
{
}

std::vector<std::unique_ptr<rapidjson::Document>> &layerDocuments()
{
    static std::vector<std::unique_ptr<rapidjson::Document>> result;

    if (result.empty())
    {
        for (int i = 0; i < 4; i++)
        {
            const auto path = "screens/layer" + std::to_string(i + 1) + ".json";
            auto data = mpc::MpcResourceUtil::get_resource_data(path);

            auto panelDoc = std::make_unique<Document>();
            panelDoc->Parse(&data[0], data.size());
            result.push_back(std::move(panelDoc));
        }
    }

    return result;
}

std::vector<int> getFunctionKeyTypes(Value &functionKeyTypes)
{
    std::vector<int> types;

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

std::vector<std::string> getFunctionKeyLabels(Value &functionKeyLabels)
{
    std::vector<std::string> labels;

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

std::pair<std::vector<std::shared_ptr<Component>>, std::map<std::string, std::vector<std::string>>>
Screens::get(const std::string &screenName, int &foundInLayer, std::string &firstField)
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
    {
        return {};
    }

    Value &arrangement = layerDocuments()[foundInLayer]->GetObj()[screenName.c_str()];

    std::vector<std::shared_ptr<Component>> components;

    if (arrangement.HasMember("fblabels"))
    {
        Value &functionLabels = arrangement["fblabels"];
        Value &functionTypes = arrangement["fbtypes"];

        std::vector<std::vector<std::string>> allLabels;
        std::vector<std::vector<int>> allTypes;

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

        auto functionKeysComponent = std::make_unique<FunctionKeys>(mpc, "function-keys", allLabels, allTypes);
        components.push_back(std::move(functionKeysComponent));
    }

    std::map<std::string, std::vector<std::string>> transferMap;

    if (arrangement.HasMember("parameters"))
    {
        Value &parameters = arrangement["parameters"];
        Value &labels = arrangement["labels"];
        Value &x = arrangement["x"];
        Value &y = arrangement["y"];
        Value &tfsize = arrangement["tfsize"];

        std::vector<std::string> row;

        int skipCounter = 0;

        std::string previous;

        for (int i = 0; i < parameters.Size(); i++)
        {
            if (parameters[i].IsArray())
            {
                std::vector<std::string> parameterTransferMap;

                for (int j = 0; j < parameters[i].Size(); j++)
                    parameterTransferMap.push_back(parameters[i][j].GetString());

                transferMap[previous] = parameterTransferMap;
                skipCounter++;
                continue;
            }

            auto parameterName = std::string(parameters[i].GetString());
            auto parameterNames = StrUtil::split(parameterName, ',');
            std::string nextFocus = "_";

            if (parameterNames.size() > 1)
            {
                parameterName = parameterNames[0];
                nextFocus = parameterNames[1];
            }

            row.push_back(parameterName);

            components.push_back(std::make_unique<Parameter>(
                    mpc,
                    labels[i - skipCounter].GetString(),
                    parameterName,
                    x[i - skipCounter].GetInt(),
                    y[i - skipCounter].GetInt(),
                    tfsize[i - skipCounter].GetInt()));

            auto parameter = components.back();
            auto field = parameter->findField(parameterName);

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
        Value &infoNames = arrangement["infowidgets"];
        Value &infoSize = arrangement["infosize"];
        Value &infoX = arrangement["infox"];
        Value &infoY = arrangement["infoy"];

        for (int i = 0; i < infoNames.Size(); i++)
        {
            components.push_back(
                    std::make_shared<Label>(mpc, infoNames[i].GetString(), "", infoX[i].GetInt(), infoY[i].GetInt(),
                                       infoSize[i].GetInt()));
        }
    }

    return {components, transferMap};
}

std::shared_ptr <ScreenComponent> Screens::getScreenComponent(const std::string &screenName)
{
    auto candidate = screens[screenName];

    if (candidate)
    {
        return candidate;
    }

    std::shared_ptr <ScreenComponent> screen;

    int layerIndex = -1;
    std::string firstField;
    auto arrangement = get(screenName, layerIndex, firstField);
    auto children = arrangement.first;
    auto transferMap = arrangement.second;

    if (screenName == "sequencer")
    {
        screen = std::make_shared<SequencerScreen>(mpc, layerIndex);
    }
    else if (screenName == "sequence")
    {
        screen = std::make_shared<SequenceScreen>(mpc, layerIndex);
    }
    else if (screenName == "tempo-change")
    {
        screen = std::make_shared<TempoChangeScreen>(mpc, layerIndex);
    }
    else if (screenName == "count-metronome")
    {
        screen = std::make_shared<CountMetronomeScreen>(mpc, layerIndex);
    }
    else if (screenName == "edit-multiple")
    {
        screen = std::make_shared<EditMultipleScreen>(mpc, layerIndex);
    }
    else if (screenName == "transmit-program-changes")
    {
        screen = std::make_shared<TransmitProgramChangesScreen>(mpc, layerIndex);
    }
    else if (screenName == "timing-correct")
    {
        screen = std::make_shared<TimingCorrectScreen>(mpc, layerIndex);
    }
    else if (screenName == "time-display")
    {
        screen = std::make_shared<TimeDisplayScreen>(mpc, layerIndex);
    }
    else if (screenName == "paste-event")
    {
        screen = std::make_shared<PasteEventScreen>(mpc, layerIndex);
    }
    else if (screenName == "multi-recording-setup")
    {
        screen = std::make_shared<MultiRecordingSetupScreen>(mpc, layerIndex);
    }
    else if (screenName == "midi-output")
    {
        screen = std::make_shared<MidiOutputScreen>(mpc, layerIndex);
    }
    else if (screenName == "midi-input")
    {
        screen = std::make_shared<MidiInputScreen>(mpc, layerIndex);
    }
    else if (screenName == "loop-bars-window")
    {
        screen = std::make_shared<LoopBarsScreen>(mpc, layerIndex);
    }
    else if (screenName == "insert-event")
    {
        screen = std::make_shared<InsertEventScreen>(mpc, layerIndex);
    }
    else if (screenName == "erase-all-off-tracks")
    {
        screen = std::make_shared<EraseAllOffTracksScreen>(mpc, layerIndex);
    }
    else if (screenName == "change-tsig")
    {
        screen = std::make_shared<ChangeTsigScreen>(mpc, layerIndex);
    }
    else if (screenName == "edit-velocity")
    {
        screen = std::make_shared<EditVelocityScreen>(mpc, layerIndex);
    }
    else if (screenName == "erase")
    {
        screen = std::make_shared<EraseScreen>(mpc, layerIndex);
    }
    else if (screenName == "change-bars")
    {
        screen = std::make_shared<ChangeBarsScreen>(mpc, layerIndex);
    }
    else if (screenName == "change-bars-2")
    {
        screen = std::make_shared<ChangeBars2Screen>(mpc, layerIndex);
    }
    else if (screenName == "track")
    {
        screen = std::make_shared<TrackScreen>(mpc, layerIndex);
    }
    else if (screenName == "assign-16-levels")
    {
        screen = std::make_shared<Assign16LevelsScreen>(mpc, layerIndex);
    }
    else if (screenName == "midi-input-monitor")
    {
        screen = std::make_shared<MidiMonitorScreen>(mpc, screenName, layerIndex);
    }
    else if (screenName == "midi-output-monitor")
    {
        screen = std::make_shared<MidiMonitorScreen>(mpc, screenName, layerIndex);
    }
    else if (screenName == "metronome-sound")
    {
        screen = std::make_shared<MetronomeSoundScreen>(mpc, layerIndex);
    }
    else if (screenName == "copy-sequence")
    {
        screen = std::make_shared<CopySequenceScreen>(mpc, layerIndex);
    }
    else if (screenName == "copy-track")
    {
        screen = std::make_shared<CopyTrackScreen>(mpc, layerIndex);
    }
    else if (screenName == "delete-sequence")
    {
        screen = std::make_shared<DeleteSequenceScreen>(mpc, layerIndex);
    }
    else if (screenName == "delete-track")
    {
        screen = std::make_shared<DeleteTrackScreen>(mpc, layerIndex);
    }
    else if (screenName == "delete-all-sequences")
    {
        screen = std::make_shared<DeleteAllSequencesScreen>(mpc, layerIndex);
    }
    else if (screenName == "delete-all-tracks")
    {
        screen = std::make_shared<DeleteAllTracksScreen>(mpc, layerIndex);
    }
    else if (screenName == "next-seq-pad")
    {
        screen = std::make_shared<NextSeqPadScreen>(mpc, layerIndex);
    }
    else if (screenName == "next-seq")
    {
        screen = std::make_shared<NextSeqScreen>(mpc, layerIndex);
    }
    else if (screenName == "song")
    {
        screen = std::make_shared<SongScreen>(mpc, layerIndex);
    }
    else if (screenName == "track-mute")
    {
        screen = std::make_shared<TrMuteScreen>(mpc, layerIndex);
    }
    else if (screenName == "step-editor")
    {
        screen = std::make_shared<StepEditorScreen>(mpc, layerIndex);
    }
    else if (screenName == "events")
    {
        screen = std::make_shared<EventsScreen>(mpc, layerIndex);
    }
    else if (screenName == "bars")
    {
        screen = std::make_shared<BarsScreen>(mpc, layerIndex);
    }
    else if (screenName == "tr-move")
    {
        screen = std::make_shared<TrMoveScreen>(mpc, layerIndex);
    }
    else if (screenName == "user")
    {
        screen = std::make_shared<UserScreen>(mpc, layerIndex);
    }
    else if (screenName == "assign")
    {
        screen = std::make_shared<AssignScreen>(mpc, layerIndex);
    }
    else if (screenName == "step-timing-correct")
    {
        screen = std::make_shared<StepTcScreen>(mpc, layerIndex);
    }
    else if (screenName == "others")
    {
        screen = std::make_shared<OthersScreen>(mpc, layerIndex);
    }
    else if (screenName == "purge")
    {
        screen = std::make_shared<PurgeScreen>(mpc, layerIndex);
    }
    else if (screenName == "drum")
    {
        screen = std::make_shared<DrumScreen>(mpc, layerIndex);
    }
    else if (screenName == "trim")
    {
        screen = std::make_shared<TrimScreen>(mpc, layerIndex);
    }
    else if (screenName == "loop")
    {
        screen = std::make_shared<LoopScreen>(mpc, layerIndex);
    }
    else if (screenName == "zone")
    {
        screen = std::make_shared<ZoneScreen>(mpc, layerIndex);
    }
    else if (screenName == "number-of-zones")
    {
        screen = std::make_shared<NumberOfZonesScreen>(mpc, layerIndex);
    }
    else if (screenName == "params")
    {
        screen = std::make_shared<SndParamsScreen>(mpc, layerIndex);
    }
    else if (screenName == "program-params")
    {
        screen = std::make_shared<PgmParamsScreen>(mpc, layerIndex);
    }
    else if (screenName == "program-assign")
    {
        screen = std::make_shared<PgmAssignScreen>(mpc, layerIndex);
    }
    else if (screenName == "select-drum")
    {
        screen = std::make_shared<SelectDrumScreen>(mpc, layerIndex);
    }
    else if (screenName == "sample")
    {
        screen = std::make_shared<SampleScreen>(mpc, layerIndex);
    }
    else if (screenName == "sound")
    {
        screen = std::make_shared<SoundScreen>(mpc, layerIndex);
    }
    else if (screenName == "convert-sound")
    {
        screen = std::make_shared<ConvertSoundScreen>(mpc, layerIndex);
    }
    else if (screenName == "copy-sound")
    {
        screen = std::make_shared<CopySoundScreen>(mpc, layerIndex);
    }
    else if (screenName == "delete-sound")
    {
        screen = std::make_shared<DeleteSoundScreen>(mpc, layerIndex);
    }
    else if (screenName == "mono-to-stereo")
    {
        screen = std::make_shared<MonoToStereoScreen>(mpc, layerIndex);
    }
    else if (screenName == "resample")
    {
        screen = std::make_shared<ResampleScreen>(mpc, layerIndex);
    }
    else if (screenName == "stereo-to-mono")
    {
        screen = std::make_shared<StereoToMonoScreen>(mpc, layerIndex);
    }
    else if (screenName == "mixer")
    {
        screen = std::make_shared<MixerScreen>(mpc, layerIndex);
    }
    else if (screenName == "mixer-setup")
    {
        screen = std::make_shared<MixerSetupScreen>(mpc, layerIndex);
    }
    else if (screenName == "select-mixer-drum")
    {
        screen = std::make_shared<SelectMixerDrumScreen>(mpc, layerIndex);
    }
    else if (screenName == "fx-edit")
    {
        screen = std::make_shared<FxEditScreen>(mpc, layerIndex);
    }
    else if (screenName == "start-fine")
    {
        screen = std::make_shared<StartFineScreen>(mpc, layerIndex);
    }
    else if (screenName == "end-fine")
    {
        screen = std::make_shared<EndFineScreen>(mpc, layerIndex);
    }
    else if (screenName == "loop-to-fine")
    {
        screen = std::make_shared<LoopToFineScreen>(mpc, layerIndex);
    }
    else if (screenName == "loop-end-fine")
    {
        screen = std::make_shared<LoopEndFineScreen>(mpc, layerIndex);
    }
    else if (screenName == "zone-start-fine")
    {
        screen = std::make_shared<ZoneStartFineScreen>(mpc, layerIndex);
    }
    else if (screenName == "zone-end-fine")
    {
        screen = std::make_shared<ZoneEndFineScreen>(mpc, layerIndex);
    }
    else if (screenName == "channel-settings")
    {
        screen = std::make_shared<ChannelSettingsScreen>(mpc, layerIndex);
    }
    else if (screenName == "edit-sound")
    {
        screen = std::make_shared<EditSoundScreen>(mpc, layerIndex);
    }
    else if (screenName == "assignment-view")
    {
        screen = std::make_shared<AssignmentViewScreen>(mpc, layerIndex);
    }
    else if (screenName == "auto-chromatic-assignment")
    {
        screen = std::make_shared<AutoChromaticAssignmentScreen>(mpc, layerIndex);
    }
    else if (screenName == "copy-note-parameters")
    {
        screen = std::make_shared<CopyNoteParametersScreen>(mpc, layerIndex);
    }
    else if (screenName == "keep-or-retry")
    {
        screen = std::make_shared<KeepOrRetryScreen>(mpc, layerIndex);
    }
    else if (screenName == "mute-assign")
    {
        screen = std::make_shared<MuteAssignScreen>(mpc, layerIndex);
    }
    else if (screenName == "program")
    {
        screen = std::make_shared<ProgramScreen>(mpc, layerIndex);
    }
    else if (screenName == "velocity-modulation")
    {
        screen = std::make_shared<VelocityModulationScreen>(mpc, layerIndex);
    }
    else if (screenName == "velo-env-filter")
    {
        screen = std::make_shared<VeloEnvFilterScreen>(mpc, layerIndex);
    }
    else if (screenName == "velo-pitch")
    {
        screen = std::make_shared<VeloPitchScreen>(mpc, layerIndex);
    }
    else if (screenName == "init-pad-assign")
    {
        screen = std::make_shared<InitPadAssignScreen>(mpc, layerIndex);
    }
    else if (screenName == "create-new-program")
    {
        screen = std::make_shared<CreateNewProgramScreen>(mpc, layerIndex);
    }
    else if (screenName == "copy-program")
    {
        screen = std::make_shared<CopyProgramScreen>(mpc, layerIndex);
    }
    else if (screenName == "delete-all-programs")
    {
        screen = std::make_shared<DeleteAllProgramsScreen>(mpc, layerIndex);
    }
    else if (screenName == "delete-all-sound")
    {
        screen = std::make_shared<DeleteAllSoundScreen>(mpc, layerIndex);
    }
    else if (screenName == "delete-program")
    {
        screen = std::make_shared<DeleteProgramScreen>(mpc, layerIndex);
    }
    else if (screenName == "load")
    {
        screen = std::make_shared<LoadScreen>(mpc, layerIndex);
    }
    else if (screenName == "format")
    {
        screen = std::make_shared<FormatScreen>(mpc, layerIndex);
    }
    else if (screenName == "setup")
    {
        screen = std::make_shared<SetupScreen>(mpc, layerIndex);
    }
    else if (screenName == "save")
    {
        screen = std::make_shared<SaveScreen>(mpc, layerIndex);
    }
    else if (screenName == "directory")
    {
        screen = std::make_shared<DirectoryScreen>(mpc, layerIndex);
    }
    else if (screenName == "load-a-program")
    {
        screen = std::make_shared<LoadAProgramScreen>(mpc, layerIndex);
    }
    else if (screenName == "load-a-sequence")
    {
        screen = std::make_shared<LoadASequenceScreen>(mpc, layerIndex);
    }
    else if (screenName == "load-a-sequence-from-all")
    {
        screen = std::make_shared<LoadASequenceFromAllScreen>(mpc, layerIndex);
    }
    else if (screenName == "load-a-sound")
    {
        screen = std::make_shared<LoadASoundScreen>(mpc, layerIndex);
    }
    else if (screenName == "load-aps-file")
    {
        screen = std::make_shared<LoadApsFileScreen>(mpc, layerIndex);
    }
    else if (screenName == "mpc2000xl-all-file")
    {
        screen = std::make_shared<Mpc2000XlAllFileScreen>(mpc, layerIndex);
    }
    else if (screenName == "save-a-program")
    {
        screen = std::make_shared<SaveAProgramScreen>(mpc, layerIndex);
    }
    else if (screenName == "save-a-sequence")
    {
        screen = std::make_shared<SaveASequenceScreen>(mpc, layerIndex);
    }
    else if (screenName == "save-a-sound")
    {
        screen = std::make_shared<SaveASoundScreen>(mpc, layerIndex);
    }
    else if (screenName == "save-all-file")
    {
        screen = std::make_shared<SaveAllFileScreen>(mpc, layerIndex);
    }
    else if (screenName == "save-aps-file")
    {
        screen = std::make_shared<SaveApsFileScreen>(mpc, layerIndex);
    }
    else if (screenName == "delete-all-files")
    {
        screen = std::make_shared<DeleteAllFilesScreen>(mpc, layerIndex);
    }
    else if (screenName == "delete-file")
    {
        screen = std::make_shared<DeleteFileScreen>(mpc, layerIndex);
    }
    else if (screenName == "delete-folder")
    {
        screen = std::make_shared<DeleteFolderScreen>(mpc, layerIndex);
    }
    else if (screenName == "cant-find-file")
    {
        screen = std::make_shared<CantFindFileScreen>(mpc, layerIndex);
    }
    else if (screenName == "file-exists")
    {
        screen = std::make_shared<FileExistsScreen>(mpc, layerIndex);
    }
    else if (screenName == "name")
    {
        screen = std::make_shared<NameScreen>(mpc, layerIndex);
    }
    else if (screenName == "vmpc-disks")
    {
        screen = std::make_shared<VmpcDisksScreen>(mpc, layerIndex);
    }
    else if (screenName == "vmpc-settings")
    {
        screen = std::make_shared<VmpcSettingsScreen>(mpc, layerIndex);
    }
    else if (screenName == "vmpc-direct-to-disk-recorder")
    {
        screen = std::make_shared<VmpcDirectToDiskRecorderScreen>(mpc, layerIndex);
    }
    else if (screenName == "vmpc-record-jam")
    {
        screen = std::make_shared<VmpcRecordJamScreen>(mpc, layerIndex);
    }
    else if (screenName == "vmpc-recording-finished")
    {
        screen = std::make_shared<VmpcRecordingFinishedScreen>(mpc, layerIndex);
    }
    else if (screenName == "ver")
    {
        screen = std::make_shared<VerScreen>(mpc, layerIndex);
    }
    else if (screenName == "init")
    {
        screen = std::make_shared<InitScreen>(mpc, layerIndex);
    }
    else if (screenName == "second-seq")
    {
        screen = std::make_shared<SecondSeqScreen>(mpc, layerIndex);
    }
    else if (screenName == "trans")
    {
        screen = std::make_shared<TransScreen>(mpc, layerIndex);
    }
    else if (screenName == "transpose-permanent")
    {
        screen = std::make_shared<TransposePermanentScreen>(mpc, layerIndex);
    }
    else if (screenName == "punch")
    {
        screen = std::make_shared<PunchScreen>(mpc, layerIndex);
    }
    else if (screenName == "sync")
    {
        screen = std::make_shared<SyncScreen>(mpc, layerIndex);
    }

    // We break up the else-if chain due to "C1061 Compiler limit: blocks nested too deeply" on Visual Studio

    if (screenName == "popup")
    {
        screen = std::make_shared<PopupScreen>(mpc);
    }
    else if (screenName == "sound-memory")
    {
        screen = std::make_shared<SoundMemoryScreen>(mpc, layerIndex);
    }
    else if (screenName == "song-window")
    {
        screen = std::make_shared<SongWindow>(mpc, layerIndex);
    }
    else if (screenName == "vmpc-file-in-use")
    {
        screen = std::make_shared<VmpcFileInUseScreen>(mpc, layerIndex);
    }
    else if (screenName == "ignore-tempo-change")
    {
        screen = std::make_shared<IgnoreTempoChangeScreen>(mpc, layerIndex);
    }
    else if (screenName == "loop-song")
    {
        screen = std::make_shared<LoopSongScreen>(mpc, layerIndex);
    }
    else if (screenName == "delete-song")
    {
        screen = std::make_shared<DeleteSongScreen>(mpc, layerIndex);
    }
    else if (screenName == "delete-all-song")
    {
        screen = std::make_shared<DeleteAllSongScreen>(mpc, layerIndex);
    }
    else if (screenName == "copy-song")
    {
        screen = std::make_shared<CopySongScreen>(mpc, layerIndex);
    }
    else if (screenName == "vmpc-keyboard")
    {
        screen = std::make_shared<VmpcKeyboardScreen>(mpc, layerIndex);
    }
    else if (screenName == "vmpc-midi")
    {
        screen = std::make_shared<VmpcMidiScreen>(mpc, layerIndex);
    }
    else if (screenName == "vmpc-reset-keyboard")
    {
        screen = std::make_shared<VmpcResetKeyboardScreen>(mpc, layerIndex);
    }
    else if (screenName == "vmpc-discard-mapping-changes")
    {
        screen = std::make_shared<VmpcDiscardMappingChangesScreen>(mpc, layerIndex);
    }
    else if (screenName == "vmpc-auto-save")
    {
        screen = std::make_shared<VmpcAutoSaveScreen>(mpc, layerIndex);
    }
    else if (screenName == "midi-sw")
    {
        screen = std::make_shared<MidiSwScreen>(mpc, layerIndex);
    }
    else if (screenName == "vmpc-convert-and-load-wav")
    {
        screen = std::make_shared<VmpcConvertAndLoadWavScreen>(mpc, layerIndex);
    }
    else if (screenName == "step-edit-options")
    {
        screen = std::make_shared<StepEditOptionsScreen>(mpc, layerIndex);
    }
    else if (screenName == "vmpc-midi-presets")
    {
        screen = std::make_shared<VmpcMidiPresetsScreen>(mpc, layerIndex);
    }
    else if (screenName == "vmpc-warning-settings-ignored")
    {
        screen = std::make_shared<VmpcWarningSettingsIgnoredScreen>(mpc, layerIndex);
    }
    else if (screenName == "vmpc-known-controller-detected")
    {
        screen = std::make_shared<VmpcKnownControllerDetectedScreen>(mpc, layerIndex);
    }
    else if (screenName == "vmpc-continue-previous-session")
    {
        screen = std::make_shared<VmpcContinuePreviousSessionScreen>(mpc, layerIndex);
    }
    else if (screenName == "convert-song-to-seq")
    {
        screen = std::make_shared<ConvertSongToSeqScreen>(mpc, layerIndex);
    }
    else if (screenName == "locate")
    {
        screen = std::make_shared<LocateScreen>(mpc, layerIndex);
    }

    if (screen)
    {
        screen->findBackground()->addChildren(children);
        screen->setTransferMap(transferMap);
        screen->setFirstField(firstField);
        screens[screenName] = screen;
    }

    return screen;
}
