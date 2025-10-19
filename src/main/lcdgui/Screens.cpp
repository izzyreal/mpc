#include "Screens.hpp"

#include "Logger.hpp"

#include "lcdgui/ScreenNames.h"

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
#include <lcdgui/screens/window/StepEditOptionsScreen.hpp>
#include <lcdgui/screens/window/ConvertSongToSeqScreen.hpp>
#include <lcdgui/screens/window/LocateScreen.hpp>
#include <lcdgui/screens/window/VmpcWarningSettingsIgnoredScreen.hpp>
#include <lcdgui/screens/window/VmpcKnownControllerDetectedScreen.hpp>
#include <lcdgui/screens/window/VmpcContinuePreviousSessionScreen.hpp>
#include <lcdgui/screens/window/VmpcConvertAndLoadWavScreen.hpp>
#include <lcdgui/screens/window/VmpcDiscardMappingChangesScreen.hpp>
#include <lcdgui/screens/window/VmpcRecordingFinishedScreen.hpp>
#include <lcdgui/screens/window/VmpcResetKeyboardScreen.hpp>
#include <lcdgui/screens/window/VmpcDirectToDiskRecorderScreen.hpp>
#include <lcdgui/screens/window/VmpcMidiPresetsScreen.hpp>

#include <lcdgui/screens/dialog/MetronomeSoundScreen.hpp>
#include <lcdgui/screens/dialog/MidiInputMonitorScreen.hpp>
#include <lcdgui/screens/dialog/MidiOutputMonitorScreen.hpp>
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

#include <StrUtil.hpp>

#include "MpcResourceUtil.hpp"

#include <rapidjson/document.h>

#include <functional>

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

void Screens::createAndCacheAllScreens()
{
    for (auto screenName : screenNames)
    {
        if (std::find(knownUnimplementedScreens.begin(), knownUnimplementedScreens.end(), screenName) != knownUnimplementedScreens.end())
        {
            continue;
        }

        createAndCacheScreen(screenName);
    }

    MLOG("Created and cached " + std::to_string(screens.size()) + " screens.");
}

std::shared_ptr<ScreenComponent> Screens::getByName(const std::string name)
{
    for (auto& screen : screens)
    {
        if (screen->getName() == name)
        {
            return screen;
        }
    }

    return {};
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

std::optional<Screens::ScreenLayout> Screens::getScreenLayout(const std::string& screenName)
{
    Screens::ScreenLayout result;

    for (int i = 0; i < 4; i++)
    {
        if (layerDocuments()[i]->HasMember(screenName.c_str()))
        {
            result.layerIndex = i;
            break;
        }
    }

    if (result.layerIndex < 0)
    {
        return std::nullopt;
    }

    Value &arrangement = layerDocuments()[result.layerIndex]->GetObj()[screenName.c_str()];

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
        result.components.push_back(std::move(functionKeysComponent));
    }

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
                {
                    parameterTransferMap.push_back(parameters[i][j].GetString());
                }

                result.transferMap[previous] = parameterTransferMap;
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

            result.components.push_back(std::make_unique<Parameter>(
                    mpc,
                    labels[i - skipCounter].GetString(),
                    parameterName,
                    x[i - skipCounter].GetInt(),
                    y[i - skipCounter].GetInt(),
                    tfsize[i - skipCounter].GetInt()));

            auto parameter = result.components.back();
            auto field = parameter->findField(parameterName);

            if (i == 0)
            {
                result.firstField = parameterName;
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
            auto label = std::make_shared<Label>(mpc,
                                                 infoNames[i].GetString(),
                                                 "",
                                                 infoX[i].GetInt(),
                                                 infoY[i].GetInt(),
                                                 infoSize[i].GetInt());
            result.components.push_back(label);
        }
    }

    return result;
}

using ScreenFactory = std::function<std::shared_ptr<ScreenComponent>(mpc::Mpc&, int)>;

