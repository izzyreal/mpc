#pragma once
#include <unordered_map>
#include <queue>
#include "sequencer/NoteEvent.hpp"


namespace mpc::sequencer
{
	template <typename IndexType>
	class NoteEventStore
	{
	private:
		struct NoteHashFunctor
		{
			template <typename T1, typename T2>
			std::size_t operator () (const std::pair<T1, T2>& p) const {return std::hash<T1>{}(p.first) * 127 + std::hash<T2>{}(p.second);}
			template <typename T1>
			std::size_t operator () (const T1& p) const{return std::hash<T1>{}(p);}
		};
		std::unordered_map<IndexType, std::queue<std::shared_ptr<NoteOnEventPlayOnly>>, NoteHashFunctor> playStore = {};
		std::unordered_map<IndexType, std::queue<std::shared_ptr<NoteOnEvent>>, NoteHashFunctor> recordStore = {};
	public:
		void storePlayNoteEvent(IndexType index, std::shared_ptr<NoteOnEventPlayOnly> event);
		std::shared_ptr<NoteOnEventPlayOnly> retrievePlayNoteEvent(IndexType index);
		void storeRecordNoteEvent(IndexType index, std::shared_ptr<NoteOnEvent> event);
		std::shared_ptr<NoteOnEvent> retrieveRecordNoteEvent(IndexType index);
		void clearStores() { playStore.clear(); recordStore.clear(); }
		void clearPlayStore() { playStore.clear(); }
		void clearRecordStore() { recordStore.clear(); }

	};
	
	template<typename IndexType>
	inline void NoteEventStore<IndexType>::storePlayNoteEvent(IndexType index, std::shared_ptr<NoteOnEventPlayOnly> event)
	{
		playStore[index].push(event);
	}
	template<typename IndexType>
	inline std::shared_ptr<NoteOnEventPlayOnly> NoteEventStore<IndexType>::retrievePlayNoteEvent(IndexType index)
	{
		if (playStore[index].empty()) return nullptr;
		auto event = playStore[index].front();
		playStore[index].pop();
		return event;
	}
	template<typename IndexType>
	inline void NoteEventStore<IndexType>::storeRecordNoteEvent(IndexType index, std::shared_ptr<NoteOnEvent> event)
	{
		assert(!std::dynamic_pointer_cast<NoteOnEventPlayOnly>(event));
		recordStore[index].push(event);
	}
	template<typename IndexType>
	inline std::shared_ptr<NoteOnEvent> NoteEventStore<IndexType>::retrieveRecordNoteEvent(IndexType index)
	{
		if (recordStore[index].empty()) return nullptr;
		auto event = recordStore[index].front();
		recordStore[index].pop();
		return event;
	}
}
