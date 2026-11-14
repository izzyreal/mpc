#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "audiomidi/EventHandler.hpp"
#include "audiomidi/MidiOutput.hpp"
#include "client/event/ClientMidiEvent.hpp"
#include "concurrency/BoundedMpmcQueue.hpp"
#include "concurrency/UiTaskQueue.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientMidiEventController.hpp"
#include "engine/EngineHost.hpp"
#include "engine/SequencerPlaybackEngine.hpp"
#include "disk/AllLoader.hpp"
#include "file/kaitai/AllIo.hpp"
#include "file/kaitai/MidIo.hpp"
#include "file/kaitai/generated/mpc2000xl_all.h"
#include "lcdgui/Screens.hpp"
#include "lcdgui/Field.hpp"
#include "lcdgui/screens/StepEditorScreen.hpp"
#include "lcdgui/screens/window/InsertEventScreen.hpp"
#include "lcdgui/screens/window/PasteEventScreen.hpp"
#include "sequencer/OwnedEventSnapshot.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/SystemExclusiveEvent.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/Transport.hpp"

#include <atomic>
#include <thread>
#include <type_traits>
#include <sstream>
#include <kaitai/kaitaistream.h>

using namespace mpc;
using namespace mpc::sequencer;
using namespace mpc::client::event;

namespace
{
    std::vector<uint8_t> message(size_t size)
    {
        std::vector<uint8_t> bytes(size);
        for (size_t i = 1; i + 1 < size; ++i)
        {
            bytes[i] = i % 128;
        }
        bytes.front() = 0xF0;
        bytes.back() = 0xF7;
        return bytes;
    }

    OwnedEventSnapshot event(Mpc &mpc, size_t size, int tick = 0)
    {
        OwnedEventSnapshot result;
        const auto bytes = message(size);
        result.setSysExBytes(*mpc.sysExPool, bytes.data(), bytes.size());
        result.tick = tick;
        return result;
    }

    void checkPayload(const EventData &event,
                      const std::vector<uint8_t> &expected)
    {
        REQUIRE(event.getSysExByteCount() == expected.size());
        CHECK(std::equal(expected.begin(), expected.end(),
                         event.sysExPayload->data()));
    }

    void vlq(std::vector<char> &out, unsigned value)
    {
        uint8_t bytes[4];
        int size = 0;
        bytes[size++] = value & 127;
        while ((value >>= 7))
        {
            bytes[size++] = 128 | (value & 127);
        }
        while (size)
        {
            out.push_back(bytes[--size]);
        }
    }

    std::vector<char> midiWithSysEx(size_t size)
    {
        std::vector<char> track;
        vlq(track, 12);
        track.push_back(char(0xF0));
        const auto bytes = message(size);
        vlq(track, size - 1); // SMF excludes the F0 status from the length.
        track.insert(track.end(), bytes.begin() + 1, bytes.end());
        track.insert(track.end(), {24, char(0x90), 60, 100, 12, char(0x80), 60,
                                   0, 0, char(0xFF), 0x2F, 0});
        const char header[] =
            "MThd\x00\x00\x00\x06\x00\x00\x00\x01\x00\x60MTrk";
        std::vector<char> out(header, header + 18);
        for (int i = 3; i >= 0; --i)
        {
            out.push_back(char(track.size() >> (i * 8)));
        }
        out.insert(out.end(), track.begin(), track.end());
        return out;
    }
} // namespace

TEST_CASE("SysEx pool reserves transient slots and shares immutable payloads",
          "[sysex]")
{
    static_assert(std::is_trivially_destructible_v<EventData>);
    unsigned reports = 0;
    auto pool = std::make_shared<SysExPool>(
        [&]
        {
            ++reports;
        });
    const auto bytes = message(512);
    std::vector<SysExPayloadOwner> persistent, transient;
    for (unsigned i = 0; i < SysExPool::PersistentLimit; ++i)
    {
        auto *p = pool->acquire(bytes.data(), bytes.size());
        REQUIRE(p);
        REQUIRE(pool->replaceStored(nullptr, p));
        persistent.emplace_back(p);
    }
    REQUIRE(pool->persistentUsed() == 768);
    {
        SysExReportBatch batch;
        for (int i = 0; i < 100; ++i)
        {
            auto *p = pool->acquire(bytes.data(), bytes.size());
            REQUIRE(p);
            CHECK_FALSE(pool->replaceStored(nullptr, p));
            pool->release(p);
        }
    }
    CHECK(reports == 1);
    for (unsigned i = 0; i < 256; ++i)
    {
        transient.emplace_back(pool->acquire(bytes.data(), bytes.size()));
    }
    CHECK(pool->used() == 1024);
    CHECK_FALSE(pool->acquire(bytes.data(), bytes.size()));
    CHECK(reports == 2);
    transient.clear();
    auto copy = persistent.front();
    REQUIRE(copy.get());
    REQUIRE(copy.get() == persistent.front().get());
    SysExPayloadOwner edited(pool->acquireEdited(copy.get(), 40, 99));
    REQUIRE(edited.get());
    CHECK(edited.get()->data()[40] == 99);
    CHECK(persistent.front().get()->data()[40] == 40);
    copy = {};
    for (const auto &item : persistent)
        REQUIRE(pool->replaceStored(item.get(), nullptr));
    persistent.clear();
    edited = {};
    CHECK(pool->used() == 0);
    CHECK_FALSE(pool->acquire(bytes.data(), 513));
    CHECK(pool->used() == 0);
}

