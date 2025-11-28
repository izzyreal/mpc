#pragma once

#include "sequencer/SequencerAudioState.hpp"
#include "sequencer/SequencerAudioStateView.hpp"
#include "sequencer/SequencerAudioMessage.hpp"
#include "concurrency/AtomicStateExchange.hpp"

namespace mpc::sequencer
{
    class Sequencer;

    class SequencerAudioStateManager final
        : public concurrency::AtomicStateExchange<SequencerAudioState,
                                                  SequencerAudioStateView,
                                                  SequencerAudioMessage>
    {
    public:
        explicit SequencerAudioStateManager(Sequencer *);
        ~SequencerAudioStateManager() override;

    protected:
        void applyMessage(const SequencerAudioMessage &msg) noexcept override;

    private:
        Sequencer *sequencer;
    };
} // namespace mpc::sequencer
