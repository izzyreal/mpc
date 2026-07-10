#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"
#include "engine/EngineHost.hpp"
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
    REQUIRE(data.size() > 338);
    const auto numberOfTrackHeaders =
        static_cast<unsigned char>(data[338]);
    return 339 + (numberOfTrackHeaders * 24) + (index * 6);
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
    REQUIRE(data.size() > 337);
    const auto numberOfTempoChanges = static_cast<unsigned char>(data[337]);
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
    data[337] = static_cast<char>(numberOfTempoChanges + 1);
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
    REQUIRE(data.size() > 338);
    const auto numberOfTempoChanges = static_cast<unsigned char>(data[337]);
    const auto numberOfTrackHeaders = static_cast<unsigned char>(data[338]);
    REQUIRE(index < numberOfTrackHeaders);
    return 339 + (numberOfTempoChanges * 6) + (index * 24);
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
