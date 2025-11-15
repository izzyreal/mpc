#pragma once

#include "BusType.hpp"
#include "IntTypes.hpp"

#include <map>
#include <memory>
#include <vector>
#include <functional>

namespace mpc::engine
{
    class StereoMixer;
    class IndivFxMixer;
} // namespace mpc::engine

namespace mpc::sequencer
{
    class Bus
    {
    public:
        virtual ~Bus() = default;

        Bus(BusType);
        Bus(const Bus &) = delete;
        Bus &operator=(const Bus &) = delete;
        Bus(Bus &&) = delete;
        Bus &operator=(Bus &&) = delete;

        const BusType busType;

    private:
        Bus() = delete;
    };

    class DrumBus final : public Bus
    {
    public:
        explicit DrumBus(DrumBusIndex, std::function<void(const DrumBusIndex, const ProgramIndex)> setProgramInPerformanceState);
        ~DrumBus() override = default;

        DrumBusIndex getIndex() const;

        ProgramIndex getProgramIndex() const;
        void setProgramIndex(ProgramIndex);

        bool receivesPgmChange() const;
        void setReceivePgmChange(bool b);

        bool receivesMidiVolume() const;
        void setReceiveMidiVolume(bool b);

        void setLastReceivedMidiVolume(int volume);
        int getLastReceivedMidiVolume() const;

        std::map<int, int> &getSimultA();
        std::map<int, int> &getSimultB();

        std::vector<std::shared_ptr<engine::StereoMixer>> &
        getStereoMixerChannels();
        std::vector<std::shared_ptr<engine::IndivFxMixer>> &
        getIndivFxMixerChannels();

    private:
        const DrumBusIndex drumIndex;
        const std::function<void(const DrumBusIndex, const ProgramIndex)> setProgramInPerformanceState;
        ProgramIndex programIndex{0};

        std::vector<std::shared_ptr<engine::StereoMixer>> stereoMixerChannels;
        std::vector<std::shared_ptr<engine::IndivFxMixer>> indivFxMixerChannels;

        std::map<int, int> simultA;
        std::map<int, int> simultB;

        bool receivePgmChange = false;
        bool receiveMidiVolume = false;
        int lastReceivedMidiVolume = 127;
    };

    class MidiBus final : public Bus
    {
    public:
        MidiBus();
        ~MidiBus() override = default;

        MidiBus(const MidiBus &) = delete;
        MidiBus &operator=(const MidiBus &) = delete;
        MidiBus(MidiBus &&) = delete;
        MidiBus &operator=(MidiBus &&) = delete;
    };

} // namespace mpc::sequencer
