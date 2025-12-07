#include "sequencer/Track.hpp"

#include "EventStateToEventMapper.hpp"
#include "SequencerStateManager.hpp"
#include "TrackStateView.hpp"
#include "sampler/Sampler.hpp"

#include "sequencer/EventRef.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/NoteOnEvent.hpp"
#include "audiomidi/EventHandler.hpp"

#include <concurrentqueue.h>

using namespace mpc::sequencer;
using namespace mpc::sampler;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

Track::Track(
    const std::shared_ptr<SequencerStateManager> &manager,
    const std::function<std::shared_ptr<TrackStateView>(TrackIndex)>
        &getSnapshot,
    const std::function<void(TrackMessage &&)> &dispatch, const int trackIndex,
    Sequence *parent,
    const std::function<std::string(int)> &getDefaultTrackName,
    const std::function<int64_t()> &getTickPosition,
    const std::function<std::shared_ptr<Screens>()> &getScreens,
    const std::function<bool()> &isRecordingModeMulti,
    const std::function<std::shared_ptr<Sequence>()> &getActiveSequence,
    const std::function<int()> &getAutoPunchMode,
    const std::function<std::shared_ptr<Bus>(BusType)> &getSequencerBus,
    const std::function<bool()> &isEraseButtonPressed,
    const std::function<bool(ProgramPadIndex, ProgramIndex)>
        &isProgramPadPressed,
    const std::shared_ptr<Sampler> &sampler,
    const std::shared_ptr<audiomidi::EventHandler> &eventHandler,
    const std::function<bool()> &isSixteenLevelsEnabled,
    const std::function<int()> &getActiveTrackIndex,
    const std::function<bool()> &isRecording,
    const std::function<bool()> &isOverdubbing,
    const std::function<bool()> &isPunchEnabled,
    const std::function<int64_t()> &getPunchInTime,
    const std::function<int64_t()> &getPunchOutTime,
    const std::function<bool()> &isSoloEnabled)
    : parent(parent), manager(manager), getSnapshot(getSnapshot),
      dispatch(dispatch), trackIndex(trackIndex),
      getDefaultTrackName(getDefaultTrackName),
      getTickPosition(getTickPosition), getScreens(getScreens),
      isRecordingModeMulti(isRecordingModeMulti),
      getActiveSequence(getActiveSequence), getAutoPunchMode(getAutoPunchMode),
      getSequencerBus(getSequencerBus),
      isEraseButtonPressed(isEraseButtonPressed),
      isProgramPadPressed(isProgramPadPressed), sampler(sampler),
      eventHandler(eventHandler),
      isSixteenLevelsEnabled(isSixteenLevelsEnabled),
      getActiveTrackIndex(getActiveTrackIndex), isRecording(isRecording),
      isOverdubbing(isOverdubbing), isPunchEnabled(isPunchEnabled),
      getPunchInTime(getPunchInTime), getPunchOutTime(getPunchOutTime),
      isSoloEnabled(isSoloEnabled)
{
    initializeTrackName();
}

Track::~Track() {}

void Track::initializeTrackName()
{
    name = trackIndex == TempoChangeTrackIndex
               ? "tempo"
               : getDefaultTrackName(trackIndex);
}

void Track::purge()
{
    initializeTrackName();
    removeEvents();
}


void Track::printEvents() const
{
    for (const auto &e : getEvents())
    {
        const auto eventState = e->handle;
        printf("Track event at tick %lld with note number %i\n",
               eventState->tick, eventState->noteNumber.get());
    }
}

void Track::setEventStates(const std::vector<EventData> &eventStates) const
{
    dispatch(UpdateEvents{parent->getSequenceIndex(), getIndex(), eventStates});
}

void Track::setTrackIndex(const TrackIndex i)
{
    if (trackIndex == i)
    {
        return;
    }
    trackIndex = i;
}

mpc::TrackIndex Track::getIndex() const
{
    return trackIndex;
}

void Track::setUsed(const bool b)
{
    if (!isUsed() && b && trackIndex < 64)
    {
        name = getDefaultTrackName(trackIndex);
    }

    dispatch(SetTrackUsed{parent->getSequenceIndex(), getIndex(), b});
}

void Track::setOn(const bool b) const
{
    dispatch(SetTrackOn{parent->getSequenceIndex(), getIndex(), b});
}

