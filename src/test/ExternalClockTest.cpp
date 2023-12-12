#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "sequencer/FrameSeq.hpp"
#include "sequencer/ExternalClock.hpp"

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

void parseDoubles(const char* input, std::vector<double>& output) {
    std::stringstream ss(input);
    std::string line;

    while (std::getline(ss, line)) {
        try {
            double value = std::stod(line);
            output.push_back(value);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid argument: " << e.what() << '\n';
        } catch (const std::out_of_range& e) {
            std::cerr << "Out of range: " << e.what() << '\n';
        }
    }
}

void readDoublesFromFile(std::vector<double>& ppqPositions, std::string fileName)
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/ExternalClock/" + fileName);
    char *data = (char *) std::string_view(file.begin(), file.end() - file.begin()).data();
    parseDoubles(data, ppqPositions);
}

std::string doubleToStringWithPrecision(double value, int n) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(n) << value;
    return out.str();
}

void printDouble(double d)
{
    printf("%s\n", doubleToStringWithPrecision(d, 15).c_str());
}

TEST_CASE("16 bars, 120bpm constant, 44.1khz, 64 frames", "[external-clock]")
{
    std::vector<double> ppqPositions;
    readDoublesFromFile(ppqPositions, "ableton-live-44.1khz-64frames-120bpm.txt");

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    std::vector<double> ticks;

    for (double ppqPosition : ppqPositions)
    {
        mpc.getExternalClock()->computeTicksForCurrentBuffer(ppqPosition, 64, 44100, 120);

        auto& ticksForCurrentBuffer = mpc.getExternalClock()->getTicksForCurrentBuffer();

        for (auto& tick : ticksForCurrentBuffer)
        {
            if (tick == -1) continue;
            ticks.push_back(tick);
        }

        if (std::count(ticksForCurrentBuffer.begin(),
                       ticksForCurrentBuffer.end(), -1) == ticksForCurrentBuffer.size())
        {
            // No ticks generated for current buffer
            ticks.push_back(-1);
        }

        mpc.getExternalClock()->clearTicks();
    }

    for (int tick = 1; tick < ticks.size(); tick++)
    {
        if (ticks[tick] == -1) continue;

        int i = 1;

        double ta = ticks[tick - i];

        int emptyBuffersBetween = 1;

        while (ta == -1)
        {
            i++;
            ta = ticks[tick - i];
            emptyBuffersBetween++;
        }

        const double tb = ticks[tick] + (emptyBuffersBetween * 64);
        const double diff = tb - ta;
        const double deviation = diff - frames_per_tick;
        REQUIRE(std::abs(deviation) <= 0.6875);
    }

    const int barCount = 16;
    const int expectedTickCount = barCount * 4 * 96;
    const size_t emptyBuffers = std::count(ticks.begin(), ticks.end(), -1);
    REQUIRE(ticks.size() - emptyBuffers == expectedTickCount);
}

TEST_CASE("16 bars, linear descent from 300bpm to 30bpm, 44.1khz, 2048 frames", "[external-clock]")
{
    std::vector<double> ppqPositions;
    readDoublesFromFile(ppqPositions, "ableton-live-44.1khz-2048frames-300bpm-to-30bpm-linear-16-bars.txt");
    std::vector<double> tempos;
    readDoublesFromFile(tempos, "ableton-live-44.1khz-2048frames-300bpm-to-30bpm-linear-16-bars-tempo-map.txt");

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    std::vector<double> ticks;

    for (int i = 0; i < ppqPositions.size(); i++)
    {
        const auto ppqPosition = ppqPositions[i];
        const auto tempo = tempos[i];
        mpc.getExternalClock()->computeTicksForCurrentBuffer(ppqPosition, 2048, 44100, tempo);

        auto& ticksForCurrentBuffer = mpc.getExternalClock()->getTicksForCurrentBuffer();

        for (auto& tick : ticksForCurrentBuffer)
        {
            if (tick == -1) continue;
            ticks.push_back(tick);
        }

        mpc.getExternalClock()->clearTicks();
    }

    const int barCount = 16;
    const int expectedTickCount = barCount * 4 * 96;
    REQUIRE(ticks.size() == expectedTickCount);
}
