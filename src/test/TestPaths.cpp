#include "TestPaths.hpp"

fs::path mpc::TestPaths::appConfigHome() const
{
    return appDocumentsPath() / "config";
}

fs::path mpc::TestPaths::appDocumentsPath() const
{
    return fs::temp_directory_path() / "VMPC2000XL-test";
}