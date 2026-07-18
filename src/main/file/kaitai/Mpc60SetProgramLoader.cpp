#include "file/kaitai/Mpc60SetProgramLoader.hpp"

#include "Mpc.hpp"
#include "disk/MpcFile.hpp"
#include "file/kaitai/Mpc60SetPreview.hpp"
#include "file/kaitai/Mpc60SetSoundLoader.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "performance/PerformanceManager.hpp"
#include "performance/PerformanceMessage.hpp"
#include "sampler/Pad.hpp"
#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"
#include "sampler/SoundGenerationMode.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"
#include "StrUtil.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <thread>
#include <vector>

using namespace mpc::file::kaitai;

namespace
{
constexpr int kImportedMpc60HihatVelocityThreshold1 = 14;
constexpr int kImportedMpc60HihatVelocityThreshold2 = 42;
constexpr int kImportedMpc60HihatSliderDecayLow = 12;
constexpr int kImportedMpc60HihatSliderDecayHigh = 45;


mpc::DrumMixerLevel toMixerPercent(const uint8_t value)
{
    return mpc::DrumMixerLevel(std::clamp(
        static_cast<int>(std::lround(static_cast<double>(value) * 100.0 /
                                     127.0)),
        0, 100));
}

int8_t tuneFromPitchFactor(const uint16_t pitchFactor)
{
    if (pitchFactor == 0)
    {
        return 0;
    }

    return static_cast<int8_t>(std::clamp(
        static_cast<int>(std::lround(
            120.0 * std::log2(static_cast<double>(pitchFactor) / 4096.0))),
        -120, 120));
}

void configureImportedHihatSwitching(
    const Mpc60SetPreview &preview,
    const Mpc60SetProgramLoader::ConversionTable &conversionTable,
    const std::shared_ptr<mpc::sampler::Program> &program,
    mpc::performance::Program &perfProgram)
{
    if (preview.soundDirectoryEntryIndexByMpc60Pad.size() < 3 ||
        conversionTable.size() < 3)
    {
        return;
    }

    const auto *closed = preview.assignedSoundAtMpc60Pad(0);
    const auto *medium = preview.assignedSoundAtMpc60Pad(1);
    const auto *open = preview.assignedSoundAtMpc60Pad(2);

    if (closed == nullptr || medium == nullptr || open == nullptr ||
        !closed->isHihat || !medium->isHihat || !open->isHihat)
    {
        return;
    }

    auto &closedHatNoteParams =
        perfProgram.noteParameters[conversionTable[0].get() -
                                   mpc::MinDrumNoteNumber.get()];
    closedHatNoteParams.soundGenerationMode = mpc::sampler::toRaw(
        mpc::sampler::SoundGenerationMode::DecaySwitch);
    closedHatNoteParams.velocityRangeLower =
        kImportedMpc60HihatVelocityThreshold1;
    closedHatNoteParams.velocityRangeUpper =
        kImportedMpc60HihatVelocityThreshold2;
    closedHatNoteParams.optionalNoteA = conversionTable[1];
    closedHatNoteParams.optionalNoteB = conversionTable[2];
    closedHatNoteParams.muteAssignA = conversionTable[1];
    closedHatNoteParams.muteAssignB = conversionTable[2];
    closedHatNoteParams.sliderParameterNumber = mpc::NoteVariationTypeDecay;

    perfProgram.slider.assignNote = conversionTable[0];
    perfProgram.slider.parameter = mpc::NoteVariationTypeDecay;
    perfProgram.slider.decayLowRange = kImportedMpc60HihatSliderDecayLow;
    perfProgram.slider.decayHighRange = kImportedMpc60HihatSliderDecayHigh;

    if (program != nullptr)
    {
        auto *slider = program->getSlider();
        slider->setAssignNote(conversionTable[0]);
        slider->setParameter(mpc::NoteVariationTypeDecay);
        slider->setDecayLowRange(kImportedMpc60HihatSliderDecayLow);
        slider->setDecayHighRange(kImportedMpc60HihatSliderDecayHigh);
        const auto closedHatNoteParameters =
            program->getNoteParameters(conversionTable[0]);
        closedHatNoteParameters->setMuteAssignA(conversionTable[1]);
        closedHatNoteParameters->setMuteAssignB(conversionTable[2]);
        closedHatNoteParameters->setSliderParameterNumber(
            mpc::NoteVariationTypeDecay);
    }
}

bool loadIntoProgram(
    mpc::Mpc &mpc,
    const std::shared_ptr<mpc::disk::MpcFile> &file,
    const Mpc60SetPreview &preview,
    const Mpc60SetProgramLoader::ConversionTable &conversionTable,
    const std::shared_ptr<mpc::sampler::Program> &program,
    mpc::performance::Program &perfProgram)
{
    auto sampler = mpc.getSampler();
    const auto existingSoundCount = sampler->getSoundCount();
    std::vector<int> soundIndexByEntry(preview.soundDirectoryEntries.size(), -1);

    for (size_t entryIndex = 0; entryIndex < preview.soundDirectoryEntries.size();
         ++entryIndex)
    {
        const auto soundName = preview.soundDirectoryEntries[entryIndex].name;
        if (preview.soundDirectoryEntries[entryIndex].lengthInSamples == 0)
        {
            continue;
        }

        const auto msg =
            "Loading " +
            mpc::StrUtil::toUpper(mpc::StrUtil::padRight(soundName, " ", 16));
        const auto ls = mpc.getLayeredScreen();
        ls->postToUiThread(mpc::utils::Task(
            [ls, msg]
            {
                ls->showPopup(msg);
            }));
        std::this_thread::sleep_for(
            mpc.getFileOperationTimings().progressDisplay);

        auto sound = sampler->addSound(44100);
        if (sound == nullptr)
        {
            return false;
        }

        const auto result = Mpc60SetSoundLoader::loadSoundDirectoryEntry(
            preview, entryIndex, sound);

        if (!result.has_value())
        {
            sampler->deleteSound(sound);
            return false;
        }

        soundIndexByEntry[entryIndex] =
            existingSoundCount + static_cast<int>(entryIndex);
    }

    program->setName(file->getNameWithoutExtension());
    perfProgram.used = true;

    const auto &padNotes = mpc::sampler::Pad::getPadNotes(mpc);
    for (int8_t padIndex = 0;
         padIndex < mpc::Mpc2000XlSpecs::PROGRAM_PAD_COUNT;
         ++padIndex)
    {
        perfProgram.pads[padIndex].note = padNotes[padIndex];
    }

    for (size_t mpc60PadIndex = 0;
         mpc60PadIndex < preview.soundDirectoryEntryIndexByMpc60Pad.size() &&
         mpc60PadIndex < conversionTable.size();
         ++mpc60PadIndex)
    {
        const auto entryIndex = preview.soundDirectoryEntryIndexByMpc60Pad[mpc60PadIndex];
        if (entryIndex >= soundIndexByEntry.size())
        {
            continue;
        }

        const auto soundIndex = soundIndexByEntry[entryIndex];

        if (soundIndex < 0)
        {
            continue;
        }

        const auto &entry = preview.soundDirectoryEntries[entryIndex];
        auto &noteParameters =
            perfProgram.noteParameters[conversionTable[mpc60PadIndex].get() -
                                       mpc::MinDrumNoteNumber.get()];
        noteParameters.soundIndex = soundIndex;
        noteParameters.tune = tuneFromPitchFactor(entry.pitchFactor);
        noteParameters.stereoMixer.level =
            toMixerPercent(entry.requestedStereoMixVolume);
        noteParameters.stereoMixer.panning =
            toMixerPercent(entry.requestedStereoMixPan);
    }

    configureImportedHihatSwitching(preview, conversionTable, program, perfProgram);

    if (const auto track = mpc.getSequencer()->getSelectedTrack();
        mpc::sequencer::isDrumBusType(track->getBusType()))
    {
        const auto drumBus = mpc.getSequencer()->getDrumBus(track->getBusType());
        drumBus->setProgramIndex(program->getProgramIndex());
    }

    return true;
}

void resetPerformancePrograms(mpc::Mpc &mpc)
{
    for (int i = 0; i < mpc::Mpc2000XlSpecs::MAX_PROGRAM_COUNT; ++i)
    {
        mpc::performance::UpdateProgramBulk msg;
        msg.programIndex = mpc::ProgramIndex(i);
        mpc.getPerformanceManager().lock()->enqueue(
            mpc::performance::PerformanceMessage{std::move(msg)});
    }
}
} // namespace

