#pragma once

#include <map>
#include <vector>
#include <memory>

namespace mpc::engine {
    class StereoMixer;
    class IndivFxMixer;
}

namespace mpc::engine
{
    class Drum final
    {
        public:
            Drum(const int drumIndex);

            int getIndex() const { return drumIndex; }
            int getProgram() const;
            void setProgram(int programIndex);
            bool receivesPgmChange() const;
            void setReceivePgmChange(bool b);
            bool receivesMidiVolume() const;
            void setReceiveMidiVolume(bool b);
            void setLastReceivedMidiVolume(int volume);
            int getLastReceivedMidiVolume() const;

            std::map<int, int>& getSimultA() { return simultA; }
            std::map<int, int>& getSimultB() { return simultB; }

            std::vector<std::shared_ptr<StereoMixer>>& getStereoMixerChannels();
            std::vector<std::shared_ptr<IndivFxMixer>>& getIndivFxMixerChannels();

        private:
            const int drumIndex;

            int programIndex = 0;

            std::vector<std::shared_ptr<StereoMixer>> stereoMixerChannels;
            std::vector<std::shared_ptr<IndivFxMixer>> indivFxMixerChannels;

            std::map<int, int> simultA;
            std::map<int, int> simultB;

            bool receivePgmChange = false;
            bool receiveMidiVolume = false;
            int lastReceivedMidiVolume = 127;
    };
}

