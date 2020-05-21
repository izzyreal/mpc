#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <Mpc.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/Sequence.hpp>
#include <ui/UserDefaults.hpp>

#include <thirdp/bcmath/bcmath_stl.h>

#include <string>

using namespace mpc;
using namespace std;

TEST_CASE("Mpc is instantiated and booted", "[mpc-boot]")
{
	auto& mpc = mpc::Mpc::instance();
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
	
	REQUIRE( (length == 4 || length == 5) );

	auto tempo = BCMath(str.substr(0, length));
        
	REQUIRE( tempo.toDouble() == 120.0 );
}

SCENARIO("A Sequencer initializes correctly", "[sequencer]") {

	GIVEN("A Sequencer") {
		auto& mpc = mpc::Mpc::instance();
		mpc.init(44100, 1, 1);
		auto seq = mpc.getSequencer().lock();
		seq->init();

		REQUIRE( seq->getTempo().toDouble() == 120 );
	}
}

SCENARIO("A Sequence initializes correctly", "[sequence]") {

	GIVEN("An initialized Sequence") {
		auto& mpc = mpc::Mpc::instance();
		mpc.init(44100, 1, 1);
		std::vector<string> trackNames;
		
		auto& defaults = mpc::ui::UserDefaults::instance();
		
		for (int i = 0; i < 64; i++)
		{
		       trackNames.push_back(defaults.getTrackName(i));
		}	       
		
		mpc::sequencer::Sequence seq(trackNames);
		seq.init(1);
	    REQUIRE( seq.getInitialTempo().toDouble() == 120.0 );
	}
}

SCENARIO("UserDefaults have sane values", "[userdefaults]") {

	GIVEN("Some UserDefaults") {
		auto& defaults = mpc::ui::UserDefaults::instance();
		BCMath tempo = defaults.getTempo();
		REQUIRE( tempo.toDouble() == 120.0 );
	}
}