Mpc60SetProgramLoader::ConversionTable
Mpc60SetProgramLoader::defaultConversionTable()
{
    return ConversionTable{
        DrumNoteNumber(42), DrumNoteNumber(82), DrumNoteNumber(46),
        DrumNoteNumber(38), DrumNoteNumber(37), DrumNoteNumber(36),
        DrumNoteNumber(48), DrumNoteNumber(47), DrumNoteNumber(45),
        DrumNoteNumber(43), DrumNoteNumber(51), DrumNoteNumber(53),
        DrumNoteNumber(49), DrumNoteNumber(55), DrumNoteNumber(69),
        DrumNoteNumber(54), DrumNoteNumber(56), DrumNoteNumber(39),
        DrumNoteNumber(57), DrumNoteNumber(58), DrumNoteNumber(59),
        DrumNoteNumber(60), DrumNoteNumber(61), DrumNoteNumber(62),
        DrumNoteNumber(63), DrumNoteNumber(64), DrumNoteNumber(65),
        DrumNoteNumber(66), DrumNoteNumber(67), DrumNoteNumber(68),
        DrumNoteNumber(70), DrumNoteNumber(71), DrumNoteNumber(72),
        DrumNoteNumber(73)};
}

Mpc60SetProgramLoader::ConversionTable
Mpc60SetProgramLoader::defaultConversionTable(const mpc::Mpc &mpc)
{
    auto result = defaultConversionTable();
    const auto &originalPadNotes = mpc::sampler::Pad::getOriginalPadNotes();
    const auto &activePadNotes = mpc::sampler::Pad::getPadNotes(mpc);

    for (auto &note : result)
    {
        const auto originalPadIt =
            std::find(originalPadNotes.begin(), originalPadNotes.end(), note);

        if (originalPadIt == originalPadNotes.end())
        {
            continue;
        }

        const auto padIndex =
            static_cast<size_t>(std::distance(originalPadNotes.begin(),
                                              originalPadIt));

        if (padIndex < activePadNotes.size())
        {
            note = activePadNotes[padIndex];
        }
    }

    return result;
}