TEST_CASE("SysEx replacement transfers quota at the stored-slot limit",
          "[sysex]")
{
    auto pool = std::make_shared<SysExPool>();
    const auto bytes = message(267);
    std::vector<SysExPayloadOwner> stored;
    stored.reserve(SysExPool::PersistentLimit);
    for (unsigned i = 0; i < SysExPool::PersistentLimit; ++i)
    {
        auto *p = pool->acquire(bytes.data(), bytes.size());
        REQUIRE(p);
        REQUIRE(pool->replaceStored(nullptr, p));
        stored.emplace_back(p);
    }

    SysExPayloadOwner edited(
        pool->acquireEdited(stored.front().get(), 40, 99));
    REQUIRE(edited.get());
    REQUIRE(pool->replaceStored(stored.front().get(), edited.get()));
    stored.front() = std::move(edited);
    CHECK(pool->persistentUsed() == SysExPool::PersistentLimit);
    CHECK(pool->used() == SysExPool::PersistentLimit);

    REQUIRE(pool->replaceStored(nullptr, stored[1].get()));
    SysExPayloadOwner another(
        pool->acquireEdited(stored[1].get(), 40, 100));
    REQUIRE(another.get());
    CHECK_FALSE(pool->replaceStored(stored[1].get(), another.get()));
    REQUIRE(pool->replaceStored(stored[1].get(), nullptr));
    another = {};
    for (const auto &item : stored)
        REQUIRE(pool->replaceStored(item.get(), nullptr));
    stored.clear();
    CHECK(pool->used() == 0);
    CHECK(pool->persistentUsed() == 0);
}

TEST_CASE("SysEx owners survive queue moves, rejection, and queue destruction",
          "[sysex][queue]")
{
    auto pool = std::make_shared<SysExPool>();
    const auto bytes = message(267);
    {
        concurrency::BoundedMpmcQueue<SysExPayloadOwner, 2> queue;
        SysExPayloadOwner a(pool->acquire(bytes.data(), bytes.size()));
        SysExPayloadOwner b(pool->acquire(bytes.data(), bytes.size()));
        CHECK(queue.enqueue(std::move(a)));
        CHECK(queue.enqueue(std::move(b)));
        SysExPayloadOwner rejected(pool->acquire(bytes.data(), bytes.size()));
        CHECK_FALSE(queue.enqueue(std::move(rejected)));
        REQUIRE(rejected.get());
        SysExPayloadOwner out(pool->acquire(bytes.data(), bytes.size()));
        CHECK(
            queue.dequeue(out)); // Replaces and releases an already owned slot.
        CHECK(pool->used() == 3);
        std::vector<SysExPayloadOwner> output(1);
        CHECK(queue.dequeue_bulk(output) == 1);
        CHECK(queue.enqueue(std::move(out)));
    }
    CHECK(pool->used() == 0);
}

TEST_CASE("State queue bounds retries of a deferred message", "[queue]")
{
    struct View
    {
        explicit View(int *) {}
    };
    struct RetryingExchange
        : concurrency::AtomicStateExchange<int, View, int, 3, 2, 8>
    {
        RetryingExchange() : AtomicStateExchange([](int &) {}) {}
        int attempts = 0;
        void applyMessage(const int &message) noexcept override
        {
            ++attempts;
            defer(message);
        }
    };

    RetryingExchange exchange;
    REQUIRE(exchange.enqueue(1));
    exchange.drainQueue();
    CHECK(exchange.attempts == 1);
    exchange.drainQueue();
    CHECK(exchange.attempts == 2);
}

