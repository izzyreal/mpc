#include "TestMpc.hpp"

#include "TestPaths.hpp"
#include "engine/EngineHost.hpp"
#include "engine/audio/server/NonRealTimeAudioServer.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/NonRtSequencerStateWorker.hpp"

#include <memory>

void mpc::TestMpc::initializeTestMpc(Mpc &mpc)
{
    mpc.paths = std::make_shared<TestPaths>();
    const auto testDocPath = mpc.paths->appDocumentsPath();
    fs::remove_all(testDocPath);
    fs::create_directories(testDocPath);
    mpc.init();
    mpc.getSequencer()->getNonRtSequencerStateWorker()->stopAndWaitUntilStopped();
    mpc.getEngineHost()->getAudioServer()->start();
}
