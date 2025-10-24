#include "TestPaths.hpp"

fs::path mpc::TestPaths::appConfigHome()
{
    return appDocumentsPath() / "config";
}

fs::path mpc::TestPaths::appDocumentsPath()
{
    return fs::temp_directory_path() / "VMPC2000XL-test";
    ;
}