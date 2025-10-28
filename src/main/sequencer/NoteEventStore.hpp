#pragma once

#include <unordered_map>
#include <queue>
#include <cassert>

namespace mpc::sequencer
{
    class NoteOnEvent;
    class NoteOnEventPlayOnly;

    struct MidiInputNoteKey
    {
        int track;
        int channel;
        int note;

        bool operator==(const MidiInputNoteKey& o) const noexcept
        {
            return track == o.track && channel == o.channel && note == o.note;
        }

        bool operator<(const MidiInputNoteKey& o) const noexcept
        {
            if (track != o.track) return track < o.track;
            if (channel != o.channel) return channel < o.channel;
            return note < o.note;
        }
    };

    template <typename IndexType> class NoteEventStore final
    {
    private:
        struct NoteHashFunctor
        {
            template <typename T1, typename T2>
            std::size_t operator()(const std::pair<T1, T2> &p) const
            {
                return std::hash<T1>{}(p.first) * 127 +
                       std::hash<T2>{}(p.second);
            }

            template <typename T1> std::size_t operator()(const T1 &p) const
            {
                return std::hash<T1>{}(p);
            }

            std::size_t operator()(const MidiInputNoteKey &k) const
            {
                size_t h1 = std::hash<int>{}(k.track);
                size_t h2 = std::hash<int>{}(k.channel);
                size_t h3 = std::hash<int>{}(k.note);
                return h1 ^ (h2 << 1) ^ (h3 << 2);
            }
        };

        std::unordered_map<IndexType,
                           std::queue<std::shared_ptr<NoteOnEventPlayOnly>>,
                           NoteHashFunctor>
            playStore;
        std::unordered_map<IndexType, std::queue<std::shared_ptr<NoteOnEvent>>,
                           NoteHashFunctor>
            recordStore;

    public:
        void storePlayNoteEvent(IndexType index,
                                std::shared_ptr<NoteOnEventPlayOnly> event);
        std::shared_ptr<NoteOnEventPlayOnly>
        retrievePlayNoteEvent(IndexType index);
        void storeRecordNoteEvent(IndexType index,
                                  std::shared_ptr<NoteOnEvent> event);
        std::shared_ptr<NoteOnEvent> retrieveRecordNoteEvent(IndexType index);

        void reserve(const size_t capacity)
        {
            playStore.reserve(capacity);
            recordStore.reserve(capacity);
        }

        void clearPlayAndRecordStore()
        {
            playStore.clear();
            recordStore.clear();
        }
    };

    template <typename IndexType>
    inline void NoteEventStore<IndexType>::storePlayNoteEvent(
        IndexType index, std::shared_ptr<NoteOnEventPlayOnly> event)
    {
        playStore[index].push(event);
    }

    template <typename IndexType>
    inline std::shared_ptr<NoteOnEventPlayOnly>
    NoteEventStore<IndexType>::retrievePlayNoteEvent(IndexType index)
    {
        if (playStore[index].empty())
        {
            return {};
        }

        auto event = playStore[index].front();
        playStore[index].pop();
        return event;
    }

    template <typename IndexType>
    inline void NoteEventStore<IndexType>::storeRecordNoteEvent(
        IndexType index, std::shared_ptr<NoteOnEvent> event)
    {
        assert(!std::dynamic_pointer_cast<NoteOnEventPlayOnly>(event));
        recordStore[index].push(event);
    }

    template <typename IndexType>
    inline std::shared_ptr<NoteOnEvent>
    NoteEventStore<IndexType>::retrieveRecordNoteEvent(IndexType index)
    {
        if (recordStore[index].empty())
        {
            return {};
        }

        auto event = recordStore[index].front();
        recordStore[index].pop();
        return event;
    }
} // namespace mpc::sequencer