static const std::map<std::string, ScreenFactory> screenFactories = {
    { "sequencer", [](mpc::Mpc& mpc, int layer){ return std::make_shared<SequencerScreen>(mpc, layer); } },
    { "sequence", [](mpc::Mpc& mpc, int layer){ return std::make_shared<SequenceScreen>(mpc, layer); } },
    { "tempo-change", [](mpc::Mpc& mpc, int layer){ return std::make_shared<TempoChangeScreen>(mpc, layer); } },
    { "count-metronome", [](mpc::Mpc& mpc, int layer){ return std::make_shared<CountMetronomeScreen>(mpc, layer); } },
    { "edit-multiple", [](mpc::Mpc& mpc, int layer){ return std::make_shared<EditMultipleScreen>(mpc, layer); } },
    { "transmit-program-changes", [](mpc::Mpc& mpc, int layer){ return std::make_shared<TransmitProgramChangesScreen>(mpc, layer); } },
    { "timing-correct", [](mpc::Mpc& mpc, int layer){ return std::make_shared<TimingCorrectScreen>(mpc, layer); } },
    { "time-display", [](mpc::Mpc& mpc, int layer){ return std::make_shared<TimeDisplayScreen>(mpc, layer); } },
    { "paste-event", [](mpc::Mpc& mpc, int layer){ return std::make_shared<PasteEventScreen>(mpc, layer); } },
    { "multi-recording-setup", [](mpc::Mpc& mpc, int layer){ return std::make_shared<MultiRecordingSetupScreen>(mpc, layer); } },
    { "midi-output", [](mpc::Mpc& mpc, int layer){ return std::make_shared<MidiOutputScreen>(mpc, layer); } },
    { "midi-input", [](mpc::Mpc& mpc, int layer){ return std::make_shared<MidiInputScreen>(mpc, layer); } },
    { "loop-bars-window", [](mpc::Mpc& mpc, int layer){ return std::make_shared<LoopBarsScreen>(mpc, layer); } },
    { "insert-event", [](mpc::Mpc& mpc, int layer){ return std::make_shared<InsertEventScreen>(mpc, layer); } },
    { "erase-all-off-tracks", [](mpc::Mpc& mpc, int layer){ return std::make_shared<EraseAllOffTracksScreen>(mpc, layer); } },
    { "change-tsig", [](mpc::Mpc& mpc, int layer){ return std::make_shared<ChangeTsigScreen>(mpc, layer); } },
    { "edit-velocity", [](mpc::Mpc& mpc, int layer){ return std::make_shared<EditVelocityScreen>(mpc, layer); } },
    { "erase", [](mpc::Mpc& mpc, int layer){ return std::make_shared<EraseScreen>(mpc, layer); } },
    { "change-bars", [](mpc::Mpc& mpc, int layer){ return std::make_shared<ChangeBarsScreen>(mpc, layer); } },
    { "change-bars-2", [](mpc::Mpc& mpc, int layer){ return std::make_shared<ChangeBars2Screen>(mpc, layer); } },
    { "track", [](mpc::Mpc& mpc, int layer){ return std::make_shared<TrackScreen>(mpc, layer); } },
    { "assign-16-levels", [](mpc::Mpc& mpc, int layer){ return std::make_shared<Assign16LevelsScreen>(mpc, layer); } },
    { "midi-input-monitor", [](mpc::Mpc& mpc, int layer){ return std::make_shared<MidiInputMonitorScreen>(mpc, layer); } },
    { "midi-output-monitor", [](mpc::Mpc& mpc, int layer){ return std::make_shared<MidiOutputMonitorScreen>(mpc, layer); } },
    { "metronome-sound", [](mpc::Mpc& mpc, int layer){ return std::make_shared<MetronomeSoundScreen>(mpc, layer); } },
    { "copy-sequence", [](mpc::Mpc& mpc, int layer){ return std::make_shared<CopySequenceScreen>(mpc, layer); } },
    { "copy-track", [](mpc::Mpc& mpc, int layer){ return std::make_shared<CopyTrackScreen>(mpc, layer); } },
    { "delete-sequence", [](mpc::Mpc& mpc, int layer){ return std::make_shared<DeleteSequenceScreen>(mpc, layer); } },
    { "delete-track", [](mpc::Mpc& mpc, int layer){ return std::make_shared<DeleteTrackScreen>(mpc, layer); } },
    { "delete-all-sequences", [](mpc::Mpc& mpc, int layer){ return std::make_shared<DeleteAllSequencesScreen>(mpc, layer); } },
    { "delete-all-tracks", [](mpc::Mpc& mpc, int layer){ return std::make_shared<DeleteAllTracksScreen>(mpc, layer); } },
    { "next-seq-pad", [](mpc::Mpc& mpc, int layer){ return std::make_shared<NextSeqPadScreen>(mpc, layer); } },
    { "next-seq", [](mpc::Mpc& mpc, int layer){ return std::make_shared<NextSeqScreen>(mpc, layer); } },
    { "song", [](mpc::Mpc& mpc, int layer){ return std::make_shared<SongScreen>(mpc, layer); } },
    { "track-mute", [](mpc::Mpc& mpc, int layer){ return std::make_shared<TrMuteScreen>(mpc, layer); } },
    { "step-editor", [](mpc::Mpc& mpc, int layer){ return std::make_shared<StepEditorScreen>(mpc, layer); } },
    { "events", [](mpc::Mpc& mpc, int layer){ return std::make_shared<EventsScreen>(mpc, layer); } },
    { "bars", [](mpc::Mpc& mpc, int layer){ return std::make_shared<BarsScreen>(mpc, layer); } },
    { "tr-move", [](mpc::Mpc& mpc, int layer){ return std::make_shared<TrMoveScreen>(mpc, layer); } },
    { "user", [](mpc::Mpc& mpc, int layer){ return std::make_shared<UserScreen>(mpc, layer); } },
    { "assign", [](mpc::Mpc& mpc, int layer){ return std::make_shared<AssignScreen>(mpc, layer); } },
    { "step-timing-correct", [](mpc::Mpc& mpc, int layer){ return std::make_shared<StepTcScreen>(mpc, layer); } },
    { "others", [](mpc::Mpc& mpc, int layer){ return std::make_shared<OthersScreen>(mpc, layer); } },
    { "purge", [](mpc::Mpc& mpc, int layer){ return std::make_shared<PurgeScreen>(mpc, layer); } },
    { "drum", [](mpc::Mpc& mpc, int layer){ return std::make_shared<DrumScreen>(mpc, layer); } },
    { "trim", [](mpc::Mpc& mpc, int layer){ return std::make_shared<TrimScreen>(mpc, layer); } },
    { "loop", [](mpc::Mpc& mpc, int layer){ return std::make_shared<LoopScreen>(mpc, layer); } },
    { "zone", [](mpc::Mpc& mpc, int layer){ return std::make_shared<ZoneScreen>(mpc, layer); } },
    { "number-of-zones", [](mpc::Mpc& mpc, int layer){ return std::make_shared<NumberOfZonesScreen>(mpc, layer); } },
    { "params", [](mpc::Mpc& mpc, int layer){ return std::make_shared<SndParamsScreen>(mpc, layer); } },
    { "program-params", [](mpc::Mpc& mpc, int layer){ return std::make_shared<PgmParamsScreen>(mpc, layer); } },
    { "program-assign", [](mpc::Mpc& mpc, int layer){ return std::make_shared<PgmAssignScreen>(mpc, layer); } },
    { "select-drum", [](mpc::Mpc& mpc, int layer){ return std::make_shared<SelectDrumScreen>(mpc, layer); } },
    { "sample", [](mpc::Mpc& mpc, int layer){ return std::make_shared<SampleScreen>(mpc, layer); } },
    { "sound", [](mpc::Mpc& mpc, int layer){ return std::make_shared<SoundScreen>(mpc, layer); } },
    { "convert-sound", [](mpc::Mpc& mpc, int layer){ return std::make_shared<ConvertSoundScreen>(mpc, layer); } },
    { "copy-sound", [](mpc::Mpc& mpc, int layer){ return std::make_shared<CopySoundScreen>(mpc, layer); } },
    { "delete-sound", [](mpc::Mpc& mpc, int layer){ return std::make_shared<DeleteSoundScreen>(mpc, layer); } },
    { "mono-to-stereo", [](mpc::Mpc& mpc, int layer){ return std::make_shared<MonoToStereoScreen>(mpc, layer); } },
    { "resample", [](mpc::Mpc& mpc, int layer){ return std::make_shared<ResampleScreen>(mpc, layer); } },
    { "stereo-to-mono", [](mpc::Mpc& mpc, int layer){ return std::make_shared<StereoToMonoScreen>(mpc, layer); } },
    { "mixer", [](mpc::Mpc& mpc, int layer){ return std::make_shared<MixerScreen>(mpc, layer); } },
    { "mixer-setup", [](mpc::Mpc& mpc, int layer){ return std::make_shared<MixerSetupScreen>(mpc, layer); } },
    { "select-mixer-drum", [](mpc::Mpc& mpc, int layer){ return std::make_shared<SelectMixerDrumScreen>(mpc, layer); } },
    { "fx-edit", [](mpc::Mpc& mpc, int layer){ return std::make_shared<FxEditScreen>(mpc, layer); } },
    { "start-fine", [](mpc::Mpc& mpc, int layer){ return std::make_shared<StartFineScreen>(mpc, layer); } },
    { "end-fine", [](mpc::Mpc& mpc, int layer){ return std::make_shared<EndFineScreen>(mpc, layer); } },
    { "loop-to-fine", [](mpc::Mpc& mpc, int layer){ return std::make_shared<LoopToFineScreen>(mpc, layer); } },
    { "loop-end-fine", [](mpc::Mpc& mpc, int layer){ return std::make_shared<LoopEndFineScreen>(mpc, layer); } },
    { "zone-start-fine", [](mpc::Mpc& mpc, int layer){ return std::make_shared<ZoneStartFineScreen>(mpc, layer); } },
    { "zone-end-fine", [](mpc::Mpc& mpc, int layer){ return std::make_shared<ZoneEndFineScreen>(mpc, layer); } },
    { "channel-settings", [](mpc::Mpc& mpc, int layer){ return std::make_shared<ChannelSettingsScreen>(mpc, layer); } },
    { "edit-sound", [](mpc::Mpc& mpc, int layer){ return std::make_shared<EditSoundScreen>(mpc, layer); } },
    { "assignment-view", [](mpc::Mpc& mpc, int layer){ return std::make_shared<AssignmentViewScreen>(mpc, layer); } },
    { "auto-chromatic-assignment", [](mpc::Mpc& mpc, int layer){ return std::make_shared<AutoChromaticAssignmentScreen>(mpc, layer); } },
    { "copy-note-parameters", [](mpc::Mpc& mpc, int layer){ return std::make_shared<CopyNoteParametersScreen>(mpc, layer); } },
    { "keep-or-retry", [](mpc::Mpc& mpc, int layer){ return std::make_shared<KeepOrRetryScreen>(mpc, layer); } },
    { "mute-assign", [](mpc::Mpc& mpc, int layer){ return std::make_shared<MuteAssignScreen>(mpc, layer); } },
    { "program", [](mpc::Mpc& mpc, int layer){ return std::make_shared<ProgramScreen>(mpc, layer); } },
    { "velocity-modulation", [](mpc::Mpc& mpc, int layer){ return std::make_shared<VelocityModulationScreen>(mpc, layer); } },
    { "velo-env-filter", [](mpc::Mpc& mpc, int layer){ return std::make_shared<VeloEnvFilterScreen>(mpc, layer); } },
    { "velo-pitch", [](mpc::Mpc& mpc, int layer){ return std::make_shared<VeloPitchScreen>(mpc, layer); } },
    { "init-pad-assign", [](mpc::Mpc& mpc, int layer){ return std::make_shared<InitPadAssignScreen>(mpc, layer); } },
    { "create-new-program", [](mpc::Mpc& mpc, int layer){ return std::make_shared<CreateNewProgramScreen>(mpc, layer); } },
    { "copy-program", [](mpc::Mpc& mpc, int layer){ return std::make_shared<CopyProgramScreen>(mpc, layer); } },
    { "delete-all-programs", [](mpc::Mpc& mpc, int layer){ return std::make_shared<DeleteAllProgramsScreen>(mpc, layer); } },
    { "delete-all-sound", [](mpc::Mpc& mpc, int layer){ return std::make_shared<DeleteAllSoundScreen>(mpc, layer); } },
    { "delete-program", [](mpc::Mpc& mpc, int layer){ return std::make_shared<DeleteProgramScreen>(mpc, layer); } },
    { "load", [](mpc::Mpc& mpc, int layer){ return std::make_shared<LoadScreen>(mpc, layer); } },
    { "format", [](mpc::Mpc& mpc, int layer){ return std::make_shared<FormatScreen>(mpc, layer); } },
    { "setup", [](mpc::Mpc& mpc, int layer){ return std::make_shared<SetupScreen>(mpc, layer); } },
    { "save", [](mpc::Mpc& mpc, int layer){ return std::make_shared<SaveScreen>(mpc, layer); } },
    { "directory", [](mpc::Mpc& mpc, int layer){ return std::make_shared<DirectoryScreen>(mpc, layer); } },
    { "load-a-program", [](mpc::Mpc& mpc, int layer){ return std::make_shared<LoadAProgramScreen>(mpc, layer); } },
    { "load-a-sequence", [](mpc::Mpc& mpc, int layer){ return std::make_shared<LoadASequenceScreen>(mpc, layer); } },
    { "load-a-sequence-from-all", [](mpc::Mpc& mpc, int layer){ return std::make_shared<LoadASequenceFromAllScreen>(mpc, layer); } },
    { "load-a-sound", [](mpc::Mpc& mpc, int layer){ return std::make_shared<LoadASoundScreen>(mpc, layer); } },
    { "load-aps-file", [](mpc::Mpc& mpc, int layer){ return std::make_shared<LoadApsFileScreen>(mpc, layer); } },
    { "mpc2000xl-all-file", [](mpc::Mpc& mpc, int layer){ return std::make_shared<Mpc2000XlAllFileScreen>(mpc, layer); } },
    { "save-a-program", [](mpc::Mpc& mpc, int layer){ return std::make_shared<SaveAProgramScreen>(mpc, layer); } },
    { "save-a-sequence", [](mpc::Mpc& mpc, int layer){ return std::make_shared<SaveASequenceScreen>(mpc, layer); } },
    { "save-a-sound", [](mpc::Mpc& mpc, int layer){ return std::make_shared<SaveASoundScreen>(mpc, layer); } },
    { "save-all-file", [](mpc::Mpc& mpc, int layer){ return std::make_shared<SaveAllFileScreen>(mpc, layer); } },
    { "save-aps-file", [](mpc::Mpc& mpc, int layer){ return std::make_shared<SaveApsFileScreen>(mpc, layer); } },
    { "delete-all-files", [](mpc::Mpc& mpc, int layer){ return std::make_shared<DeleteAllFilesScreen>(mpc, layer); } },
    { "delete-file", [](mpc::Mpc& mpc, int layer){ return std::make_shared<DeleteFileScreen>(mpc, layer); } },
    { "delete-folder", [](mpc::Mpc& mpc, int layer){ return std::make_shared<DeleteFolderScreen>(mpc, layer); } },
    { "cant-find-file", [](mpc::Mpc& mpc, int layer){ return std::make_shared<CantFindFileScreen>(mpc, layer); } },
    { "file-exists", [](mpc::Mpc& mpc, int layer){ return std::make_shared<FileExistsScreen>(mpc, layer); } },
    { "name", [](mpc::Mpc& mpc, int layer){ return std::make_shared<NameScreen>(mpc, layer); } },
    { "vmpc-disks", [](mpc::Mpc& mpc, int layer){ return std::make_shared<VmpcDisksScreen>(mpc, layer); } },
    { "vmpc-settings", [](mpc::Mpc& mpc, int layer){ return std::make_shared<VmpcSettingsScreen>(mpc, layer); } },
    { "vmpc-direct-to-disk-recorder", [](mpc::Mpc& mpc, int layer){ return std::make_shared<VmpcDirectToDiskRecorderScreen>(mpc, layer); } },
    { "vmpc-record-jam", [](mpc::Mpc& mpc, int layer){ return std::make_shared<VmpcRecordJamScreen>(mpc, layer); } },
    { "vmpc-recording-finished", [](mpc::Mpc& mpc, int layer){ return std::make_shared<VmpcRecordingFinishedScreen>(mpc, layer); } },
    { "ver", [](mpc::Mpc& mpc, int layer){ return std::make_shared<VerScreen>(mpc, layer); } },
    { "init", [](mpc::Mpc& mpc, int layer){ return std::make_shared<InitScreen>(mpc, layer); } },
    { "second-seq", [](mpc::Mpc& mpc, int layer){ return std::make_shared<SecondSeqScreen>(mpc, layer); } },
    { "trans", [](mpc::Mpc& mpc, int layer){ return std::make_shared<TransScreen>(mpc, layer); } },
    { "transpose-permanent", [](mpc::Mpc& mpc, int layer){ return std::make_shared<TransposePermanentScreen>(mpc, layer); } },
    { "punch", [](mpc::Mpc& mpc, int layer){ return std::make_shared<PunchScreen>(mpc, layer); } },
    { "sync", [](mpc::Mpc& mpc, int layer){ return std::make_shared<SyncScreen>(mpc, layer); } },
    { "popup", [](mpc::Mpc& mpc, int layer){ return std::make_shared<PopupScreen>(mpc); } },
    { "sound-memory", [](mpc::Mpc& mpc, int layer){ return std::make_shared<SoundMemoryScreen>(mpc, layer); } },
    { "song-window", [](mpc::Mpc& mpc, int layer){ return std::make_shared<SongWindow>(mpc, layer); } },
    { "vmpc-file-in-use", [](mpc::Mpc& mpc, int layer){ return std::make_shared<VmpcFileInUseScreen>(mpc, layer); } },
    { "ignore-tempo-change", [](mpc::Mpc& mpc, int layer){ return std::make_shared<IgnoreTempoChangeScreen>(mpc, layer); } },
    { "loop-song", [](mpc::Mpc& mpc, int layer){ return std::make_shared<LoopSongScreen>(mpc, layer); } },
    { "delete-song", [](mpc::Mpc& mpc, int layer){ return std::make_shared<DeleteSongScreen>(mpc, layer); } },
    { "delete-all-song", [](mpc::Mpc& mpc, int layer){ return std::make_shared<DeleteAllSongScreen>(mpc, layer); } },
    { "copy-song", [](mpc::Mpc& mpc, int layer){ return std::make_shared<CopySongScreen>(mpc, layer); } },
    { "vmpc-keyboard", [](mpc::Mpc& mpc, int layer){ return std::make_shared<VmpcKeyboardScreen>(mpc, layer); } },
    { "vmpc-midi", [](mpc::Mpc& mpc, int layer){ return std::make_shared<VmpcMidiScreen>(mpc, layer); } },
    { "vmpc-reset-keyboard", [](mpc::Mpc& mpc, int layer){ return std::make_shared<VmpcResetKeyboardScreen>(mpc, layer); } },
    { "vmpc-discard-mapping-changes", [](mpc::Mpc& mpc, int layer){ return std::make_shared<VmpcDiscardMappingChangesScreen>(mpc, layer); } },
    { "vmpc-auto-save", [](mpc::Mpc& mpc, int layer){ return std::make_shared<VmpcAutoSaveScreen>(mpc, layer); } },
    { "midi-sw", [](mpc::Mpc& mpc, int layer){ return std::make_shared<MidiSwScreen>(mpc, layer); } },
    { "vmpc-convert-and-load-wav", [](mpc::Mpc& mpc, int layer){ return std::make_shared<VmpcConvertAndLoadWavScreen>(mpc, layer); } },
    { "step-edit-options", [](mpc::Mpc& mpc, int layer){ return std::make_shared<StepEditOptionsScreen>(mpc, layer); } },
    { "vmpc-midi-presets", [](mpc::Mpc& mpc, int layer){ return std::make_shared<VmpcMidiPresetsScreen>(mpc, layer); } },
    { "vmpc-warning-settings-ignored", [](mpc::Mpc& mpc, int layer){ return std::make_shared<VmpcWarningSettingsIgnoredScreen>(mpc, layer); } },
    { "vmpc-known-controller-detected", [](mpc::Mpc& mpc, int layer){ return std::make_shared<VmpcKnownControllerDetectedScreen>(mpc, layer); } },
    { "vmpc-continue-previous-session", [](mpc::Mpc& mpc, int layer){ return std::make_shared<VmpcContinuePreviousSessionScreen>(mpc, layer); } },
    { "convert-song-to-seq", [](mpc::Mpc& mpc, int layer){ return std::make_shared<ConvertSongToSeqScreen>(mpc, layer); } },
    { "locate", [](mpc::Mpc& mpc, int layer){ return std::make_shared<LocateScreen>(mpc, layer); } },
};

