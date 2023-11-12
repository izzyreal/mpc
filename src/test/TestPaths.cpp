#include "TestPaths.hpp"

fs::path mpc::TestPaths::appDocumentsPath()
{
    auto tmpDocsPath = fs::temp_directory_path();
    tmpDocsPath = tmpDocsPath / "VMPC2000XL-test";
    return tmpDocsPath;
}