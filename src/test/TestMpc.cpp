#include "TestMpc.hpp"

#include "TestPaths.hpp"

#include <memory>

void mpc::TestMpc::initializeTestMpc(Mpc &mpc)
{
    mpc.paths = std::make_shared<TestPaths>();
    auto testDocPath = mpc.paths->appDocumentsPath();
    fs::remove_all(testDocPath);
    fs::create_directories(testDocPath);
    mpc.init();
}