#include <catch2/catch_test_macros.hpp>

#include "Mpc.hpp"
#include "sequencer/Track.hpp"

#include "lcdgui/screens/EventsScreen.hpp"

#include "mpc/MpcSoundPlayerChannel.hpp"

using namespace mpc::sequencer;
using namespace mpc::lcdgui::screens;

TEST_CASE("COPY1", "[events-screen]")
{
    mpc::Mpc mpc;
    mpc.init(44100, 1, 5);
    auto seq = mpc.getSequencer().lock()->getActiveSequence();
    seq->init(0);
    auto tr = seq->getTrack(0);

    for (int i = 0; i < 192; i += 24)
    {
        auto noteEvent = std::make_shared<NoteEvent>(35);
        noteEvent->setVelocity(127);
        noteEvent->setDuration(42);
        noteEvent->setTick(i);
        tr->insertEventWhileRetainingSort(noteEvent);
    }

    auto eventsScreen = mpc.screens->get<EventsScreen>("events");

    int start = 0;
    int end = 192;
    int toSeq = 0;
    int destStart = 192;
    int toTrack = 0;
    bool merge = true;
    int copies = 1;
    int note0 = 34;
    int note1 = 0;

    eventsScreen->performCopy(start, end, toSeq, destStart, toTrack, merge, copies, note0, note1);

    REQUIRE(tr->getEvents().size() == 16);
    int eventCounter = 8;
    for (int i = 192; i < 384; i += 24)
    {
        REQUIRE(tr->getEvent(eventCounter++)->getTick() == i);
    }
}

TEST_CASE("COPY2", "[events-screen]")
{
    mpc::Mpc mpc;
    mpc.init(44100, 1, 5);
    auto seq = mpc.getSequencer().lock()->getActiveSequence();
    seq->init(0);
    auto tr = seq->getTrack(0);

    for (int i = 23; i < 192; i += 24)
    {
        auto noteEvent = std::make_shared<NoteEvent>(35);
        noteEvent->setVelocity(127);
        noteEvent->setDuration(42);
        noteEvent->setTick(i);
        tr->insertEventWhileRetainingSort(noteEvent);
    }

    auto eventsScreen = mpc.screens->get<EventsScreen>("events");

    int start = 0;
    int end = 192;
    int toSeq = 0;
    int destStart = 192;
    int toTrack = 0;
    bool merge = true;
    int copies = 1;
    int note0 = 34;
    int note1 = 0;

    eventsScreen->performCopy(start, end, toSeq, destStart, toTrack, merge, copies, note0, note1);

    REQUIRE(tr->getEvents().size() == 16);
    int eventCounter = 8;
    for (int i = 215; i < 384; i += 24)
    {
        REQUIRE(tr->getEvent(eventCounter++)->getTick() == i);
    }
}


TEST_CASE("COPY3", "[events-screen]")
{
    mpc::Mpc mpc;
    mpc.init(44100, 1, 5);
    auto seq = mpc.getSequencer().lock()->getActiveSequence();
    seq->init(0);
    auto tr = seq->getTrack(0);

    for (int i = 23; i < 192; i += 24)
    {
        auto noteEvent = std::make_shared<NoteEvent>(35);
        noteEvent->setVelocity(127);
        noteEvent->setDuration(42);
        noteEvent->setTick(i);
        tr->insertEventWhileRetainingSort(noteEvent);
    }

    auto eventsScreen = mpc.screens->get<EventsScreen>("events");

    int start = 0;
    int end = 192;
    int toSeq = 0;
    int destStart = 193;
    int toTrack = 0;
    bool merge = true;
    int copies = 1;
    int note0 = 34;
    int note1 = 0;

    eventsScreen->performCopy(start, end, toSeq, destStart, toTrack, merge, copies, note0, note1);

    REQUIRE(tr->getEvents().size() == 15);
    int eventCounter = 8;
    for (int i = 216; i < 384; i += 24)
    {
        REQUIRE(tr->getEvent(eventCounter++)->getTick() == i);
    }
}