// screenName -> layerIndex
static const std::map<std::string, int> screensWithoutLayoutJson {
    { "popup", 4 }
};

void Screens::createAndCacheScreen(const std::string &screenName)
{
    if (const auto screenFactory = screenFactories.find(screenName); screenFactory != screenFactories.end())
    {
        if (auto screenWithoutLayoutJson = screensWithoutLayoutJson.find(screenName); screenWithoutLayoutJson != screensWithoutLayoutJson.end())
        {
            auto screen = screenFactory->second(mpc, screenWithoutLayoutJson->second);
            screens.push_back(screen);
        }

        auto layout = getScreenLayout(screenName);

        if (!layout)
        {
            MLOG("Screens::getOrCreateScreenComponent has the requested screen name '" + screenName + "' in its map, and a ScreenComponent subclass for it is available in the mpc::lcdgui::screens namespace, but the layout can't be found. Most likely the layout is missing from screens/layer1.json, screens/layer2.json, screens/layer3.json or screens/layer4.json.");
            return; 
        }

        auto screen = screenFactory->second(mpc, layout->layerIndex);
        screen->findBackground()->addChildren(layout->components);
        screen->setTransferMap(layout->transferMap);
        screen->setFirstField(layout->firstField);
        screens.push_back(screen);
    }

    MLOG("Screens::getOrCreateScreenComponent is not familiar with screen name '" + screenName + "'. Add it to src/main/lcdgui/Screens.cpp");
}

