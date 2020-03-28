#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <Mpc.hpp>
#include <sequencer/Sequencer.hpp>

#include <thirdp/bcmath/bcmath_stl.h>

#include <string>

using namespace mpc;
using namespace std;

TEST_CASE("Mpc is instantiated and booted", "[mpc-boot]")
{
	Mpc mpc;
	mpc.init(44100, 1, 1);
	REQUIRE(mpc.getSequencer().lock());
}

TEST_CASE("BCMath functions as expected", "[bcmath]")
{
	auto str = to_string(BCMath("120.0").toDouble());
	printf("BCMath(\"120.0\").toDouble() as string: %s\n", str.c_str());
	if (str.find(".") == string::npos) str += ".0";
	printf("After appending .0 if there was no period: %s\n", str.c_str());
	auto length = (int)(str.find(".")) + 2;
	printf("Reported length of tempo string, should be 4 or 5: %d\n", length);
	auto tempo = BCMath(str.substr(0, length));
	printf("New BCMath tempo as double: %f", tempo.toDouble()); 
        if (tempo.toDouble() < 30.0) tempo = BCMath("30.0");
	if (tempo.toDouble() > 300.0) tempo = BCMath("300.0");

}

