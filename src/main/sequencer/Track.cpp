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
    const utils::PostToUiThreadFn &postToUiThread,
    const std::function<std::string(int)> &getDefaultTrackName,
    const std::shared_ptr<SequencerStateManager> &manager,
    const std::function<std::shared_ptr<TrackStateView>(TrackIndex)>
        &getSnapshot,
    const std::function<void(TrackMessage &&)> &dispatch, const int trackIndex,
    Sequence *parent, const std::function<int64_t()> &getTickPosition,
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
    : postToUiThread(postToUiThread), getDefaultTrackName(getDefaultTrackName),
      parent(parent), manager(manager), getSnapshot(getSnapshot),
      dispatch(dispatch), trackIndex(trackIndex),
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
}

Track::~Track() {}

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
    dispatch(UpdateEvents{getSequenceIndex(), getIndex(), eventStates});
}

mpc::SequenceIndex Track::getSequenceIndex() const
{
    return parent->getSequenceIndex();
}

void Track::setUsedIfCurrentlyUnused(
    utils::SimpleAction &&onCompleteNameSetting) const
{
    if (isUsed() && getIndex() != TempoChangeTrackIndex)
    {
        onCompleteNameSetting();
        return;
    }

    if (getIndex() != TempoChangeTrackIndex)
    {
        setName(getDefaultTrackName(getIndex()));
    }

    dispatch(SetTrackUsed{getSequenceIndex(), getIndex(), true});

    if (getIndex() != TempoChangeTrackIndex)
    {
        manager->enqueueCallback(onCompleteNameSetting);
    }
}

bool Track::isTransmitProgramChangesEnabled() const
{
    return getSnapshot(getIndex())->isTransmitProgramChangesEnabled();
}

void Track::setTransmitProgramChangesEnabled(const bool b,
                                             const bool updateUsedness) const
{
    dispatch(SetTrackTransmitProgramChangesEnabled{getSequenceIndex(),
                                                   getIndex(), b});
    if (updateUsedness)
    {
        setUsedIfCurrentlyUnused();
    }
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

void Track::setOn(const bool b, const bool updateUsedness) const
{
    dispatch(SetTrackOn{getSequenceIndex(), getIndex(), b});

    if (updateUsedness)
    {
        setUsedIfCurrentlyUnused();
    }
}

void Track::removeEvent(const std::shared_ptr<EventRef> &event) const
{
    dispatch(RemoveEvent{event->handle});
}

void Track::removeEvent(EventData *eventState) const
{
    assert(eventState && eventState->trackIndex == getIndex() &&
           eventState->sequenceIndex == getSequenceIndex());
    dispatch(RemoveEvent{eventState});
}

void Track::removeEvents() const
{
    dispatch(RemoveEvents{getSequenceIndex(), getIndex()});
}

void Track::setVelocityRatio(const int i, const bool updateUsedness) const
{
    dispatch(
        SetTrackVelocityRatio{getSequenceIndex(), getIndex(),
                              static_cast<uint8_t>(std::clamp(i, 1, 200))});

    if (updateUsedness)
    {
        setUsedIfCurrentlyUnused();
    }
}

int Track::getVelocityRatio() const
{
    return getSnapshot(getIndex())->getVelocityRatio();
}

void Track::setProgramChange(const int i, const bool updateUsedness) const
{
    dispatch(
        SetTrackProgramChange{getSequenceIndex(), getIndex(),
                              static_cast<uint8_t>(std::clamp(i, 0, 128))});
    if (updateUsedness)
    {
        setUsedIfCurrentlyUnused();
    }
}

int Track::getProgramChange() const
{
    return getSnapshot(getIndex())->getProgramChange();
}

void Track::setBusType(BusType busType, const bool updateUsedness) const
{
    using U = std::underlying_type_t<BusType>;
    constexpr U MIN = static_cast<U>(BusType::MIDI);
    constexpr U MAX = static_cast<U>(BusType::DRUM4);

    const U raw = static_cast<U>(busType);
    const U clamped = std::clamp(raw, MIN, MAX);

    dispatch(SetTrackBusType{getSequenceIndex(), getIndex(),
                             static_cast<BusType>(clamped)});
    if (updateUsedness)
    {
        setUsedIfCurrentlyUnused();
    }
}

BusType Track::getBusType() const
{
    return getSnapshot(getIndex())->getBusType();
}

void Track::setDeviceIndex(const int i, const bool updateUsedness) const
{
    dispatch(SetTrackDeviceIndex{getSequenceIndex(), getIndex(),
                                 static_cast<uint8_t>(std::clamp(i, 0, 32))});
    if (updateUsedness)
    {
        setUsedIfCurrentlyUnused();
    }
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

void Track::setName(const std::string &s) const
{
    SetTrackName msg{getSequenceIndex(), getIndex()};
    std::snprintf(msg.name, sizeof(msg.name), "%s", s.c_str());
    dispatch(msg);
}

std::string Track::getName() const
{
    const auto snapshot = getSnapshot(getIndex());
    if (!snapshot->isUsed())
    {
        return "(Unused)";
    }
    return std::string(snapshot->getName());
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

std::vector<EventData *> Track::getEventHandles() const
{
    return getSnapshot(getIndex())->getEvents();
}

std::vector<std::shared_ptr<EventRef>> Track::getEvents() const
{
    std::vector<std::shared_ptr<EventRef>> result;
    auto &lock = manager->trackLocks[getSequenceIndex()][getIndex()];
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
    auto &lock = manager->trackLocks[getSequenceIndex()][getIndex()];
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
    dispatch(RemoveDoubles{getSequenceIndex(), getIndex()});
}

void Track::updateEventTick(EventData *handle, const int newTick) const
{
    dispatch(UpdateEventTick{handle, newTick});
}

std::vector<std::shared_ptr<NoteOnEvent>> Track::getNoteEvents() const
{
    std::vector<std::shared_ptr<NoteOnEvent>> result;

    auto &lock = manager->trackLocks[getSequenceIndex()][getIndex()];
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
    dispatch(SetPlayEventIndex{getSequenceIndex(), getIndex(), playEventIndex});
    manager->drainQueue();
}

void Track::insertAcquiredEvent(EventData *event,
                                const utils::SimpleAction &onComplete) const
{
    event->sequenceIndex = getSequenceIndex();
    event->trackIndex = trackIndex;
    dispatch(InsertAcquiredEvent{event, onComplete});
    setUsedIfCurrentlyUnused();
}

void Track::acquireAndInsertEvent(const EventData &eventState,
                                  const utils::SimpleAction &onComplete) const
{
    const auto e = manager->acquireEvent();
    *e = eventState;
    insertAcquiredEvent(e, onComplete);
}

EventData *Track::recordNoteEventNonLive(const int tick, const NoteNumber note,
                                         const Velocity velocity,
                                         const int64_t metronomeOnlyTick) const
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
    result->sequenceIndex = getSequenceIndex();
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