TEST_CASE("Deferred state messages survive a full queue and delay callbacks",
          "[queue]")
{
    struct View
    {
        explicit View(int *) {}
    };
    struct Exchange : concurrency::AtomicStateExchange<int, View, int, 3, 2, 8>
    {
        Exchange() : AtomicStateExchange([](int &) {}) {}
        bool blocked = true;
        int applied = 0;
        void applyMessage(const int &message) noexcept override
        {
            if (blocked)
            {
                defer(message);
                return;
            }
            applied += message;
        }
    };

    Exchange exchange;
    int callbackSaw = -1;
    REQUIRE(exchange.enqueue(1));
    REQUIRE(exchange.enqueue(2));
    exchange.enqueueCallback(utils::SimpleAction(
        [&] { callbackSaw = exchange.applied; }));
    exchange.drainQueue();
    CHECK(exchange.applied == 0);
    CHECK(callbackSaw == -1);
    REQUIRE(exchange.enqueue(3));
    exchange.blocked = false;
    exchange.drainQueue();
    CHECK(exchange.applied == 3);
    CHECK(callbackSaw == -1);
    exchange.drainQueue();
    CHECK(exchange.applied == 6);
    CHECK(callbackSaw == 6);

    REQUIRE(exchange.enqueue(4));
    exchange.enqueueCallback(utils::SimpleAction(
        [&] { callbackSaw = exchange.applied; }));
    exchange.applyMessageImmediate(5);
    CHECK(callbackSaw == 6);
    exchange.drainQueue();
    CHECK(callbackSaw == 15);
}

TEST_CASE("A full deferred queue rejects without terminating", "[queue]")
{
    struct View
    {
        explicit View(int *) {}
    };
    struct Exchange : concurrency::AtomicStateExchange<int, View, int, 3, 2, 2>
    {
        Exchange() : AtomicStateExchange([](int &) {}) {}
        int rejected = 0;
        void applyMessage(const int &message) noexcept override
        {
            defer(message);
        }
        void onDeferredOverflow(const int &) noexcept override { ++rejected; }
    };

    Exchange exchange;
    exchange.applyMessageImmediate(1);
    exchange.applyMessageImmediate(2);
    exchange.applyMessageImmediate(3);
    CHECK(exchange.rejected == 1);
    CHECK(exchange.getDeferredOverflowCount() == 1);
}

TEST_CASE("SysEx failure notifications coalesce across producers",
          "[sysex][queue]")
{
    concurrency::UiTaskQueue queue;
    // A full queue must clear the pending flag after a rejected log task.
    unsigned executed = 0;
    for (int i = 0; i < 128; ++i)
    {
        utils::Task task(
            [&]
            {
                ++executed;
            });
        REQUIRE(queue.post(task));
    }
    CHECK_FALSE(queue.post(concurrency::LogSysExRamFullTask{}));
    queue.drain();
    CHECK(executed == 128);
    std::vector<std::thread> producers;
    for (int i = 0; i < 4; ++i)
    {
        producers.emplace_back(
            [&]
            {
                for (int n = 0; n < 1000; ++n)
                {
                    queue.post(concurrency::LogSysExRamFullTask{});
                }
            });
    }
    for (auto &producer : producers)
    {
        producer.join();
    }
    // Exactly one log task occupies the queue; 127 ordinary tasks still fit.
    for (int i = 0; i < 127; ++i)
    {
        utils::Task task([] {});
        REQUIRE(queue.post(task));
    }
    utils::Task overflow([] {});
    CHECK_FALSE(queue.post(overflow));
    queue.drain();
    CHECK(queue.post(concurrency::LogSysExRamFullTask{}));
    queue.drain();
}

TEST_CASE("ALL and MID preserve complete SysEx payloads at supported lengths",
          "[sysex][kaitai-all][kaitai-mid]")
{
    Mpc source;
    TestMpc::initializeTestMpcWithoutIoServices(source);
    const auto sequence = source.getSequencer()->getSequence(0);
    const auto manager = source.getSequencer()->getStateManager();
    sequence->init(0);
    manager->drainQueue();
    int tick = 0;
    const size_t lengths[] = {2, 128, 129, 267, 512};
    for (const auto size : lengths)
    {
        auto data = event(source, size, tick++ * 12);
        sequence->getTrack(0)->acquireAndInsertEvent(data);
    }
    manager->drainQueue();
    SECTION("ALL")
    {
        const auto bytes = file::kaitai::AllIo::save(source);
        Mpc loaded;
        TestMpc::initializeTestMpcWithoutIoServices(loaded);
        disk::AllLoader::loadEverythingFromBytes(loaded, bytes);
        loaded.getSequencer()->getStateManager()->drainQueue();
        const auto states = loaded.getSequencer()
                                ->getSequence(0)
                                ->getTrack(0)
                                ->getEventStates();
        REQUIRE(states.size() == 5);
        for (size_t i = 0; i < states.size(); ++i)
        {
            checkPayload(states[i], message(lengths[i]));
        }
    }
    SECTION("MID")
    {
        const auto bytes = file::kaitai::MidIo::saveBytes(sequence);
        const auto loaded =
            file::kaitai::MidIo::loadBytes(source, bytes, "SYSEX");
        REQUIRE(loaded);
        manager->drainQueue();
        const auto states = (*loaded)->getTrack(0)->getEventStates();
        REQUIRE(states.size() == 5);
        for (size_t i = 0; i < states.size(); ++i)
        {
            checkPayload(states[i], message(lengths[i]));
        }
    }
}

