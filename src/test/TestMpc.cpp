#include "TestMpc.hpp"

#include "TestPaths.hpp"

#include <memory>

void mpc::TestMpc::initializeTestMpc(mpc::Mpc& mpc)
{
    mpc.paths = std::make_shared<mpc::TestPaths>();
    auto testDocPath = mpc.paths->appDocumentsPath();
    fs::remove_all(testDocPath);
    fs::create_directories(testDocPath);
}