#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"
#include "engine/EngineHost.hpp"
#include "file/kaitai/Mpc3000SeqIo.hpp"
#include "command/ReleaseFunctionCommand.hpp"
#include "lcdgui/Label.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/LoadScreen.hpp"
#include "lcdgui/screens/window/LoadASequenceScreen.hpp"
#include "sequencer/BusType.hpp"
#include "sequencer/ChannelPressureEvent.hpp"
#include "sequencer/ControlChangeEvent.hpp"
#include "sequencer/MixerEvent.hpp"
#include "sequencer/NoteOnEvent.hpp"
#include "sequencer/PolyPressureEvent.hpp"
#include "sequencer/PitchBendEvent.hpp"
#include "sequencer/ProgramChangeEvent.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/SystemExclusiveEvent.hpp"
#include "sequencer/TempoChangeEvent.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/Transport.hpp"

#include <cmrc/cmrc.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <memory>
#include <string>
#include <thread>
#include <vector>

CMRC_DECLARE(mpctest);

namespace
{
constexpr size_t kMpc3000SeqFixedPreludeSize =
    2 + 37 + 5 + (64 * 4) + 2 + 15 + 3 + 16 + 1 + 1 + 1;
constexpr size_t kMpc3000SeqTrackHeaderSize = 24;
constexpr size_t kMpc3000SeqTempoChangeSize = 6;
constexpr size_t kMpc3000SeqTempoChangeCountOffset =
    kMpc3000SeqFixedPreludeSize - 2;
constexpr size_t kMpc3000SeqTrackHeaderCountOffset =
    kMpc3000SeqFixedPreludeSize - 1;

std::shared_ptr<mpc::disk::MpcFile> prepareSeqFile(
    mpc::Mpc &mpc,
    const std::vector<char> &data,
    const std::string &fileName)
{
    auto mutableData = data;
    auto newFile = mpc.getDisk()->newFile(fileName);
    newFile->setFileData(mutableData);
    mpc.getDisk()->initFiles();
    return newFile;
}

std::shared_ptr<mpc::disk::MpcFile> prepareSeqFile(
    mpc::Mpc &mpc,
    const std::string &resourcePath,
    const std::string &fileName)
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open(resourcePath);
    std::vector<char> data(file.begin(), file.end());
    return prepareSeqFile(mpc, data, fileName);
}

template <typename T>
std::shared_ptr<T> eventAs(const std::shared_ptr<mpc::sequencer::EventRef> &event)
{
    auto cast = std::dynamic_pointer_cast<T>(event);
    REQUIRE(cast);
    return cast;
}

size_t tempoChangeOffset(const std::vector<char> &data, const size_t index)
{
    REQUIRE(data.size() > kMpc3000SeqTrackHeaderCountOffset);
    const auto numberOfTrackHeaders =
        static_cast<unsigned char>(data[kMpc3000SeqTrackHeaderCountOffset]);
    return kMpc3000SeqFixedPreludeSize +
           (numberOfTrackHeaders * kMpc3000SeqTrackHeaderSize) +
           (index * kMpc3000SeqTempoChangeSize);
}

void setTempoChangeFactorPercentage(
    std::vector<char> &data,
    const size_t index,
    const uint16_t factorWord)
{
    const auto offset = tempoChangeOffset(data, index);
    REQUIRE(offset + 5 < data.size());
    data[offset + 4] = static_cast<char>(factorWord & 0xff);
    data[offset + 5] = static_cast<char>((factorWord >> 8) & 0xff);
}

void insertTempoChange(
    std::vector<char> &data,
    const uint32_t tick,
    const uint16_t factorWord)
{
    REQUIRE(data.size() > kMpc3000SeqTempoChangeCountOffset);
    const auto numberOfTempoChanges =
        static_cast<unsigned char>(data[kMpc3000SeqTempoChangeCountOffset]);
    const auto insertOffset = tempoChangeOffset(data, numberOfTempoChanges);

    const std::vector<char> entry{
        static_cast<char>(tick & 0xff),
        static_cast<char>((tick >> 8) & 0xff),
        static_cast<char>((tick >> 16) & 0xff),
        static_cast<char>((tick >> 24) & 0xff),
        static_cast<char>(factorWord & 0xff),
        static_cast<char>((factorWord >> 8) & 0xff),
    };

    data.insert(
        data.begin() + static_cast<std::ptrdiff_t>(insertOffset),
        entry.begin(), entry.end());
    data[kMpc3000SeqTempoChangeCountOffset] =
        static_cast<char>(numberOfTempoChanges + 1);
}

void setLoopToBar(std::vector<char> &data, const bool enabled,
                  const uint16_t loopBarNumber)
{
    REQUIRE(data.size() > 30);
    auto flags = static_cast<unsigned char>(data[28]);
    flags = static_cast<unsigned char>((flags & 0xfe) | (enabled ? 0x01 : 0x00));
    data[28] = static_cast<char>(flags);
    data[29] = static_cast<char>(loopBarNumber & 0xff);
    data[30] = static_cast<char>((loopBarNumber >> 8) & 0xff);
}

size_t trackHeaderOffset(const std::vector<char> &data, const size_t index)
{
    const auto numberOfTrackHeaders =
        static_cast<unsigned char>(data[kMpc3000SeqTrackHeaderCountOffset]);
    REQUIRE(index < numberOfTrackHeaders);
    return kMpc3000SeqFixedPreludeSize + (index * kMpc3000SeqTrackHeaderSize);
}

size_t eventStreamOffset(const std::vector<char> &data)
{
    REQUIRE(data.size() > kMpc3000SeqTrackHeaderCountOffset);
    const auto numberOfTempoChanges =
        static_cast<unsigned char>(data[kMpc3000SeqTempoChangeCountOffset]);
    const auto numberOfTrackHeaders =
        static_cast<unsigned char>(data[kMpc3000SeqTrackHeaderCountOffset]);
    return kMpc3000SeqFixedPreludeSize +
           (numberOfTrackHeaders * kMpc3000SeqTrackHeaderSize) +
           (numberOfTempoChanges * kMpc3000SeqTempoChangeSize);
}

void setTrackHeaderFlags(
    std::vector<char> &data,
    const size_t index,
    const bool muted,
    const bool inUse,
    const bool drumTrack)
{
    const auto offset = trackHeaderOffset(data, index);
    auto flags = static_cast<unsigned char>(data[offset + 2]);
    flags = static_cast<unsigned char>(flags & 0xf8);
    if (muted)
    {
        flags = static_cast<unsigned char>(flags | 0x01);
    }
    if (inUse)
    {
        flags = static_cast<unsigned char>(flags | 0x02);
    }
    if (drumTrack)
    {
        flags = static_cast<unsigned char>(flags | 0x04);
    }
    data[offset + 2] = static_cast<char>(flags);
}

void setTrackPrimaryPortChannelAssignment(
    std::vector<char> &data,
    const size_t index,
    const uint8_t assignment)
{
    const auto offset = trackHeaderOffset(data, index);
    data[offset + 3] = static_cast<char>(assignment);
}

void setTrackProgramChangeNumber(
    std::vector<char> &data,
    const size_t index,
    const uint8_t programChangeNumber)
{
    const auto offset = trackHeaderOffset(data, index);
    data[offset + 22] = static_cast<char>(programChangeNumber);
}