TEST_CASE("MID omits oversized SysEx without shifting following notes",
          "[sysex][kaitai-mid]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    for (const auto size : {size_t{512}, size_t{513}})
    {
        const auto loaded =
            file::kaitai::MidIo::loadBytes(mpc, midiWithSysEx(size), "TEST");
        REQUIRE(loaded);
        mpc.getSequencer()->getStateManager()->drainQueue();
        const auto states = (*loaded)->getTrack(0)->getEventStates();
        REQUIRE(states.size() == (size == 512 ? 2 : 1));
        CHECK(states.back().type == EventType::NoteOn);
        CHECK(states.back().tick == 36);
        if (size == 512)
        {
            checkPayload(states.front(), message(size));
        }
    }
}

TEST_CASE("ALL omits oversized SysEx and preserves the next event",
          "[sysex][kaitai-all]")
{
    Mpc source;
    TestMpc::initializeTestMpcWithoutIoServices(source);
    const auto seq = source.getSequencer()->getSelectedSequence();
    const auto manager = source.getSequencer()->getStateManager();
    seq->init(0);
    manager->drainQueue();
    seq->getTrack(0)->acquireAndInsertEvent(event(source, 512));
    EventData note;
    note.type = EventType::NoteOn;
    note.tick = 36;
    note.noteNumber = NoteNumber(60);
    note.velocity = Velocity(100);
    note.duration = Duration(12);
    seq->getTrack(0)->acquireAndInsertEvent(note);
    manager->drainQueue();
    const auto bytes = file::kaitai::AllIo::save(source);
    std::istringstream input(std::string(bytes.begin(), bytes.end()),
                             std::ios::binary);
    kaitai::kstream reader(&input);
    mpc2000xl_all_t parsed(&reader);
    parsed._read();
    auto *exclusive =
        parsed.sequences()->at(0)->body()->events()->at(0)->exclusive();
    REQUIRE(exclusive);
    const auto oversized = message(513);
    exclusive->set_bytes(std::string(oversized.begin(), oversized.end()));
    exclusive->set_payload_length(513);
    exclusive->set_padding(std::string(7, '\0'));
    std::stringstream output(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writer(&output);
    parsed._set_io(&writer);
    parsed._check();
    parsed._write();
    const auto encoded = output.str();
    Mpc loaded;
    TestMpc::initializeTestMpcWithoutIoServices(loaded);
    disk::AllLoader::loadEverythingFromBytes(loaded,
                                             {encoded.begin(), encoded.end()});
    loaded.getSequencer()->getStateManager()->drainQueue();
    const auto states =
        loaded.getSequencer()->getSelectedTrack()->getEventStates();
    REQUIRE(states.size() == 1);
    CHECK(states[0].type == EventType::NoteOn);
    CHECK(states[0].tick == 36);
}

TEST_CASE("SysEx copies share bytes until an edit", "[sysex]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto sequencer = mpc.getSequencer();
    const auto manager = sequencer->getStateManager();
    const auto sequence = sequencer->getSequence(0);
    sequence->init(0);
    manager->drainQueue();
    const auto track = sequence->getTrack(0);
    auto data = event(mpc, 267);
    track->acquireAndInsertEvent(data);
    manager->drainQueue();
    auto snapshot = track->getEventStates();
    REQUIRE(snapshot[0].sysExPayload == data.sysExPayload);
    CHECK(mpc.sysExPool->used() == 1);
    auto ref =
        std::dynamic_pointer_cast<SystemExclusiveEvent>(track->getEvent(0));
    REQUIRE(ref);
    ref->setByte(200, 99);
    manager->drainQueue();
    CHECK(snapshot[0].getSysExByte(200) == 72);
    CHECK(data.getSysExByte(200) == 72);
    CHECK(track->getEvent(0)->snapshot.getSysExByte(200) == 99);
    sequencer->copySequence(SequenceIndex(0), SequenceIndex(1));
    manager->drainQueue();
    const auto copied =
        sequencer->getSequence(1)->getTrack(0)->getEventStates();
    REQUIRE(copied.size() == 1);
    CHECK(copied[0].getSysExByte(200) == 99);
    CHECK(copied[0].sysExPayload != snapshot[0].sysExPayload);
    track->removeEvents();
    manager->drainQueue();
    CHECK(copied[0].getSysExByte(200) == 99);
}

TEST_CASE("Copying bars shares immutable SysEx while edits stay independent",
          "[sysex][bars-screen]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto sequencer = mpc.getSequencer();
    const auto manager = sequencer->getStateManager();
    const auto source = sequencer->getSequence(0);
    const auto destination = sequencer->getSequence(1);
    source->init(0);
    destination->init(0);
    manager->drainQueue();
    source->getTrack(0)->acquireAndInsertEvent(event(mpc, 267));
    manager->drainQueue();
    manager->enqueue(CopyBars{SequenceIndex(0), SequenceIndex(1), BarIndex(0),
                              BarIndex(0), 1, BarIndex(0), 1});
    manager->drainQueue();
    auto sourceRef = std::dynamic_pointer_cast<SystemExclusiveEvent>(
        source->getTrack(0)->getEvent(0));
    REQUIRE(sourceRef);
    CHECK(mpc.sysExPool->used() == 1);
    sourceRef->setByte(200, 99);
    manager->drainQueue();
    CHECK(mpc.sysExPool->used() == 2);
    const auto copied = destination->getTrack(0)->getEventStates();
    REQUIRE(copied.size() == 1);
    checkPayload(copied[0], message(267));
    source->getTrack(0)->removeEvents();
    manager->drainQueue();
    checkPayload(destination->getTrack(0)->getEvent(0)->snapshot, message(267));
}

TEST_CASE("A contended bar copy retains more event inserts than the retry ring",
          "[queue][bars-screen]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto sequencer = mpc.getSequencer();
    const auto manager = sequencer->getStateManager();
    const auto source = sequencer->getSequence(0);
    const auto destination = sequencer->getSequence(1);
    source->init(0);
    destination->init(0);
    manager->drainQueue();

    EventData note;
    note.type = EventType::NoteOn;
    note.tick = 0;
    note.noteNumber = NoteNumber(60);
    note.velocity = Velocity(100);
    note.duration = Duration(1);
    constexpr int eventCount = 2100;
    for (int i = 0; i < eventCount; ++i)
    {
        source->getTrack(0)->acquireAndInsertEvent(note);
        if (i % 100 == 99) manager->drainQueue();
    }
    REQUIRE(source->getTrack(0)->getEventStates().size() == eventCount);

    auto &lock = manager->trackLocks[SequenceIndex(1)][TrackIndex(0)];
    lock.acquire();
    REQUIRE(manager->enqueue(CopyBars{SequenceIndex(0), SequenceIndex(1),
                                      BarIndex(0), BarIndex(0), 1, BarIndex(0),
                                      1}));
    bool callbackRan = false;
    manager->enqueueCallback(utils::SimpleAction([&] { callbackRan = true; }));
    manager->drainQueue();
    CHECK_FALSE(callbackRan);
    manager->drainQueue();
    manager->drainQueue();
    CHECK_FALSE(callbackRan);
    lock.release();

    for (int i = 0; i < 10 && !callbackRan; ++i) manager->drainQueue();
    CHECK(callbackRan);
    CHECK(destination->getTrack(0)->getEventStates().size() == eventCount);
}

TEST_CASE("Pending bar-copy SysEx is released on shutdown", "[sysex][queue]")
{
    std::shared_ptr<SysExPool> pool;
    {
        Mpc mpc;
        TestMpc::initializeTestMpcWithoutIoServices(mpc);
        pool = mpc.sysExPool;
        const auto sequencer = mpc.getSequencer();
        const auto manager = sequencer->getStateManager();
        const auto source = sequencer->getSequence(0);
        const auto destination = sequencer->getSequence(1);
        source->init(0);
        destination->init(0);
        manager->drainQueue();
        source->getTrack(0)->acquireAndInsertEvent(event(mpc, 267));
        manager->drainQueue();

        auto &lock = manager->trackLocks[SequenceIndex(1)][TrackIndex(0)];
        lock.acquire();
        REQUIRE(manager->enqueue(CopyBars{SequenceIndex(0), SequenceIndex(1),
                                          BarIndex(0), BarIndex(0), 1,
                                          BarIndex(0), 1}));
        manager->drainQueue();
        lock.release();
    }
    CHECK(pool->used() == 0);
}

TEST_CASE("A rejected bar-copy retry releases pending SysEx",
          "[sysex][queue]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto pool = mpc.sysExPool;
    const auto sequencer = mpc.getSequencer();
    const auto manager = sequencer->getStateManager();
    const auto source = sequencer->getSequence(0);
    const auto destination = sequencer->getSequence(1);
    source->init(0);
    destination->init(0);
    manager->drainQueue();
    source->getTrack(0)->acquireAndInsertEvent(event(mpc, 267));
    manager->drainQueue();

    for (int i = 0; i < 2048; ++i)
        REQUIRE(manager->defer(SetLoopEnabled{SequenceIndex(2), true}));
    auto &lock = manager->trackLocks[SequenceIndex(1)][TrackIndex(0)];
    lock.acquire();
    manager->applyMessageImmediate(CopyBars{SequenceIndex(0), SequenceIndex(1),
                                            BarIndex(0), BarIndex(0), 1,
                                            BarIndex(0), 1});
    lock.release();
    CHECK(manager->getDeferredOverflowCount() >= 2);

    source->getTrack(0)->removeEvents();
    for (int i = 0; i < 8; ++i) manager->drainQueue();
    CHECK(pool->used() == 0);
}

TEST_CASE("Undo swaps complete SysEx payloads without sharing mutable bytes",
          "[sysex]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto sequencer = mpc.getSequencer();
    const auto manager = sequencer->getStateManager();
    const auto sequence = sequencer->getSelectedSequence();
    sequence->init(0);
    manager->drainQueue();
    const auto track = sequence->getTrack(0);
    track->acquireAndInsertEvent(event(mpc, 512));
    manager->drainQueue();
    sequencer->copySelectedSequenceToUndoSequence();
    manager->drainQueue();
    auto ref =
        std::dynamic_pointer_cast<SystemExclusiveEvent>(track->getEvent(0));
    REQUIRE(ref);
    ref->setByte(400, 99);
    manager->drainQueue();
    CHECK(track->getEvent(0)->snapshot.getSysExByte(400) == 99);
    sequencer->undoSeq();
    manager->drainQueue();
    checkPayload(track->getEvent(0)->snapshot, message(512));
    sequencer->undoSeq();
    manager->drainQueue();
    CHECK(track->getEvent(0)->snapshot.getSysExByte(400) == 99);
}

TEST_CASE("Step editor inserts bounded SysEx and scrolls through every byte",
          "[sysex][step-editor]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto sequencer = mpc.getSequencer();
    const auto manager = sequencer->getStateManager();
    sequencer->getSelectedSequence()->init(0);
    manager->drainQueue();
    const auto screen = mpc.getLayeredScreen();
    screen->openScreenById(lcdgui::ScreenId::InsertEventScreen);
    const auto insert = mpc.screens->get<lcdgui::ScreenId::InsertEventScreen>();
    REQUIRE(screen->setFocus("eventtype"));
    insert->turnWheel(6);
    REQUIRE(screen->setFocus("bytes"));
    insert->turnWheel(1000);
    insert->function(4);
    manager->drainQueue();
    screen->timerCallback();
    const auto track = sequencer->getSelectedTrack();
    const auto states = track->getEventStates();
    REQUIRE(states.size() == 1);
    REQUIRE(states[0].getSysExByteCount() == 128);
    CHECK(states[0].getSysExByte(0) == 0xF0);
    CHECK(states[0].getSysExByte(127) == 0xF7);
    for (size_t i = 1; i < 127; ++i)
    {
        CHECK(states[0].getSysExByte(i) == 0);
    }
    const auto step = mpc.screens->get<lcdgui::ScreenId::StepEditorScreen>();
    REQUIRE(screen->setFocus("a0"));
    for (int i = 0; i < 127; ++i)
    {
        step->right();
    }
    CHECK(screen->getFocusedFieldName() == "l0");
    step->right(); // At the last byte, stay put.
    step->turnWheel(-1);
    manager->drainQueue();
    CHECK(track->getEvent(0)->snapshot.getSysExByte(127) == 0xF6);
    CHECK(track->getEvent(0)->snapshot.getSysExByte(126) == 0);
    for (int i = 0; i < 127; ++i)
    {
        step->left();
    }
    CHECK(screen->getFocusedFieldName() == "a0");
    step->left();
    CHECK(screen->getFocusedFieldName() == "a0");
}

TEST_CASE("SysEx MIDI output retains its payload until consumed",
          "[sysex][midi-output]")
{
    auto pool = std::make_shared<SysExPool>();
    const auto bytes = message(512);
    audiomidi::MidiOutput output;
    ClientMidiEvent event;
    event.setSysExPayload(SysExPayloadOwner(
        pool->acquire(bytes.data(), bytes.size())));
    event.setBufferOffset(37);
    REQUIRE(output.enqueueEvent(std::move(event)));
    CHECK(pool->used() == 1);
    std::vector<ClientMidiEvent> buffer(1);
    REQUIRE(output.dequeue(buffer) == 1);
    REQUIRE(buffer[0].getSysExPayload());
    CHECK(buffer[0].getBufferOffset() == 37);
    CHECK(buffer[0].getSysExPayload()->byteCount == 512);
    CHECK(std::equal(bytes.begin(), bytes.end(),
                     buffer[0].getSysExPayload()->data()));
    buffer[0].releaseSysExPayload();
    CHECK(pool->used() == 0);
}

TEST_CASE("Pending SysEx insertions are released on rejection and shutdown",
          "[sysex][queue]")
{
    std::shared_ptr<SysExPool> pool;
    {
        Mpc mpc;
        TestMpc::initializeTestMpcWithoutIoServices(mpc);
        pool = mpc.sysExPool;
        const auto sequencer = mpc.getSequencer();
        const auto manager = sequencer->getStateManager();
        sequencer->getSelectedSequence()->init(0);
        manager->drainQueue();
        // Fill the message queue, then reject an insertion with an owned
        // payload.
        for (int i = 0; i < 512; ++i)
        {
            REQUIRE(manager->enqueue(SetUndoSequenceAvailable{false}));
        }
        sequencer->getSelectedTrack()->acquireAndInsertEvent(event(mpc, 512));
        CHECK(pool->used() == 0);
        manager->drainQueue();
        // Leave a second insertion queued when the MPC is destroyed.
        sequencer->getSelectedTrack()->acquireAndInsertEvent(event(mpc, 267));
        CHECK(pool->used() == 1);
    }
    CHECK(pool->used() == 0);
}

TEST_CASE("Deferred SysEx insertions are released on shutdown",
          "[sysex][queue]")
{
    std::shared_ptr<SysExPool> pool;
    {
        Mpc mpc;
        TestMpc::initializeTestMpcWithoutIoServices(mpc);
        pool = mpc.sysExPool;
        const auto sequencer = mpc.getSequencer();
        const auto manager = sequencer->getStateManager();
        sequencer->getSelectedSequence()->init(0);
        manager->drainQueue();
        auto track = sequencer->getSelectedTrack();
        auto &lock = manager->trackLocks[track->getSequenceIndex()]
                                      [track->getIndex()];
        lock.acquire();
        track->acquireAndInsertEvent(event(mpc, 267));
        manager->drainQueue();
        CHECK(pool->used() == 1);
        lock.release();
    }
    CHECK(pool->used() == 0);
}

TEST_CASE("A rejected deferred SysEx insertion releases its payload",
          "[sysex][queue]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto manager = mpc.getSequencer()->getStateManager();
    for (int i = 0; i < 2048; ++i)
    {
        REQUIRE(manager->defer(SetLoopEnabled{SequenceIndex(0), true}));
    }

    auto *handle = manager->acquireEvent();
    REQUIRE(handle);
    const auto bytes = message(267);
    handle->type = EventType::SystemExclusive;
    handle->sysExPayload = mpc.sysExPool->acquire(bytes.data(), bytes.size());
    REQUIRE(handle->sysExPayload);
    handle->sequenceIndex = SequenceIndex(0);
    handle->trackIndex = TrackIndex(0);
    CHECK_FALSE(manager->defer(InsertAcquiredEvent{handle}));
    CHECK(manager->getDeferredOverflowCount() == 1);
    CHECK(mpc.sysExPool->used() == 0);
}

TEST_CASE("SysEx pool supports concurrent acquire and release",
          "[sysex][queue]")
{
    auto pool = std::make_shared<SysExPool>();
    const auto bytes = message(512);
    std::atomic<bool> intact{true};
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i)
    {
        threads.emplace_back(
            [&]
            {
                for (int n = 0; n < 2000; ++n)
                {
                    SysExPayloadOwner owner(
                        pool->acquire(bytes.data(), bytes.size()));
                    if (!owner.get() || std::memcmp(owner.get()->data(),
                                                    bytes.data(), bytes.size()))
                    {
                        intact.store(false);
                    }
                    auto copy = owner;
                    if (!copy.get() || copy.get() != owner.get())
                    {
                        intact.store(false);
                    }
                }
            });
    }
    for (auto &thread : threads)
    {
        thread.join();
    }
    CHECK(intact.load());
    CHECK(pool->used() == 0);
}

