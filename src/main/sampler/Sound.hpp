#pragma once

#include <memory>
#include <string>
#include <vector>
#include <cstdint>

namespace mpc::sampler
{
    class Sound
    {
        std::string name;
        int numberOfBeats{4};
        bool mono = false, loopEnabled = false;
        int sampleRate = 44100;
        int sndLevel = 100, tune = 0;
        int start = 0, end = 0, loopTo = 0;
        std::shared_ptr<std::vector<float>> sampleData;

        // Should only be called on newly created sounds that are unassigned
        void insertFrames(std::vector<float> &left, std::vector<float> &right,
                          unsigned int index, uint32_t nFrames) const;

    public:
        void setName(const std::string &s);
        std::shared_ptr<const std::vector<float>> getSampleData() const;

        // Should only be called on newly created sounds that are unassigned
        std::shared_ptr<std::vector<float>> getMutableSampleData() const;

        // Swaps the existing sampleData with another one, without destroying
        // the existing one
        void setSampleData(const std::shared_ptr<std::vector<float>> &);
        int getTune() const;
        int getStart() const;
        int getEnd() const;
        bool isLoopEnabled() const;
        int getLoopTo() const;
        bool isMono() const;
        int getLastFrameIndex() const;
        int getSndLevel() const;

        void setBeatCount(int i);
        int getBeatCount() const;
        std::string getName();
        int getSampleRate() const;
        void setSampleRate(int sr);
        int getFrameCount() const;
        void setMono(bool mono);
        void setEnd(int end);
        void setLevel(int level);
        void setStart(int start);
        void setLoopEnabled(bool loopEnabled);
        void setLoopTo(int loopTo);
        void setTune(int tune);

        // Should only be called on newly created sounds that are unassigned
        void insertFrame(const std::vector<float> &frame,
                         unsigned int index) const;
        // Should only be called on newly created sounds that are unassigned
        void insertFrames(std::vector<float> &frames, unsigned int index,
                          uint32_t nFrames) const;
        // Should only be called on newly created sounds that are unassigned
        void appendFrames(std::vector<float> &frames, uint32_t nFrames) const;
        // Should only be called on newly created sounds that are unassigned
        void appendFrames(std::vector<float> &left, std::vector<float> &right,
                          uint32_t nFrames) const;
        // Should only be called on newly created sounds that are unassigned
        void removeFramesFromEnd(int numFramesToRemove) const;

        Sound(int rate);
    };
} // namespace mpc::sampler