void Track::removeEvent(const std::shared_ptr<EventRef> &event) const
{
    dispatch(RemoveEvent{event->handle});
}

void Track::removeEvent(EventData *eventState) const
{
    assert(eventState && eventState->trackIndex == getIndex() &&
           eventState->sequenceIndex == parent->getSequenceIndex());
    dispatch(RemoveEvent{eventState});
}

void Track::removeEvents() const
{
    dispatch(ClearEvents{parent->getSequenceIndex(), getIndex()});
}

void Track::setVelocityRatio(const int i) const
{
    dispatch(
        SetTrackVelocityRatio{parent->getSequenceIndex(), getIndex(),
                              static_cast<uint8_t>(std::clamp(i, 1, 200))});
}

int Track::getVelocityRatio() const
{
    return getSnapshot(getIndex())->getVelocityRatio();
}

void Track::setProgramChange(const int i) const
{
    dispatch(
        SetTrackProgramChange{parent->getSequenceIndex(), getIndex(),
                              static_cast<uint8_t>(std::clamp(i, 0, 128))});
}

int Track::getProgramChange() const
{
    return getSnapshot(getIndex())->getProgramChange();
}

void Track::setBusType(BusType bt) const
{
    using U = std::underlying_type_t<BusType>;
    constexpr U MIN = static_cast<U>(BusType::MIDI);
    constexpr U MAX = static_cast<U>(BusType::DRUM4);

    const U raw = static_cast<U>(bt);
    const U clamped = std::clamp(raw, MIN, MAX);

    dispatch(SetTrackBusType{parent->getSequenceIndex(), getIndex(),
                             static_cast<BusType>(clamped)});
}

BusType Track::getBusType() const
{
    return getSnapshot(getIndex())->getBusType();
}

void Track::setDeviceIndex(const int i) const
{

    dispatch(SetTrackDeviceIndex{parent->getSequenceIndex(), getIndex(),
                                 static_cast<uint8_t>(std::clamp(i, 0, 32))});
}

int Track::getDeviceIndex() const
{
    return getSnapshot(getIndex())->getDeviceIndex();
}

std::shared_ptr<EventRef> Track::getEvent(const int i) const
{
    const auto eventHandle =
        getSnapshot(getIndex())->getEventByIndex(EventIndex(i));
    auto &lock =
        manager
            ->trackLocks[eventHandle->sequenceIndex][eventHandle->trackIndex];
    lock.acquire();
    const auto eventSnapshot = *eventHandle;
    lock.release();
    return mapEventStateToEvent(eventHandle, eventSnapshot, dispatch, parent);
}

void Track::setName(const std::string &s)
{
    name = s;
}

std::string Track::getName()
{
    if (!isUsed())
    {
        return "(Unused)";
    }
    return name;
}

std::vector<EventData> Track::getEventStates() const
{
    std::vector<EventData> result;
    for (const auto e : getSnapshot(getIndex())->getEvents())
    {
        result.push_back(*e);
    }
    return result;
}

std::vector<std::shared_ptr<EventRef>> Track::getEvents() const
{
    std::vector<std::shared_ptr<EventRef>> result;
    auto &lock = manager->trackLocks[parent->getSequenceIndex()][getIndex()];
    lock.acquire();

    const auto snapshot = getSnapshot(getIndex());

    const int eventCount = snapshot->getEventCount();

    for (int i = 0; i < eventCount; ++i)
    {
        const auto eventHandle = snapshot->getEventByIndex(EventIndex(i));
        const auto eventSnapshot = *eventHandle;
        auto event =
            mapEventStateToEvent(eventHandle, eventSnapshot, dispatch, parent);
        result.emplace_back(event);
    }

    lock.release();

    return result;
}

bool Track::isOn() const
{
    return getSnapshot(getIndex())->isOn();
}

bool Track::isUsed() const
{
    return getSnapshot(getIndex())->isUsed();
}

std::vector<std::shared_ptr<EventRef>>
Track::getEventRange(const int startTick, const int endTick) const
{
    auto &lock = manager->trackLocks[parent->getSequenceIndex()][getIndex()];
    lock.acquire();

    std::vector<std::shared_ptr<EventRef>> result;
    for (const auto &eventHandle :
         getSnapshot(getIndex())->getEventRange(startTick, endTick))
    {
        const auto eventSnapshot = *eventHandle;
        result.emplace_back(
            mapEventStateToEvent(eventHandle, eventSnapshot, dispatch, parent));
    }

    lock.release();
    return result;
}

