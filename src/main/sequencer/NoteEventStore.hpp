#pragma once
#include <unordered_map>
#include "sequencer/NoteEvent.hpp"

namespace mpc::sequencer
{
	template <typename IndexType, typename NoteType>
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
		std::unordered_map<IndexType, std::shared_ptr<NoteType>, NoteHashFunctor> store = {};
	public:
		bool storeNoteEvent(IndexType index, std::shared_ptr<NoteType> event);
		std::shared_ptr<NoteType> retrieveNoteEvent(IndexType index);
	};
	
	template<typename IndexType, typename NoteType>
	inline bool NoteEventStore<IndexType, NoteType>::storeNoteEvent(IndexType index, std::shared_ptr<NoteType> event)
	{
		if (store.find(index) != store.end()) return false;
		store[index] = event;
		return true;
	}
	template<typename IndexType, typename NoteType>
	inline std::shared_ptr<NoteType> NoteEventStore<IndexType, NoteType>::retrieveNoteEvent(IndexType index)
	{
		if (store.find(index) == store.end()) return nullptr;
		auto event = store[index];
		store.erase(index);
		return event;
	}
}