void setTrackName(
    std::vector<char> &data,
    const size_t index,
    const std::string &name)
{
    const auto offset = trackHeaderOffset(data, index);
    auto paddedName = mpc::StrUtil::padRight(name, " ", 16);
    paddedName.resize(16);
    std::copy(
        paddedName.begin(), paddedName.end(), data.begin() + offset + 5);
}

void configureTrackHeader(
    std::vector<char> &data,
    const size_t index,
    const int8_t absoluteRecordedTrackNumber,
    const uint8_t userTrackNumber,
    const bool muted,
    const bool inUse,
    const bool drumTrack,
    const uint8_t primaryPortChannelAssignment,
    const int8_t secondaryPortChannelAssignment,
    const std::string &name,
    const uint8_t trackVolume,
    const uint8_t programChangeNumber)
{
    auto offset = trackHeaderOffset(data, index);
    data[offset] = static_cast<char>(absoluteRecordedTrackNumber);
    data[offset + 1] = static_cast<char>(userTrackNumber);

    unsigned char flags = 0;
    if (muted)
    {
        flags |= 0x01;
    }
    if (inUse)
    {
        flags |= 0x02;
    }
    if (drumTrack)
    {
        flags |= 0x04;
    }
    data[offset + 2] = static_cast<char>(flags);
    data[offset + 3] = static_cast<char>(primaryPortChannelAssignment);
    data[offset + 4] = static_cast<char>(secondaryPortChannelAssignment);

    auto paddedName = mpc::StrUtil::padRight(name, " ", 16);
    paddedName.resize(16);
    std::copy(
        paddedName.begin(), paddedName.end(), data.begin() + offset + 5);

    data[offset + 21] = static_cast<char>(trackVolume);
    data[offset + 22] = static_cast<char>(programChangeNumber);
    data[offset + 23] = 0;
}

void mutateBarEvent(
    std::vector<char> &data,
    const uint16_t oneBasedBarNumber,
    const uint8_t numerator,
    const uint8_t denominator)
{
    const auto offset = eventStreamOffset(data);
    const unsigned char barNumber1 = static_cast<unsigned char>(oneBasedBarNumber & 0x7f);
    const unsigned char barNumber2 =
        static_cast<unsigned char>((oneBasedBarNumber >> 7) & 0x7f);

    for (size_t i = offset; i + 4 < data.size(); ++i)
    {
        if (static_cast<unsigned char>(data[i]) != 0xA8)
        {
            continue;
        }
        if (static_cast<unsigned char>(data[i + 1]) != barNumber1 ||
            static_cast<unsigned char>(data[i + 2]) != barNumber2)
        {
            continue;
        }

        data[i + 3] = static_cast<char>(numerator);
        data[i + 4] = static_cast<char>(denominator);
        return;
    }

    FAIL("Did not find target MPC3000 SEQ bar event to mutate");
}

void replaceProgramChangeWithPitchBend(
    std::vector<char> &data,
    const uint8_t lsb,
    const uint8_t msb)
{
    const std::vector<char> programChangeBytes{
        static_cast<char>(0xC0),
        static_cast<char>(0x01),
        static_cast<char>(0x00),
        static_cast<char>(0x88),
        static_cast<char>(0x00),
        static_cast<char>(0x03),
    };
    const std::vector<char> pitchBendBytes{
        static_cast<char>(0xE0),
        static_cast<char>(0x01),
        static_cast<char>(lsb),
        static_cast<char>(msb),
        static_cast<char>(0x88),
        static_cast<char>(0x00),
        static_cast<char>(0x03),
    };

    const auto eventIt = std::search(
        data.begin(), data.end(), programChangeBytes.begin(), programChangeBytes.end());
    REQUIRE(eventIt != data.end());
    const auto eventOffset = static_cast<size_t>(std::distance(data.begin(), eventIt));
    data.erase(data.begin() + static_cast<std::ptrdiff_t>(eventOffset),
               data.begin() + static_cast<std::ptrdiff_t>(eventOffset + programChangeBytes.size()));
    data.insert(data.begin() + static_cast<std::ptrdiff_t>(eventOffset),
                pitchBendBytes.begin(), pitchBendBytes.end());
}

