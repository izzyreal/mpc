#include "TestMpc.hpp"

#include "TestPaths.hpp"
#include "engine/EngineHost.hpp"
#include "engine/audio/server/NonRealTimeAudioServer.hpp"
#include "sequencer/Sequencer.hpp"

#include <memory>

void mpc::TestMpc::resetTestDataRoot(Mpc &mpc)
{
    mpc.paths = std::make_shared<TestPaths>();
    auto testDocPath = mpc.paths->getDocuments()->appDocumentsPath();
    (void) mpc_fs::remove_all(testDocPath);
    (void) mpc_fs::create_directories(testDocPath);
}

void mpc::TestMpc::initializeTestMpc(Mpc &mpc)
{
    resetTestDataRoot(mpc);
    mpc.init();
    mpc.getEngineHost()->getAudioServer()->start();
}
