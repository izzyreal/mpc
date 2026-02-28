#include "TestPaths.hpp"

mpc_fs::path mpc::TestPaths::appConfigHome() const
{
    return appDocumentsPath() / "config";
}

mpc_fs::path mpc::TestPaths::appDocumentsPath() const
{
    return mpc_fs::temp_directory_path() / "VMPC2000XL-test";
}