void waitForTimedPopupToReturnToLoad(mpc::Mpc& mpc)
{
    constexpr auto timeout = std::chrono::seconds(5);
    const auto start = std::chrono::steady_clock::now();

    while (mpc.getLayeredScreen()->getCurrentScreenName() != "load")
    {
        REQUIRE(std::chrono::steady_clock::now() - start < timeout);
        mpc.getEngineHost()->prepareProcessBlock(512);
        mpc.getLayeredScreen()->timerCallback();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
} // namespace

TEST_CASE("Kaitai MPC3000 SEQ production load imports verified event fixture",
          "[kaitai-seq]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    const std::string fileName = "M3KEVT.SEQ";
    prepareSeqFile(mpc, "test/RealMpc3000/Seq/M3KEVT.SEQ", fileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("load");

    const auto loadScreen = mpc.screens->get<mpc::lcdgui::ScreenId::LoadScreen>();
    const auto fileNames = mpc.getDisk()->getFileNames();
    const auto seqFileIt = std::find_if(
        fileNames.begin(), fileNames.end(),
        [&fileName](const std::string &candidate)
        {
            return mpc::StrUtil::eqIgnoreCase(candidate, fileName);
        });
    REQUIRE(seqFileIt != fileNames.end());

    loadScreen->setFileLoad(static_cast<int>(
        std::distance(fileNames.begin(), seqFileIt)));
    loadScreen->function(5);

    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence");
    REQUIRE(layeredScreen->getCurrentScreen()->findLabel("file")->getText() ==
            "File:M3KEVT          .SEQ");
    REQUIRE(layeredScreen->getCurrentScreen()->findField("load-into")->getText() ==
            "01");

    mpc.getSequencer()->getStateManager()->drainQueue();

    auto sequence = mpc.getSequencer()->getSequence(mpc::TempSequenceIndex);
    REQUIRE(sequence->getName() == "SEQ01");
    REQUIRE(sequence->getLastBarIndex() == 0);
    REQUIRE(std::abs(sequence->getInitialTempo() - 99.0) < 0.0001);
    REQUIRE(sequence->getNumerator(0) == 4);
    REQUIRE(sequence->getDenominator(0) == 4);

    auto track0 = sequence->getTrack(0);
    REQUIRE(track0->getName() == "TRK01");
    REQUIRE(track0->getBusType() == mpc::sequencer::BusType::DRUM1);

    const auto events = track0->getEvents();
    REQUIRE(events.size() == 7);

    const auto channelPressure =
        eventAs<mpc::sequencer::ChannelPressureEvent>(events[0]);
    REQUIRE(channelPressure->getTick() == 0);
    REQUIRE(channelPressure->getAmount() == 64);

    const auto polyPressure =
        eventAs<mpc::sequencer::PolyPressureEvent>(events[1]);
    REQUIRE(polyPressure->getTick() == 24);
    REQUIRE(polyPressure->getNote() == 35);
    REQUIRE(polyPressure->getAmount() == 64);

    const auto systemExclusive =
        eventAs<mpc::sequencer::SystemExclusiveEvent>(events[2]);
    REQUIRE(systemExclusive->getTick() == 48);
    REQUIRE(systemExclusive->getByteA() == 0x00);
    REQUIRE(systemExclusive->getByteB() == 0x00);

    const auto mixerLevel = eventAs<mpc::sequencer::MixerEvent>(events[3]);
    REQUIRE(mixerLevel->getTick() == 72);
    REQUIRE(mixerLevel->getParameter() == 0);
    REQUIRE(mixerLevel->getPad() == 0);
    REQUIRE(mixerLevel->getValue() == 100);

    const auto mixerPan = eventAs<mpc::sequencer::MixerEvent>(events[4]);
    REQUIRE(mixerPan->getTick() == 96);
    REQUIRE(mixerPan->getParameter() == 1);
    REQUIRE(mixerPan->getPad() == 0);
    REQUIRE(mixerPan->getValue() == 50);

    const auto mixerFxSend = eventAs<mpc::sequencer::MixerEvent>(events[5]);
    REQUIRE(mixerFxSend->getTick() == 120);
    REQUIRE(mixerFxSend->getParameter() == 2);
    REQUIRE(mixerFxSend->getPad() == 0);
    REQUIRE(mixerFxSend->getValue() == 100);

    const auto controlChange =
        eventAs<mpc::sequencer::ControlChangeEvent>(events[6]);
    REQUIRE(controlChange->getTick() == 144);
    REQUIRE(controlChange->getController() == 0);
    REQUIRE(controlChange->getAmount() == 0);
}

TEST_CASE("Kaitai MPC3000 SEQ production load handles real minimal sentinel fixture",
          "[kaitai-seq]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    const std::string fileName = "M3KMIN.SEQ";
    prepareSeqFile(mpc, "test/RealMpc3000/Seq/M3KMIN.SEQ", fileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("load");

    const auto loadScreen = mpc.screens->get<mpc::lcdgui::ScreenId::LoadScreen>();
    const auto fileNames = mpc.getDisk()->getFileNames();
    const auto seqFileIt = std::find_if(
        fileNames.begin(), fileNames.end(),
        [&fileName](const std::string &candidate)
        {
            return mpc::StrUtil::eqIgnoreCase(candidate, fileName);
        });
    REQUIRE(seqFileIt != fileNames.end());

    loadScreen->setFileLoad(static_cast<int>(
        std::distance(fileNames.begin(), seqFileIt)));
    loadScreen->function(5);

    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence");
    REQUIRE(layeredScreen->getCurrentScreen()->findLabel("file")->getText() ==
            "File:M3KMIN          .SEQ");

    mpc.getSequencer()->getStateManager()->drainQueue();

    auto sequence = mpc.getSequencer()->getSequence(mpc::TempSequenceIndex);
    REQUIRE(sequence->getName() == "SEQ01");
    REQUIRE(sequence->getLastBarIndex() == 0);
    REQUIRE(std::abs(sequence->getInitialTempo() - 99.0) < 0.0001);
    REQUIRE(sequence->getNumerator(0) == 1);
    REQUIRE(sequence->getDenominator(0) == 4);

    auto track0 = sequence->getTrack(0);
    REQUIRE(track0->getName() == "TRK01");
    REQUIRE(track0->getBusType() == mpc::sequencer::BusType::DRUM1);
    REQUIRE(track0->getEvents().empty());
}

TEST_CASE("Kaitai MPC3000 SEQ production load imports real note event fixture",
          "[kaitai-seq]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    const std::string fileName = "M3KNOTE.SEQ";
    prepareSeqFile(mpc, "test/RealMpc3000/Seq/M3KNOTE.SEQ", fileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("load");

    const auto loadScreen = mpc.screens->get<mpc::lcdgui::ScreenId::LoadScreen>();
    const auto fileNames = mpc.getDisk()->getFileNames();
    const auto seqFileIt = std::find_if(
        fileNames.begin(), fileNames.end(),
        [&fileName](const std::string &candidate)
        {
            return mpc::StrUtil::eqIgnoreCase(candidate, fileName);
        });
    REQUIRE(seqFileIt != fileNames.end());

    loadScreen->setFileLoad(static_cast<int>(
        std::distance(fileNames.begin(), seqFileIt)));
    loadScreen->function(5);

    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence");

    mpc.getSequencer()->getStateManager()->drainQueue();

    auto sequence = mpc.getSequencer()->getSequence(mpc::TempSequenceIndex);
    REQUIRE(sequence->getName() == "SEQ01");
    REQUIRE(sequence->getLastBarIndex() == 1);
    REQUIRE(sequence->getNumerator(0) == 4);
    REQUIRE(sequence->getDenominator(0) == 4);
    REQUIRE(sequence->getNumerator(1) == 4);
    REQUIRE(sequence->getDenominator(1) == 4);

    auto track0 = sequence->getTrack(0);
    REQUIRE(track0->getEvents().size() == 1);

    const auto note = eventAs<mpc::sequencer::NoteOnEvent>(track0->getEvents()[0]);
    REQUIRE(note->getTick() == 24);
    REQUIRE(note->getNote() == 35);
    REQUIRE(note->getVelocity() == 64);
    REQUIRE(note->getDuration() == 96);
    REQUIRE(note->getVariationType() == mpc::NoteVariationTypeTune);
    REQUIRE(note->getVariationValue() == 64);
}

TEST_CASE("Kaitai MPC3000 SEQ production load imports real duration-mutated note fixture",
          "[kaitai-seq]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    const std::string fileName = "M3KD120.SEQ";
    prepareSeqFile(mpc, "test/RealMpc3000/Seq/M3KD120.SEQ", fileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("load");

    const auto loadScreen = mpc.screens->get<mpc::lcdgui::ScreenId::LoadScreen>();
    const auto fileNames = mpc.getDisk()->getFileNames();
    const auto seqFileIt = std::find_if(
        fileNames.begin(), fileNames.end(),
        [&fileName](const std::string &candidate)
        {
            return mpc::StrUtil::eqIgnoreCase(candidate, fileName);
        });
    REQUIRE(seqFileIt != fileNames.end());

    loadScreen->setFileLoad(static_cast<int>(
        std::distance(fileNames.begin(), seqFileIt)));
    loadScreen->function(5);

    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence");

    mpc.getSequencer()->getStateManager()->drainQueue();

    auto sequence = mpc.getSequencer()->getSequence(mpc::TempSequenceIndex);
    REQUIRE(sequence->getName() == "SEQ01");
    REQUIRE(sequence->getLastBarIndex() == 1);

    auto track0 = sequence->getTrack(0);
    REQUIRE(track0->getEvents().size() == 1);

    const auto note = eventAs<mpc::sequencer::NoteOnEvent>(track0->getEvents()[0]);
    REQUIRE(note->getTick() == 0);
    REQUIRE(note->getNote() == 35);
    REQUIRE(note->getVelocity() == 64);
    REQUIRE(note->getDuration() == 120);
    REQUIRE(note->getVariationType() == mpc::NoteVariationTypeTune);
    REQUIRE(note->getVariationValue() == 64);
}

TEST_CASE("Kaitai MPC3000 SEQ production load imports loop header state",
          "[kaitai-seq]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/RealMpc3000/Seq/M3KNOTE.SEQ");
    std::vector<char> data(file.begin(), file.end());
    setLoopToBar(data, true, 2);

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    const std::string fileName = "M3KLOOP.SEQ";
    prepareSeqFile(mpc, data, fileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("load");

    const auto loadScreen = mpc.screens->get<mpc::lcdgui::ScreenId::LoadScreen>();
    const auto fileNames = mpc.getDisk()->getFileNames();
    const auto seqFileIt = std::find_if(
        fileNames.begin(), fileNames.end(),
        [&fileName](const std::string &candidate)
        {
            return mpc::StrUtil::eqIgnoreCase(candidate, fileName);
        });
    REQUIRE(seqFileIt != fileNames.end());

    loadScreen->setFileLoad(static_cast<int>(
        std::distance(fileNames.begin(), seqFileIt)));
    loadScreen->function(5);

    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence");

    mpc.getSequencer()->getStateManager()->drainQueue();

    auto sequence = mpc.getSequencer()->getSequence(mpc::TempSequenceIndex);
    REQUIRE(sequence->isLoopEnabled());
    REQUIRE(sequence->getFirstLoopBarIndex() == 0);
    REQUIRE(sequence->getLastLoopBarIndex() == 1);
}

TEST_CASE("Kaitai MPC3000 SEQ production load applies bar time-signature events",
          "[kaitai-seq]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/RealMpc3000/Seq/M3KNOTE.SEQ");
    std::vector<char> data(file.begin(), file.end());
    mutateBarEvent(data, 2, 3, 8);

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    const std::string fileName = "M3KBAR2.SEQ";
    prepareSeqFile(mpc, data, fileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("load");

    const auto loadScreen = mpc.screens->get<mpc::lcdgui::ScreenId::LoadScreen>();
    const auto fileNames = mpc.getDisk()->getFileNames();
    const auto seqFileIt = std::find_if(
        fileNames.begin(), fileNames.end(),
        [&fileName](const std::string &candidate)
        {
            return mpc::StrUtil::eqIgnoreCase(candidate, fileName);
        });
    REQUIRE(seqFileIt != fileNames.end());

    loadScreen->setFileLoad(static_cast<int>(
        std::distance(fileNames.begin(), seqFileIt)));
    loadScreen->function(5);

    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence");

    mpc.getSequencer()->getStateManager()->drainQueue();

    auto sequence = mpc.getSequencer()->getSequence(mpc::TempSequenceIndex);
    REQUIRE(sequence->getLastBarIndex() == 1);
    REQUIRE(sequence->getNumerator(0) == 4);
    REQUIRE(sequence->getDenominator(0) == 4);
    REQUIRE(sequence->getNumerator(1) == 3);
    REQUIRE(sequence->getDenominator(1) == 8);
    REQUIRE(sequence->getTimeSignatureFromBarIndex(0).numerator == 4);
    REQUIRE(sequence->getTimeSignatureFromBarIndex(0).denominator == 4);
    REQUIRE(sequence->getTimeSignatureFromBarIndex(1).numerator == 3);
    REQUIRE(sequence->getTimeSignatureFromBarIndex(1).denominator == 8);

    REQUIRE(sequence->getBarLength(0) == 384);
    REQUIRE(sequence->getBarLength(1) == 144);
    REQUIRE(sequence->getBarLengths()[0] == 384);
    REQUIRE(sequence->getBarLengths()[1] == 144);

    REQUIRE(sequence->getFirstTickOfBar(0) == 0);
    REQUIRE(sequence->getLastTickOfBar(0) == 383);
    REQUIRE(sequence->getFirstTickOfBar(1) == 384);
    REQUIRE(sequence->getLastTickOfBar(1) == 527);
    REQUIRE(sequence->getLastTick() == 528);

    REQUIRE(sequence->getBarIndexForPositionTicks(0) == 0);
    REQUIRE(sequence->getBarIndexForPositionTicks(383) == 0);
    REQUIRE(sequence->getBarIndexForPositionTicks(384) == 1);
    REQUIRE(sequence->getBarIndexForPositionTicks(527) == 1);

    REQUIRE(sequence->getTimeSignatureFromTickPos(0).numerator == 4);
    REQUIRE(sequence->getTimeSignatureFromTickPos(0).denominator == 4);
    REQUIRE(sequence->getTimeSignatureFromTickPos(383).numerator == 4);
    REQUIRE(sequence->getTimeSignatureFromTickPos(383).denominator == 4);
    REQUIRE(sequence->getTimeSignatureFromTickPos(384).numerator == 3);
    REQUIRE(sequence->getTimeSignatureFromTickPos(384).denominator == 8);
    REQUIRE(sequence->getTimeSignatureFromTickPos(527).numerator == 3);
    REQUIRE(sequence->getTimeSignatureFromTickPos(527).denominator == 8);

    auto track0 = sequence->getTrack(0);
    REQUIRE(track0->getEvents().size() == 1);
    const auto note = eventAs<mpc::sequencer::NoteOnEvent>(track0->getEvents()[0]);
    REQUIRE(note->getTick() == 24);
}

TEST_CASE("Kaitai MPC3000 SEQ production load maps MIDI track header fields",
          "[kaitai-seq]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/RealMpc3000/Seq/M3KMIN.SEQ");
    std::vector<char> data(file.begin(), file.end());
    setTrackHeaderFlags(data, 0, true, true, false);
    setTrackPrimaryPortChannelAssignment(data, 0, 17);
    setTrackProgramChangeNumber(data, 0, 42);
    setTrackName(data, 0, "MIDITRK01");

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    const std::string fileName = "M3KTRKH.SEQ";
    prepareSeqFile(mpc, data, fileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("load");

    const auto loadScreen = mpc.screens->get<mpc::lcdgui::ScreenId::LoadScreen>();
    const auto fileNames = mpc.getDisk()->getFileNames();
    const auto seqFileIt = std::find_if(
        fileNames.begin(), fileNames.end(),
        [&fileName](const std::string &candidate)
        {
            return mpc::StrUtil::eqIgnoreCase(candidate, fileName);
        });
    REQUIRE(seqFileIt != fileNames.end());

    loadScreen->setFileLoad(static_cast<int>(
        std::distance(fileNames.begin(), seqFileIt)));
    loadScreen->function(5);

    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence");

    mpc.getSequencer()->getStateManager()->drainQueue();

    auto sequence = mpc.getSequencer()->getSequence(mpc::TempSequenceIndex);
    auto track0 = sequence->getTrack(0);
    REQUIRE(track0->getName() == "MIDITRK01");
    REQUIRE(track0->getBusType() == mpc::sequencer::BusType::MIDI);
    REQUIRE(track0->getDeviceIndex() == 17);
    REQUIRE(track0->getProgramChange() == 42);
    REQUIRE_FALSE(track0->isOn());
}

TEST_CASE("Kaitai MPC3000 SEQ production load maps sparse additional track headers by user track number",
          "[kaitai-seq]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/RealMpc3000/Seq/M3KMIN.SEQ");
    std::vector<char> data(file.begin(), file.end());

    configureTrackHeader(
        data, 1, 1, 8, false, true, false, 5, -1, "MIDITRK08", 100, 17);

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    const std::string fileName = "M3KTRK8.SEQ";
    prepareSeqFile(mpc, data, fileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("load");

    const auto loadScreen = mpc.screens->get<mpc::lcdgui::ScreenId::LoadScreen>();
    const auto fileNames = mpc.getDisk()->getFileNames();
    const auto seqFileIt = std::find_if(
        fileNames.begin(), fileNames.end(),
        [&fileName](const std::string &candidate)
        {
            return mpc::StrUtil::eqIgnoreCase(candidate, fileName);
        });
    REQUIRE(seqFileIt != fileNames.end());

    loadScreen->setFileLoad(static_cast<int>(
        std::distance(fileNames.begin(), seqFileIt)));
    loadScreen->function(5);

    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence");

    mpc.getSequencer()->getStateManager()->drainQueue();

    auto sequence = mpc.getSequencer()->getSequence(mpc::TempSequenceIndex);

    auto track0 = sequence->getTrack(0);
    REQUIRE(track0->getName() == "TRK01");
    REQUIRE(track0->getBusType() == mpc::sequencer::BusType::DRUM1);

    auto track7 = sequence->getTrack(7);
    REQUIRE(track7->getName() == "MIDITRK08");
    REQUIRE(track7->getBusType() == mpc::sequencer::BusType::MIDI);
    REQUIRE(track7->getDeviceIndex() == 5);
    REQUIRE(track7->getProgramChange() == 17);
    REQUIRE(track7->isOn());
    REQUIRE(track7->getEvents().empty());
}

TEST_CASE("Kaitai MPC3000 SEQ production load imports real MIDI track fixture",
          "[kaitai-seq]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    const std::string fileName = "M3KMID1.SEQ";
    prepareSeqFile(mpc, "test/RealMpc3000/Seq/M3KMID1.SEQ", fileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("load");

    const auto loadScreen = mpc.screens->get<mpc::lcdgui::ScreenId::LoadScreen>();
    const auto fileNames = mpc.getDisk()->getFileNames();
    const auto seqFileIt = std::find_if(
        fileNames.begin(), fileNames.end(),
        [&fileName](const std::string &candidate)
        {
            return mpc::StrUtil::eqIgnoreCase(candidate, fileName);
        });
    REQUIRE(seqFileIt != fileNames.end());

    loadScreen->setFileLoad(static_cast<int>(
        std::distance(fileNames.begin(), seqFileIt)));
    loadScreen->function(5);

    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence");

    mpc.getSequencer()->getStateManager()->drainQueue();

    auto sequence = mpc.getSequencer()->getSequence(mpc::TempSequenceIndex);
    auto track0 = sequence->getTrack(0);
    REQUIRE(sequence->getName() == "SEQ01");
    REQUIRE(track0->getName() == "TRK01");
    REQUIRE(track0->getBusType() == mpc::sequencer::BusType::MIDI);
    REQUIRE(track0->isOn());
}

TEST_CASE("Kaitai MPC3000 SEQ production load imports real track-off fixture",
          "[kaitai-seq]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    const std::string fileName = "M3KTRKO.SEQ";
    prepareSeqFile(mpc, "test/RealMpc3000/Seq/M3KTRKO.SEQ", fileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("load");

    const auto loadScreen = mpc.screens->get<mpc::lcdgui::ScreenId::LoadScreen>();
    const auto fileNames = mpc.getDisk()->getFileNames();
    const auto seqFileIt = std::find_if(
        fileNames.begin(), fileNames.end(),
        [&fileName](const std::string &candidate)
        {
            return mpc::StrUtil::eqIgnoreCase(candidate, fileName);
        });
    REQUIRE(seqFileIt != fileNames.end());

    loadScreen->setFileLoad(static_cast<int>(
        std::distance(fileNames.begin(), seqFileIt)));
    loadScreen->function(5);

    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence");

    mpc.getSequencer()->getStateManager()->drainQueue();

    auto sequence = mpc.getSequencer()->getSequence(mpc::TempSequenceIndex);
    auto track0 = sequence->getTrack(0);
    REQUIRE(sequence->getName() == "SEQ01");
    REQUIRE(track0->getName() == "TRK01");
    REQUIRE(track0->getBusType() == mpc::sequencer::BusType::DRUM1);
    REQUIRE_FALSE(track0->isOn());
}

TEST_CASE("Kaitai MPC3000 SEQ production load imports real loop-enabled fixture",
          "[kaitai-seq]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    const std::string fileName = "M3KLPON.SEQ";
    prepareSeqFile(mpc, "test/RealMpc3000/Seq/M3KLPON.SEQ", fileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("load");

    const auto loadScreen = mpc.screens->get<mpc::lcdgui::ScreenId::LoadScreen>();
    const auto fileNames = mpc.getDisk()->getFileNames();
    const auto seqFileIt = std::find_if(
        fileNames.begin(), fileNames.end(),
        [&fileName](const std::string &candidate)
        {
            return mpc::StrUtil::eqIgnoreCase(candidate, fileName);
        });
    REQUIRE(seqFileIt != fileNames.end());

    loadScreen->setFileLoad(static_cast<int>(
        std::distance(fileNames.begin(), seqFileIt)));
    loadScreen->function(5);

    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence");

    mpc.getSequencer()->getStateManager()->drainQueue();

    auto sequence = mpc.getSequencer()->getSequence(mpc::TempSequenceIndex);
    REQUIRE(sequence->getName() == "SEQ01");
    REQUIRE(sequence->getLastBarIndex() == 1);
    REQUIRE(sequence->isLoopEnabled());
    REQUIRE(sequence->getFirstLoopBarIndex() == 0);
    REQUIRE(sequence->getLastLoopBarIndex() == 0);
}

TEST_CASE("Kaitai MPC3000 SEQ production load derives initial tempo from tick-zero tempo change",
          "[kaitai-seq]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/RealMpc3000/Seq/M3KNOTE.SEQ");
    std::vector<char> data(file.begin(), file.end());

    // 150.0% in MPC3000 SEQ fixed-point format.
    setTempoChangeFactorPercentage(data, 0, 0x1800);

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    const std::string fileName = "M3KTMPO.SEQ";
    prepareSeqFile(mpc, data, fileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("load");

    const auto loadScreen = mpc.screens->get<mpc::lcdgui::ScreenId::LoadScreen>();
    const auto fileNames = mpc.getDisk()->getFileNames();
    const auto seqFileIt = std::find_if(
        fileNames.begin(), fileNames.end(),
        [&fileName](const std::string &candidate)
        {
            return mpc::StrUtil::eqIgnoreCase(candidate, fileName);
        });
    REQUIRE(seqFileIt != fileNames.end());

    loadScreen->setFileLoad(static_cast<int>(
        std::distance(fileNames.begin(), seqFileIt)));
    loadScreen->function(5);

    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence");

    mpc.getSequencer()->getStateManager()->drainQueue();

    auto sequence = mpc.getSequencer()->getSequence(mpc::TempSequenceIndex);
    REQUIRE(std::abs(sequence->getInitialTempo() - 180.0) < 0.0001);

    const auto tempoChanges = sequence->getTempoChangeEvents();
    REQUIRE(tempoChanges.size() == 1);
    REQUIRE(tempoChanges[0]->getTick() == 0);
    REQUIRE(tempoChanges[0]->getRatio() == 1000);
    REQUIRE(std::abs(tempoChanges[0]->getTempo() - 180.0) < 0.0001);
}

TEST_CASE("Kaitai MPC3000 SEQ production load imports additional tempo changes as ratio events",
          "[kaitai-seq]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/RealMpc3000/Seq/M3KNOTE.SEQ");
    std::vector<char> data(file.begin(), file.end());

    // Add a second tempo-change entry at tick 96 with 150.0%.
    insertTempoChange(data, 96, 0x1800);

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    const std::string fileName = "M3KTMCH.SEQ";
    prepareSeqFile(mpc, data, fileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("load");

    const auto loadScreen = mpc.screens->get<mpc::lcdgui::ScreenId::LoadScreen>();
    const auto fileNames = mpc.getDisk()->getFileNames();
    const auto seqFileIt = std::find_if(
        fileNames.begin(), fileNames.end(),
        [&fileName](const std::string &candidate)
        {
            return mpc::StrUtil::eqIgnoreCase(candidate, fileName);
        });
    REQUIRE(seqFileIt != fileNames.end());

    loadScreen->setFileLoad(static_cast<int>(
        std::distance(fileNames.begin(), seqFileIt)));
    loadScreen->function(5);

    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence");

    mpc.getSequencer()->getStateManager()->drainQueue();

    auto sequence = mpc.getSequencer()->getSequence(mpc::TempSequenceIndex);
    REQUIRE(std::abs(sequence->getInitialTempo() - 120.0) < 0.0001);

    const auto tempoChanges = sequence->getTempoChangeEvents();
    REQUIRE(tempoChanges.size() == 2);
    REQUIRE(tempoChanges[0]->getTick() == 0);
    REQUIRE(tempoChanges[0]->getRatio() == 1000);
    REQUIRE(tempoChanges[1]->getTick() == 96);
    REQUIRE(tempoChanges[1]->getRatio() == 1500);
    REQUIRE(std::abs(tempoChanges[1]->getTempo() - 180.0) < 0.0001);
}

TEST_CASE("Kaitai MPC3000 SEQ production load imports real non-default header tempo fixture",
          "[kaitai-seq]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    const std::string fileName = "M3KT119.SEQ";
    prepareSeqFile(mpc, "test/RealMpc3000/Seq/M3KT119.SEQ", fileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("load");

    const auto loadScreen = mpc.screens->get<mpc::lcdgui::ScreenId::LoadScreen>();
    const auto fileNames = mpc.getDisk()->getFileNames();
    const auto seqFileIt = std::find_if(
        fileNames.begin(), fileNames.end(),
        [&fileName](const std::string &candidate)
        {
            return mpc::StrUtil::eqIgnoreCase(candidate, fileName);
        });
    REQUIRE(seqFileIt != fileNames.end());

    loadScreen->setFileLoad(static_cast<int>(
        std::distance(fileNames.begin(), seqFileIt)));
    loadScreen->function(5);

    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence");
    REQUIRE(layeredScreen->getCurrentScreen()->findLabel("file")->getText() ==
            "File:M3KT119         .SEQ");

    mpc.getSequencer()->getStateManager()->drainQueue();

    auto sequence = mpc.getSequencer()->getSequence(mpc::TempSequenceIndex);
    REQUIRE(sequence->getName() == "SEQ01");
    REQUIRE(sequence->getLastBarIndex() == 1);
    REQUIRE(std::abs(sequence->getInitialTempo() - 119.0) < 0.0001);

    const auto tempoChanges = sequence->getTempoChangeEvents();
    REQUIRE(tempoChanges.size() == 1);
    REQUIRE(tempoChanges[0]->getTick() == 0);
    REQUIRE(tempoChanges[0]->getRatio() == 1000);
    REQUIRE(std::abs(tempoChanges[0]->getTempo() - 119.0) < 0.0001);
}

TEST_CASE("Kaitai MPC3000 SEQ production load imports second real non-default header tempo fixture",
          "[kaitai-seq]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    const std::string fileName = "M3KT150.SEQ";
    prepareSeqFile(mpc, "test/RealMpc3000/Seq/M3KT150.SEQ", fileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("load");

    const auto loadScreen = mpc.screens->get<mpc::lcdgui::ScreenId::LoadScreen>();
    const auto fileNames = mpc.getDisk()->getFileNames();
    const auto seqFileIt = std::find_if(
        fileNames.begin(), fileNames.end(),
        [&fileName](const std::string &candidate)
        {
            return mpc::StrUtil::eqIgnoreCase(candidate, fileName);
        });
    REQUIRE(seqFileIt != fileNames.end());

    loadScreen->setFileLoad(static_cast<int>(
        std::distance(fileNames.begin(), seqFileIt)));
    loadScreen->function(5);

    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence");
    REQUIRE(layeredScreen->getCurrentScreen()->findLabel("file")->getText() ==
            "File:M3KT150         .SEQ");

    mpc.getSequencer()->getStateManager()->drainQueue();

    auto sequence = mpc.getSequencer()->getSequence(mpc::TempSequenceIndex);
    REQUIRE(sequence->getName() == "SEQ01");
    REQUIRE(sequence->getLastBarIndex() == 1);
    REQUIRE(std::abs(sequence->getInitialTempo() - 150.0) < 0.0001);

    const auto tempoChanges = sequence->getTempoChangeEvents();
    REQUIRE(tempoChanges.size() == 1);
    REQUIRE(tempoChanges[0]->getTick() == 0);
    REQUIRE(tempoChanges[0]->getRatio() == 1000);
    REQUIRE(std::abs(tempoChanges[0]->getTempo() - 150.0) < 0.0001);
}

TEST_CASE("Kaitai MPC3000 SEQ production load imports third real non-default header tempo fixture",
          "[kaitai-seq]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    const std::string fileName = "M3KT090.SEQ";
    prepareSeqFile(mpc, "test/RealMpc3000/Seq/M3KT090.SEQ", fileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("load");

    const auto loadScreen = mpc.screens->get<mpc::lcdgui::ScreenId::LoadScreen>();
    const auto fileNames = mpc.getDisk()->getFileNames();
    const auto seqFileIt = std::find_if(
        fileNames.begin(), fileNames.end(),
        [&fileName](const std::string &candidate)
        {
            return mpc::StrUtil::eqIgnoreCase(candidate, fileName);
        });
    REQUIRE(seqFileIt != fileNames.end());

    loadScreen->setFileLoad(static_cast<int>(
        std::distance(fileNames.begin(), seqFileIt)));
    loadScreen->function(5);

    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence");
    REQUIRE(layeredScreen->getCurrentScreen()->findLabel("file")->getText() ==
            "File:M3KT090         .SEQ");

    mpc.getSequencer()->getStateManager()->drainQueue();

    auto sequence = mpc.getSequencer()->getSequence(mpc::TempSequenceIndex);
    REQUIRE(sequence->getName() == "SEQ01");
    REQUIRE(sequence->getLastBarIndex() == 1);
    REQUIRE(std::abs(sequence->getInitialTempo() - 90.0) < 0.0001);

    const auto tempoChanges = sequence->getTempoChangeEvents();
    REQUIRE(tempoChanges.size() == 1);
    REQUIRE(tempoChanges[0]->getTick() == 0);
    REQUIRE(tempoChanges[0]->getRatio() == 1000);
    REQUIRE(std::abs(tempoChanges[0]->getTempo() - 90.0) < 0.0001);
}

TEST_CASE("Kaitai MPC3000 SEQ production load imports real program change fixture",
          "[kaitai-seq]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    const std::string fileName = "M3KPC.SEQ";
    prepareSeqFile(mpc, "test/RealMpc3000/Seq/M3KPC.SEQ", fileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("load");

    const auto loadScreen = mpc.screens->get<mpc::lcdgui::ScreenId::LoadScreen>();
    const auto fileNames = mpc.getDisk()->getFileNames();
    const auto seqFileIt = std::find_if(
        fileNames.begin(), fileNames.end(),
        [&fileName](const std::string &candidate)
        {
            return mpc::StrUtil::eqIgnoreCase(candidate, fileName);
        });
    REQUIRE(seqFileIt != fileNames.end());

    loadScreen->setFileLoad(static_cast<int>(
        std::distance(fileNames.begin(), seqFileIt)));
    loadScreen->function(5);

    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence");

    mpc.getSequencer()->getStateManager()->drainQueue();

    auto sequence = mpc.getSequencer()->getSequence(mpc::TempSequenceIndex);
    REQUIRE(sequence->getName() == "SEQ02");
    REQUIRE(sequence->getLastBarIndex() == 1);

    auto track0 = sequence->getTrack(0);
    REQUIRE(track0->getEvents().size() == 1);

    const auto programChange =
        eventAs<mpc::sequencer::ProgramChangeEvent>(track0->getEvents()[0]);
    REQUIRE(programChange->getTick() == 0);
    REQUIRE(programChange->getProgram() == 0);
}

TEST_CASE("Kaitai MPC3000 SEQ production load decodes two-byte pitch bend payload",
          "[kaitai-seq]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/RealMpc3000/Seq/M3KPC.SEQ");
    std::vector<char> data(file.begin(), file.end());
    replaceProgramChangeWithPitchBend(data, 0x00, 0x40);

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    const std::string fileName = "M3KPB_SYN.SEQ";
    prepareSeqFile(mpc, data, fileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("load");

    const auto loadScreen = mpc.screens->get<mpc::lcdgui::ScreenId::LoadScreen>();
    const auto fileNames = mpc.getDisk()->getFileNames();
    const auto seqFileIt = std::find_if(
        fileNames.begin(), fileNames.end(),
        [&fileName](const std::string &candidate)
        {
            return mpc::StrUtil::eqIgnoreCase(candidate, fileName);
        });
    REQUIRE(seqFileIt != fileNames.end());

    loadScreen->setFileLoad(static_cast<int>(
        std::distance(fileNames.begin(), seqFileIt)));
    loadScreen->function(5);

    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence");

    mpc.getSequencer()->getStateManager()->drainQueue();

    auto sequence = mpc.getSequencer()->getSequence(mpc::TempSequenceIndex);
    REQUIRE(sequence->getName() == "SEQ02");
    REQUIRE(sequence->getLastBarIndex() == 1);

    auto track0 = sequence->getTrack(0);
    REQUIRE(track0->getEvents().size() == 1);

    const auto pitchBend =
        eventAs<mpc::sequencer::PitchBendEvent>(track0->getEvents()[0]);
    REQUIRE(pitchBend->getTick() == 0);
    REQUIRE(pitchBend->getAmount() == 0);
}

TEST_CASE("Kaitai MPC3000 SEQ production load decodes minimum pitch bend payload",
          "[kaitai-seq]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/RealMpc3000/Seq/M3KPC.SEQ");
    std::vector<char> data(file.begin(), file.end());
    replaceProgramChangeWithPitchBend(data, 0x00, 0x00);

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    const std::string fileName = "M3KPB_MIN.SEQ";
    prepareSeqFile(mpc, data, fileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("load");

    const auto loadScreen = mpc.screens->get<mpc::lcdgui::ScreenId::LoadScreen>();
    const auto fileNames = mpc.getDisk()->getFileNames();
    const auto seqFileIt = std::find_if(
        fileNames.begin(), fileNames.end(),
        [&fileName](const std::string &candidate)
        {
            return mpc::StrUtil::eqIgnoreCase(candidate, fileName);
        });
    REQUIRE(seqFileIt != fileNames.end());

    loadScreen->setFileLoad(static_cast<int>(
        std::distance(fileNames.begin(), seqFileIt)));
    loadScreen->function(5);

    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence");

    mpc.getSequencer()->getStateManager()->drainQueue();

    auto sequence = mpc.getSequencer()->getSequence(mpc::TempSequenceIndex);
    auto track0 = sequence->getTrack(0);
    REQUIRE(track0->getEvents().size() == 1);

    const auto pitchBend =
        eventAs<mpc::sequencer::PitchBendEvent>(track0->getEvents()[0]);
    REQUIRE(pitchBend->getAmount() == -8192);
}

TEST_CASE("Kaitai MPC3000 SEQ production load decodes maximum pitch bend payload",
          "[kaitai-seq]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/RealMpc3000/Seq/M3KPC.SEQ");
    std::vector<char> data(file.begin(), file.end());
    replaceProgramChangeWithPitchBend(data, 0x7f, 0x7f);

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    const std::string fileName = "M3KPB_MAX.SEQ";
    prepareSeqFile(mpc, data, fileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("load");

    const auto loadScreen = mpc.screens->get<mpc::lcdgui::ScreenId::LoadScreen>();
    const auto fileNames = mpc.getDisk()->getFileNames();
    const auto seqFileIt = std::find_if(
        fileNames.begin(), fileNames.end(),
        [&fileName](const std::string &candidate)
        {
            return mpc::StrUtil::eqIgnoreCase(candidate, fileName);
        });
    REQUIRE(seqFileIt != fileNames.end());

    loadScreen->setFileLoad(static_cast<int>(
        std::distance(fileNames.begin(), seqFileIt)));
    loadScreen->function(5);

    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence");

    mpc.getSequencer()->getStateManager()->drainQueue();

    auto sequence = mpc.getSequencer()->getSequence(mpc::TempSequenceIndex);
    auto track0 = sequence->getTrack(0);
    REQUIRE(track0->getEvents().size() == 1);

    const auto pitchBend =
        eventAs<mpc::sequencer::PitchBendEvent>(track0->getEvents()[0]);
    REQUIRE(pitchBend->getAmount() == 8191);
}

TEST_CASE("Kaitai MPC3000 SEQ production load imports real pitch bend fixture",
          "[kaitai-seq]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    const std::string fileName = "M3KPB.SEQ";
    prepareSeqFile(mpc, "test/RealMpc3000/Seq/M3KPB.SEQ", fileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("load");

    const auto loadScreen = mpc.screens->get<mpc::lcdgui::ScreenId::LoadScreen>();
    const auto fileNames = mpc.getDisk()->getFileNames();
    const auto seqFileIt = std::find_if(
        fileNames.begin(), fileNames.end(),
        [&fileName](const std::string &candidate)
        {
            return mpc::StrUtil::eqIgnoreCase(candidate, fileName);
        });
    REQUIRE(seqFileIt != fileNames.end());

    loadScreen->setFileLoad(static_cast<int>(
        std::distance(fileNames.begin(), seqFileIt)));
    loadScreen->function(5);

    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence");

    mpc.getSequencer()->getStateManager()->drainQueue();

    auto sequence = mpc.getSequencer()->getSequence(mpc::TempSequenceIndex);
    REQUIRE(sequence->getName() == "SEQ04");
    REQUIRE(sequence->getLastBarIndex() == 1);

    auto track0 = sequence->getTrack(0);
    REQUIRE(track0->getEvents().size() == 1);

    const auto pitchBend =
        eventAs<mpc::sequencer::PitchBendEvent>(track0->getEvents()[0]);
    REQUIRE(pitchBend->getTick() == 0);
    REQUIRE(pitchBend->getAmount() == 0);
}

TEST_CASE("Load a sequence PLAY shows held preview overlay and release restores prior selection",
          "[kaitai-seq][ui]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    sequencer->getTransport()->setCountEnabled(false);

    sequencer->getSequence(5)->init(0);
    stateManager->drainQueue();
    sequencer->setSelectedSequenceIndex(mpc::SequenceIndex(5), true);
    stateManager->drainQueue();

    const std::string fileName = "M3KNOTE.SEQ";
    prepareSeqFile(mpc, "test/RealMpc3000/Seq/M3KNOTE.SEQ", fileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("load");

    const auto loadScreen = mpc.screens->get<mpc::lcdgui::ScreenId::LoadScreen>();
    const auto fileNames = mpc.getDisk()->getFileNames();
    const auto seqFileIt = std::find_if(
        fileNames.begin(), fileNames.end(),
        [&fileName](const std::string &candidate)
        {
            return mpc::StrUtil::eqIgnoreCase(candidate, fileName);
        });
    REQUIRE(seqFileIt != fileNames.end());

    loadScreen->setFileLoad(static_cast<int>(
        std::distance(fileNames.begin(), seqFileIt)));
    loadScreen->function(5);
    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence");

    auto loadSequenceScreen =
        mpc.screens->get<mpc::lcdgui::ScreenId::LoadASequenceScreen>();

    layeredScreen->getCurrentScreen()->function(2);
    stateManager->drainQueue();

    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence-play");
    REQUIRE(sequencer->getSelectedSequenceIndex() == mpc::TempSequenceIndex);
    REQUIRE(sequencer->getTransport()->isPlaying());

    mpc.getEngineHost()->prepareProcessBlock(512);
    layeredScreen->timerCallback();
    REQUIRE_FALSE(
        layeredScreen->getCurrentScreen()->findLabel("position")->getText().empty());

    mpc::command::ReleaseFunctionCommand(mpc, 2).execute();
    stateManager->drainQueue();

    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence");
    REQUIRE_FALSE(sequencer->getTransport()->isPlaying());
    REQUIRE(sequencer->getSelectedSequenceIndex() == mpc::SequenceIndex(5));

    loadSequenceScreen->function(3);
    stateManager->drainQueue();

    REQUIRE(layeredScreen->getCurrentScreenName() == "load");
}

TEST_CASE("Load a sequence KEEP copies temp sequence into chosen slot and stops preview",
          "[kaitai-seq][ui]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);

    auto sequencer = mpc.getSequencer();
    auto stateManager = sequencer->getStateManager();
    sequencer->getTransport()->setCountEnabled(false);

    sequencer->getSequence(5)->init(0);
    stateManager->drainQueue();
    sequencer->setSelectedSequenceIndex(mpc::SequenceIndex(5), true);
    stateManager->drainQueue();

    const std::string fileName = "M3KNOTE.SEQ";
    prepareSeqFile(mpc, "test/RealMpc3000/Seq/M3KNOTE.SEQ", fileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("load");

    const auto loadScreen = mpc.screens->get<mpc::lcdgui::ScreenId::LoadScreen>();
    const auto fileNames = mpc.getDisk()->getFileNames();
    const auto seqFileIt = std::find_if(
        fileNames.begin(), fileNames.end(),
        [&fileName](const std::string &candidate)
        {
            return mpc::StrUtil::eqIgnoreCase(candidate, fileName);
        });
    REQUIRE(seqFileIt != fileNames.end());

    loadScreen->setFileLoad(static_cast<int>(
        std::distance(fileNames.begin(), seqFileIt)));
    loadScreen->function(5);
    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence");

    auto loadSequenceScreen =
        mpc.screens->get<mpc::lcdgui::ScreenId::LoadASequenceScreen>();
    loadSequenceScreen->setLoadInto(mpc::SequenceIndex(7));

    layeredScreen->getCurrentScreen()->function(2);
    stateManager->drainQueue();
    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence-play");
    REQUIRE(sequencer->getSelectedSequenceIndex() == mpc::TempSequenceIndex);
    REQUIRE(sequencer->getTransport()->isPlaying());

    mpc::command::ReleaseFunctionCommand(mpc, 2).execute();
    stateManager->drainQueue();
    REQUIRE(layeredScreen->getCurrentScreenName() == "load-a-sequence");
    REQUIRE_FALSE(sequencer->getTransport()->isPlaying());
    REQUIRE(sequencer->getSelectedSequenceIndex() == mpc::SequenceIndex(5));

    loadSequenceScreen->function(4);
    stateManager->drainQueue();

    REQUIRE(layeredScreen->getCurrentScreenName() == "sequencer");
    REQUIRE_FALSE(sequencer->getTransport()->isPlaying());
    REQUIRE(sequencer->getSelectedSequenceIndex() == mpc::SequenceIndex(7));

    auto keptSequence = sequencer->getSequence(7);
    REQUIRE(keptSequence->getTrack(0)->getEvents().size() == 1);
    const auto keptNote =
        eventAs<mpc::sequencer::NoteOnEvent>(
            keptSequence->getTrack(0)->getEvents()[0]);
    REQUIRE(keptNote->getTick() == 24);
    REQUIRE(keptNote->getNote() == 35);
    REQUIRE(keptNote->getVelocity() == 64);
    REQUIRE(keptNote->getDuration() == 96);
}

TEST_CASE("Unreadable .SEQ file reports error and returns to LOAD",
          "[kaitai-seq][ui]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    const std::string fileName = "BROKEN.SEQ";
    prepareSeqFile(mpc, std::vector<char>{'B', 'A', 'D'}, fileName);

    auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreen("load");

    const auto loadScreen = mpc.screens->get<mpc::lcdgui::ScreenId::LoadScreen>();
    const auto fileNames = mpc.getDisk()->getFileNames();
    const auto seqFileIt = std::find_if(
        fileNames.begin(), fileNames.end(),
        [&fileName](const std::string &candidate)
        {
            return mpc::StrUtil::eqIgnoreCase(candidate, fileName);
        });
    REQUIRE(seqFileIt != fileNames.end());

    loadScreen->setFileLoad(static_cast<int>(
        std::distance(fileNames.begin(), seqFileIt)));
    loadScreen->function(5);

    layeredScreen->timerCallback();
    REQUIRE(layeredScreen->getCurrentScreenName() == "popup");
    waitForTimedPopupToReturnToLoad(mpc);
    REQUIRE(layeredScreen->getCurrentScreenName() == "load");
}

TEST_CASE("Kaitai MPC60 v2 SEQ loads a real MPC60 2.14 sequence through the production seam", "[kaitai-seq][real-mpc60]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);

    auto seqFile = prepareSeqFile(
        mpc,
        "test/RealMpc60/Seq/MPC60_V214_SEQ01.SEQ",
        "M60SEQ.SEQ"
    );
    REQUIRE(seqFile);

    const auto loaded = mpc::file::kaitai::Mpc3000SeqIo::load(mpc, seqFile);
    REQUIRE(loaded);
    mpc.getSequencer()->getStateManager()->drainQueue();

    const auto sequence = *loaded;
    REQUIRE(sequence->isUsed());
    REQUIRE(sequence->getName() == "SEQ01");
    REQUIRE(sequence->getLastBarIndex() == 1);
    REQUIRE(std::fabs(sequence->getInitialTempo() - 120.0) < 0.001);
    REQUIRE(sequence->getTimeSignatureFromBarIndex(0).numerator == 4);
    REQUIRE(sequence->getTimeSignatureFromBarIndex(0).denominator == 4);
    REQUIRE(sequence->isLoopEnabled());

    REQUIRE(sequence->getTrack(0)->getEvents().size() == 1U);
    const auto note = eventAs<mpc::sequencer::NoteOnEvent>(
        sequence->getTrack(0)->getEvents().at(0));
    REQUIRE(note->getTick() == 0);
    REQUIRE(note->getNote() == 1);
    REQUIRE(note->getVelocity() == 64);
    REQUIRE(note->getDuration() == 96);
}