TEST_CASE(
    "Full SysEx admission still permits serialization and visible UI snapshots",
    "[sysex]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto sequencer = mpc.getSequencer();
    const auto manager = sequencer->getStateManager();
    const auto sequence = sequencer->getSelectedSequence();
    sequence->init(2);
    manager->drainQueue();
    const auto track = sequence->getTrack(0);
    for (int i = 0; i < 768; ++i)
    {
        track->acquireAndInsertEvent(event(mpc, 512, i));
        if ((i % 64) == 63)
        {
            manager->drainQueue();
        }
    }
    REQUIRE(mpc.sysExPool->persistentUsed() == 768);
    mpc.getLayeredScreen()->openScreenById(lcdgui::ScreenId::StepEditorScreen);
    const auto step = mpc.screens->get<lcdgui::ScreenId::StepEditorScreen>();
    const auto visible = step->computeVisibleEvents();
    REQUIRE(visible[0]);
    CHECK(visible[0]->snapshot.getSysExByteCount() == 512);
    REQUIRE_NOTHROW(file::kaitai::AllIo::save(mpc));
    REQUIRE_NOTHROW(file::kaitai::MidIo::saveBytes(sequence));
    CHECK(mpc.sysExPool->persistentUsed() == 768);
}

TEST_CASE(
    "Step editor copies and pastes full imported SysEx after deleting its "
    "source",
    "[sysex][step-editor]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto sequencer = mpc.getSequencer();
    const auto manager = sequencer->getStateManager();
    sequencer->getSelectedSequence()->init(0);
    manager->drainQueue();
    const auto track = sequencer->getSelectedTrack();
    track->acquireAndInsertEvent(event(mpc, 512));
    manager->drainQueue();
    const auto screen = mpc.getLayeredScreen();
    screen->openScreenById(lcdgui::ScreenId::StepEditorScreen);
    const auto step = mpc.screens->get<lcdgui::ScreenId::StepEditorScreen>();
    REQUIRE(screen->setFocus("a0"));
    step->function(1);
    REQUIRE(step->getPlaceHolder().size() == 1);
    step->function(2);
    manager->drainQueue();
    screen->timerCallback();
    step->function(4);
    mpc.screens->get<lcdgui::ScreenId::PasteEventScreen>()->function(4);
    manager->drainQueue();
    screen->timerCallback();
    const auto states = track->getEventStates();
    REQUIRE(states.size() == 1);
    checkPayload(states[0], message(512));
    REQUIRE(screen->setFocus("a0"));
    for (int i = 0; i < 511; ++i)
    {
        step->right();
    }
    step->turnWheel(-1);
    manager->drainQueue();
    CHECK(track->getEvent(0)->snapshot.getSysExByte(511) == 0xF6);
    CHECK(step->getPlaceHolder()[0].getSysExByte(511) == 0xF7);
}

