#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "sequencer/FrameSeq.hpp"
#include "sequencer/Clock.hpp"

#include "audiomidi/AudioMidiServices.hpp"

#include "lcdgui/screens/SyncScreen.hpp"

#include <cmrc/cmrc.hpp>
#include <string_view>

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

CMRC_DECLARE(mpctest);

double bpm = 120.0;
double sample_rate = 44100.0;
double frames_per_beat = sample_rate * 60 / bpm;
double frames_per_tick = frames_per_beat / 96.0;

void parseDoubles(const char *input, std::vector<double> &output)
{
    std::stringstream ss(input);
    std::string line;

    while (std::getline(ss, line))
    {
        try
        {
            double value = std::stod(line);
            output.push_back(value);
        }
        catch (const std::invalid_argument &e)
        {
            std::cerr << "Invalid argument: " << e.what() << '\n';
        }
        catch (const std::out_of_range &e)
        {
            std::cerr << "Out of range: " << e.what() << '\n';
        }
    }
}

void readDoublesFromFile(std::vector<double> &positionsInQuarterNotes, std::string fileName)
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/Clock/" + fileName);
    char *data = (char *)std::string_view(file.begin(), file.end() - file.begin()).data();
    parseDoubles(data, positionsInQuarterNotes);
}

std::string doubleToStringWithPrecision(double value, int n)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(n) << value;
    return out.str();
}

void printDouble(double d)
{
    printf("%s\n", doubleToStringWithPrecision(d, 15).c_str());
}

TEST_CASE("16 bars, 120bpm constant, 44.1khz, 64 frames", "[clock]")
{
    const uint16_t bufferSize = 64;
    std::vector<double> positions;
    readDoublesFromFile(positions, "ableton-live-44.1khz-64frames-120bpm.txt");

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    std::vector<int32_t> ticks;
    int64_t timeInSamples = 0;

    for (double position : positions)
    {
        mpc.getClock()->processBufferExternal(position, bufferSize, 44100, 120, timeInSamples);

        auto &ticksForCurrentBuffer = mpc.getClock()->getTicksForCurrentBuffer();

        for (size_t tickIndex = 0; tickIndex < ticksForCurrentBuffer.size(); tickIndex++)
        {
            ticks.push_back(ticksForCurrentBuffer[tickIndex]);
        }

        if (ticksForCurrentBuffer.size() == 0)
        {
            // No ticks generated for current buffer
            ticks.push_back(-1);
        }

        timeInSamples += bufferSize;
    }

    for (int tick = 1; tick < ticks.size(); tick++)
    {
        if (ticks[tick] == -1)
        {
            continue;
        }

        int i = 1;

        int32_t ta = ticks[tick - i];

        int emptyBuffersBetween = 1;

        while (ta == -1)
        {
            i++;
            ta = ticks[tick - i];
            emptyBuffersBetween++;
        }

        const int32_t tb = ticks[tick] + (emptyBuffersBetween * 64);
        const int32_t diff = tb - ta;
        const double deviation = diff - frames_per_tick;
        REQUIRE(std::abs(deviation) <= 0.6875);
    }

    const int barCount = 16;
    const int expectedTickCount = barCount * 4 * 96;
    const size_t emptyBuffers = std::count(ticks.begin(), ticks.end(), -1);
    REQUIRE(ticks.size() - emptyBuffers == expectedTickCount);
}

TEST_CASE("16 bars, linear descent from 300bpm to 30bpm, 44.1khz, 2048 frames", "[clock]")
{
    const uint16_t bufferSize = 2048;
    std::vector<double> positions;
    readDoublesFromFile(positions, "ableton-live-44.1khz-2048frames-300bpm-to-30bpm-linear-16-bars.txt");
    std::vector<double> tempos;
    readDoublesFromFile(tempos, "ableton-live-44.1khz-2048frames-300bpm-to-30bpm-linear-16-bars-tempo-map.txt");

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    std::vector<int32_t> ticks;
    int64_t timeInSamples = 0;

    for (int i = 0; i < positions.size(); i++)
    {
        const auto position = positions[i];
        const auto tempo = tempos[i];
        mpc.getClock()->processBufferExternal(position, bufferSize, 44100, tempo, timeInSamples);

        auto &ticksForCurrentBuffer = mpc.getClock()->getTicksForCurrentBuffer();

        for (size_t tickIndex = 0; tickIndex < ticksForCurrentBuffer.size(); tickIndex++)
        {
            ticks.push_back(ticksForCurrentBuffer[tickIndex]);
        }

        timeInSamples += bufferSize;
    }

    const int barCount = 16;
    const int expectedTickCount = (barCount * 4 * 96) + 2;
    REQUIRE(ticks.size() == expectedTickCount);
}

