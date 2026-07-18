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
    MpcInitOptions options;
    options.detectRawUsbVolumes = false;
    options.installDemoFiles = false;
    options.fileOperationTimings =
        FileOperationTimings::uniform(std::chrono::milliseconds(10));
    mpc.init(options);
}

void mpc::TestMpc::initializeTestMpcWithoutMidiServices(Mpc &mpc)
{
    resetTestDataRoot(mpc);
    MpcInitOptions options;
    options.startMidiDeviceDetector = false;
    options.startAudioServer = true;
    options.detectRawUsbVolumes = false;
    options.installDemoFiles = false;
    options.fileOperationTimings =
        FileOperationTimings::uniform(std::chrono::milliseconds(10));
    mpc.init(options);
}

void mpc::TestMpc::initializeTestMpcWithoutIoServices(Mpc &mpc)
{
    resetTestDataRoot(mpc);
    MpcInitOptions options;
    options.startMidiDeviceDetector = false;
    options.startAudioServer = false;
    options.detectRawUsbVolumes = false;
    options.installDemoFiles = false;
    options.fileOperationTimings =
        FileOperationTimings::uniform(std::chrono::milliseconds(10));
    mpc.init(options);
}