TEST_CASE("Live MIDI recording and playback keep complete SysEx",
          "[sysex][midi-input][midi-output]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto sequencer = mpc.getSequencer();
    const auto manager = sequencer->getStateManager();
    const auto sequence = sequencer->getSelectedSequence();
    sequence->init(0);
    const auto track = sequence->getTrack(0);
    track->setBusType(BusType::MIDI);
    track->setDeviceIndex(1);
    sequencer->getTransport()->setCountEnabled(false);
    manager->drainQueue();
    sequencer->getTransport()->overdubImmediately();
    const auto bytes = message(267);
    ClientMidiEvent input;
    input.setSysExPayload(SysExPayloadOwner(mpc.sysExPool->acquire(
        bytes.data(), bytes.size())));
    mpc.clientEventController->getClientMidiEventController()
        ->handleClientMidiEvent(input);
    manager->drainQueue();
    auto states = track->getEventStates();
    REQUIRE(states.size() == 1);
    checkPayload(states.front(), bytes);
    mpc.getEventHandler()->handleFinalizedEvent(states.front(), TrackIndex(0),
                                                100, BusType::MIDI, 1);
    std::vector<ClientMidiEvent> output(8);
    const auto count = mpc.getMidiOutput()->dequeue(output);
    REQUIRE(count == 1);
    REQUIRE(output[0].getSysExPayload());
    CHECK(output[0].getSysExPayload()->byteCount == bytes.size());
    CHECK(output[0].getBufferOffset() == mpc.getEngineHost()
                                             ->getSequencerPlaybackEngine()
                                             ->getEventFrameOffset());
    CHECK(std::equal(bytes.begin(), bytes.end(),
                     output[0].getSysExPayload()->data()));
    sequencer->getTransport()->stop();
    manager->drainQueue();
}