TEST_CASE("16 bars, linear ascent from 30bpm to 300bpm, 44.1khz, 2048 frames", "[clock]")
{
    const uint16_t bufferSize = 2048;
    std::vector<double> positions;
    readDoublesFromFile(positions, "ableton-live-44.1khz-2048frames-30bpm-to-300bpm-linear-16-bars.txt");
    std::vector<double> tempos;
    readDoublesFromFile(tempos, "ableton-live-44.1khz-2048frames-30bpm-to-300bpm-linear-16-bars-tempo-map.txt");

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    std::vector<int32_t> ticks;
    int64_t timeInSamples = 0;

    for (int i = 0; i < positions.size(); i++)
    {
        const auto position = positions[i];
        const auto tempo = tempos[i];
        mpc.getClock()->processBufferExternal(position, bufferSize, 44100, tempo, timeInSamples);

        auto &ticksForCurrentBuffer = mpc.getClock()->getTicksForCurrentBuffer();

        for (size_t tickIndex = 0; tickIndex < ticksForCurrentBuffer.size(); tickIndex++)
        {
            ticks.push_back(ticksForCurrentBuffer[tickIndex]);
        }

        timeInSamples += bufferSize;
    }

    const int expectedTickCount = 6164;
    REQUIRE(ticks.size() == expectedTickCount);
}

TEST_CASE("2 bars 30bpm, 2bars 300bpm, 44.1khz, 2048 frames", "[clock]")
{
    const uint16_t bufferSize = 2048;
    std::vector<double> positions;
    readDoublesFromFile(positions, "ableton-live-44.1khz-2048frames-2bars30bpm-2bars300bpm.txt");
    std::vector<double> tempos;
    readDoublesFromFile(tempos, "ableton-live-44.1khz-2048frames-2bars30bpm-2bars300bpm-tempo-map.txt");

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    std::vector<int32_t> ticks;
    int64_t timeInSamples = 0;

    for (int i = 0; i < positions.size(); i++)
    {
        const auto position = positions[i];
        const auto tempo = tempos[i];
        mpc.getClock()->processBufferExternal(position, bufferSize, 44100, tempo, timeInSamples);

        auto &ticksForCurrentBuffer = mpc.getClock()->getTicksForCurrentBuffer();

        for (size_t tickIndex = 0; tickIndex < ticksForCurrentBuffer.size(); tickIndex++)
        {
            ticks.push_back(ticksForCurrentBuffer[tickIndex]);
        }

        timeInSamples += bufferSize;
    }

    const int expectedTickCount = 1537;
    REQUIRE(ticks.size() == expectedTickCount);
}

TEST_CASE("2 bars 300bpm, 2bars 30bpm, 44.1khz, 2048 frames", "[clock]")
{
    const uint16_t bufferSize = 2048;
    std::vector<double> positions;
    readDoublesFromFile(positions, "ableton-live-44.1khz-2048frames-2bars300bpm-2bars30bpm.txt");
    std::vector<double> tempos;
    readDoublesFromFile(tempos, "ableton-live-44.1khz-2048frames-2bars300bpm-2bars30bpm-tempo-map.txt");

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    std::vector<int32_t> ticks;
    int64_t timeInSamples = 0;

    for (int i = 0; i < positions.size(); i++)
    {
        const auto position = positions[i];
        const auto tempo = tempos[i];
        mpc.getClock()->processBufferExternal(position, bufferSize, 44100, tempo, timeInSamples);

        auto &ticksForCurrentBuffer = mpc.getClock()->getTicksForCurrentBuffer();

        for (size_t tickIndex = 0; tickIndex < ticksForCurrentBuffer.size(); tickIndex++)
        {
            ticks.push_back(ticksForCurrentBuffer[tickIndex]);
        }

        timeInSamples += bufferSize;
    }

    const int expectedTickCount = 1552;
    REQUIRE(ticks.size() == expectedTickCount);
}

TEST_CASE("1 bar loop", "[clock]")
{
    std::vector<double> positions;
    readDoublesFromFile(positions, "reaper-44.1khz-120bpm-1-bar-loop-ppqpos.txt");

    std::vector<double> blockSizes;
    readDoublesFromFile(blockSizes, "reaper-44.1khz-120bpm-1-bar-loop-block.txt");

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    assert(mpc.getSequencer()->getTickPosition() == 0);

    mpc.getSequencer()->getSequence(0)->init(1);
    mpc.getSequencer()->playFromStart();

    auto syncScreen = mpc.screens->get<mpc::lcdgui::screens::SyncScreen>();
    syncScreen->modeIn = 1;

    std::vector<int32_t> ticks;
    int64_t timeInSamples = 0;

    uint64_t highestTickPos = 0;

    uint64_t cumulativeTickPos = 0;

    for (int i = 0; i < positions.size(); i++)
    {
        const auto position = positions[i];
        mpc.getClock()->processBufferExternal(position, blockSizes[i], 44100, 120, timeInSamples);

        auto frameSeq = mpc.getAudioMidiServices()->getFrameSequencer();
        frameSeq->work(blockSizes[i]);
        auto &ticksForCurrentBuffer = mpc.getClock()->getTicksForCurrentBuffer();

        for (size_t tickIndex = 0; tickIndex < ticksForCurrentBuffer.size(); tickIndex++)
        {
            ticks.push_back(ticksForCurrentBuffer[tickIndex]);
            cumulativeTickPos += 1;
        }

        highestTickPos = std::max<uint64_t>(highestTickPos, mpc.getSequencer()->getTickPosition());
        timeInSamples += blockSizes[i];
    }

    printf("highestTickPos: %i, cumulativeTickPos: %i\n", highestTickPos, cumulativeTickPos);
}
