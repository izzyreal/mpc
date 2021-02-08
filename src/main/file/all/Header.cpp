#include <file/all/Header.hpp>

#include <VecUtil.hpp>

#include <string>

using namespace mpc::file::all;
using namespace std;

Header::Header(vector<char> b) 
{
	loadBytes = b;
}

Header::Header()
{
	string s = "MPC2KXL ALL 1.00";
	saveBytes = vector<char>(16);
	for (int i = 0; i < 16; i++)
		saveBytes[i] = s[i];
}

vector<char> Header::getHeaderArray()
{
    return loadBytes;
}

bool Header::verifyFileID()
{
	auto verifyFileID = false;
	auto checkFileID = moduru::VecUtil::CopyOfRange(loadBytes, 0, 16);
	string fileIDString = "";
	for (char c : checkFileID) {
		if (c == 0x00) break;
		fileIDString.push_back(c);
	}
	string shouldBe = "MPC2KXL ALL 1.00";
	if (fileIDString.compare(shouldBe) == 0) {
		verifyFileID = true;
	}
	return verifyFileID;
}

vector<char> Header::getBytes()
{
    return saveBytes;
}
