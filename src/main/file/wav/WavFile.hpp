#pragma once

#include "mpc_types.hpp"

#include <vector>
#include <memory>

#include <fstream>

namespace mpc::file::wav
{

    struct SampleLoop
    {
        int cuePointId;
        int type;
        int start;
        int end;
        int fraction;
        int playCount;
    };

    class WavFile
    {
    public:
        static WavFile writeWavStream(std::shared_ptr<std::ostream>, int numChannels, int numFrames, int validBits, int sampleRate);
        static wav_or_error readWavStream(std::shared_ptr<std::istream>);

    private:
        const static int SMPL_CHUNK_ID = 0x6C706D73;         // 1819307379
        const static int SMPL_CHUNK_DATA_SIZE = 0x04;        // 4 Chunk Data Size 36 + (Num Sample Loops * 24) + Sampler Data
        const static int MANUFACTURER = 0x08;                // 4 Manufacturer 0 - 0xFFFFFFFF
        const static int PRODUCT = 0x0C;                     // 4 Product 0 - 0xFFFFFFFF
        const static int SAMPLE_PERIOD = 0x10;               // 4 Sample Period 0 - 0xFFFFFFFF
        const static int MIDI_UNITY_NOTE = 0x14;             // 4 MIDI Unity Note 0 - 127
        const static int MIDI_PITCH_FRACTION = 0x18;         // 4 MIDI Pitch Fraction 0 - 0xFFFFFFFF
        const static int SMPTE_FORMAT = 0x1C;                // 4 SMPTE Format 0, 24, 25, 29, 30
        const static int SMPTE_OFFSET = 0x20;                // 4 SMPTE Offset 0 - 0xFFFFFFFF
        const static int NUM_SAMPLE_LOOPS = 0x1C;            // 4 Num Sample Loops 0 - 0xFFFFFFFF
        const static int LIST_OF_SAMPLE_LOOPS_OFFSET = 0x24; // List of Sample Loops
        std::vector<char> buffer = std::vector<char>(BUFFER_SIZE);
        std::shared_ptr<std::istream> iStream;
        std::shared_ptr<std::ostream> oStream;

    private:
        static const int BUFFER_SIZE{2048};
        static const int FMT_CHUNK_ID{544501094};
        static const int DATA_CHUNK_ID{1635017060};
        static const int RIFF_CHUNK_ID{1179011410};
        static const int RIFF_TYPE_ID{1163280727};
        int bytesPerSample;
        unsigned long numFrames;
        double floatScale;
        double floatOffset;
        bool wordAlignAdjust;
        int numChannels;
        int sampleRate;
        int blockAlign;
        int validBits;
        unsigned long bufferPointer;
        unsigned long bytesRead;
        unsigned long frameCounter;
        int numSampleLoops;
        SampleLoop sampleLoop;

    public:
        int getNumChannels();
        unsigned long getNumFrames();

        int getSampleRate();
        int getValidBits();
        int getNumSampleLoops();
        SampleLoop &getSampleLoop();

    private:
        static unsigned long getLE(std::vector<char> &buffer, unsigned long pos, int numBytes);
        static void putLE(int val, std::vector<char> &buffer, unsigned long pos, int numBytes);
        void writeSample(int val);
        int readSample();

    public:
        int readFrames(std::vector<float> &sampleBuffer, unsigned long numFramesToRead);
        int writeFrames(const std::vector<float> &sampleBuffer, unsigned long numFramesToWrite);
        void close();
    };
} // namespace mpc::file::wav
