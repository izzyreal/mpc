#include "sequencer/TrackEventStateWorker.hpp"

#include "Sequence.hpp"
#include "Track.hpp"
#include "TrackEventStateManager.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::sequencer;

TrackEventStateWorker::TrackEventStateWorker(Sequencer *sequencer)
    : running(false), sequencer(sequencer)
{
}

TrackEventStateWorker::~TrackEventStateWorker()
{
    stop();

    if (workerThread.joinable())
    {
        workerThread.join();
    }
}

void TrackEventStateWorker::start()
{
    if (running.load())
    {
        return;
    }

    running.store(true);

    if (workerThread.joinable())
    {
        workerThread.join();
    }

    workerThread = std::thread(
        [&]
        {
            while (running.load())
            {
                work();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
}

void TrackEventStateWorker::stop()
{
    if (!running.load())
    {
        return;
    }

    running.store(false);
}

void TrackEventStateWorker::work() const
{
    for (int i = 0; i < Mpc2000XlSpecs::SEQUENCE_COUNT; i++)
    {
        if (!sequencer->getSequence(i)->isUsed()) continue;
        for (const auto &t : sequencer->getSequence(i)->getTracks())
        {
            t->getEventStateManager()->drainQueue();
        }
    }

    if (sequencer->getPlaceHolder())
    {
        for (const auto &t : sequencer->getPlaceHolder()->getTracks())
        {
            t->getEventStateManager()->drainQueue();
        }
    }
}