void Track::removeDoubles() const
{
    dispatch(RemoveDoubles{parent->getSequenceIndex(), getIndex()});
}

void Track::updateEventTick(EventData *handle, const int newTick) const
{
    dispatch(UpdateEventTick{handle, newTick});
}

std::vector<std::shared_ptr<NoteOnEvent>> Track::getNoteEvents() const
{
    std::vector<std::shared_ptr<NoteOnEvent>> result;

    auto &lock = manager->trackLocks[parent->getSequenceIndex()][getIndex()];
    lock.acquire();

    for (const auto &eventHandle : getSnapshot(getIndex())->getNoteEvents())
    {
        const auto eventSnapshot = *eventHandle;
        result.emplace_back(
            std::dynamic_pointer_cast<NoteOnEvent>(mapEventStateToEvent(
                eventHandle, eventSnapshot, dispatch, parent)));
    }

    lock.release();

    return result;
}

std::string Track::getActualName()
{
    return name;
}

int Track::getNextTick() const
{
    const auto snapshot = getSnapshot(getIndex());
    const auto playEventIndex = snapshot->getPlayEventIndex();

    if (playEventIndex >= snapshot->getEventCount())
    {
        return std::numeric_limits<int>::max();
    }

    return snapshot->getEventByIndex(playEventIndex)->tick;
}

void Track::playNext() const
{
    const auto snapshot = getSnapshot(getIndex());
    auto playEventIndex = snapshot->getPlayEventIndex();
    const auto event = snapshot->getEventByIndex(playEventIndex);

    if (!event)
    {
        return;
    }

    const bool shouldDelete = shouldRemovePlayIndexEventDueToRecording() ||
                              shouldRemovePlayIndexEventDueToErasePressed();

    if (shouldDelete && !event->dontDelete)
    {
        dispatch(RemoveEvent{event});
        manager->drainQueue();
        return;
    }

    event->dontDelete = false;

    if (isOn() && (!isSoloEnabled() || getActiveTrackIndex() == trackIndex))
    {
        eventHandler->handleFinalizedEvent(*event, getIndex(),
                                           getVelocityRatio(), getBusType(),
                                           getDeviceIndex());
    }

    playEventIndex = playEventIndex + 1;
    dispatch(SetPlayEventIndex{parent->getSequenceIndex(), getIndex(),
                               playEventIndex});
    manager->drainQueue();
}

void Track::insertAcquiredEvent(EventData *event,
                                const utils::SimpleAction &onComplete)
{
    if (!isUsed())
    {
        setUsed(true);
    }

    event->sequenceIndex = parent->getSequenceIndex();
    event->trackIndex = trackIndex;

    dispatch(InsertAcquiredEvent{event, onComplete});
}

void Track::acquireAndInsertEvent(const EventData &eventState,
                                  const utils::SimpleAction &onComplete)
{
    const auto e = manager->acquireEvent();
    *e = eventState;
    insertAcquiredEvent(e, onComplete);
}

EventData *Track::recordNoteEventNonLive(const int tick, const NoteNumber note,
                                         const Velocity velocity,
                                         const int64_t metronomeOnlyTick)
{
    if (const auto result = getSnapshot(getIndex())->findNoteEvent(tick, note))
    {
        result->beingRecorded = true;
        result->velocity = velocity;
        result->duration = Duration(1);
        result->metronomeOnlyTickPosition = metronomeOnlyTick;
        return result;
    }

    EventData *result = manager->acquireEvent();
    result->type = EventType::NoteOn;
    result->noteNumber = note;
    result->velocity = velocity;
    result->duration = Duration(1);
    result->sequenceIndex = parent->getSequenceIndex();
    result->trackIndex = trackIndex;
    result->tick = tick;
    result->beingRecorded = true;
    result->metronomeOnlyTickPosition = metronomeOnlyTick;
    insertAcquiredEvent(result);
    return result;
}

void Track::finalizeNoteEventNonLive(EventData *noteOnEvent,
                                     const Duration duration) const
{
    dispatch(FinalizeNoteEventNonLive{noteOnEvent, duration});
}