bool Mpc60SetProgramLoader::load(
    mpc::Mpc &mpc,
    const std::shared_ptr<mpc::disk::MpcFile> &file,
    const Mpc60SetPreview &preview,
    const ConversionTable &conversionTable,
    const bool clearExisting)
{
    auto sampler = mpc.getSampler();

    if (clearExisting)
    {
        sampler->deleteAllSamples();
        resetPerformancePrograms(mpc);

        mpc::performance::UpdateProgramBulk msg;
        msg.programIndex = mpc::ProgramIndex(0);
        const auto loaded = loadIntoProgram(mpc, file, preview, conversionTable,
                                            mpc.getSampler()->getProgram(0),
                                            msg.program);
        if (loaded)
        {
            mpc.getPerformanceManager().lock()->enqueue(
                mpc::performance::PerformanceMessage{std::move(msg)});
        }
        return loaded;
    }

    std::shared_ptr<mpc::sampler::Program> program;
    for (int i = 0; i < mpc::Mpc2000XlSpecs::MAX_PROGRAM_COUNT; ++i)
    {
        const auto candidate = sampler->getProgram(i);
        if (!candidate->isUsed())
        {
            program = candidate;
            break;
        }
    }

    if (!program)
    {
        return false;
    }

    mpc::performance::UpdateProgramBulk msg;
    msg.programIndex = program->getProgramIndex();
    const auto loaded = loadIntoProgram(mpc, file, preview, conversionTable,
                                        program, msg.program);
    if (loaded)
    {
        mpc.getPerformanceManager().lock()->enqueue(
            mpc::performance::PerformanceMessage{std::move(msg)});
    }
    return loaded;
}
