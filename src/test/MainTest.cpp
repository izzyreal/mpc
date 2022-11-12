#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>

#include <Mpc.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/Sequence.hpp>

#include <string>

using namespace mpc;
using namespace std;

TEST_CASE("Mpc is instantiated and booted", "[mpc-boot]")
{
	Mpc mpc;
	mpc.init(1, 1);
	REQUIRE(mpc.getSequencer());
}

SCENARIO("A Sequencer initializes correctly", "[sequencer]") {

	GIVEN("A Sequencer") {
		Mpc mpc;
		mpc.init(1, 1);
		auto seq = mpc.getSequencer();
		seq->init();

		REQUIRE( seq->getTempo() == 120 );
	}
}

SCENARIO("A Sequence initializes correctly", "[sequence]") {

	GIVEN("An initialized Sequence") {
		Mpc mpc;
		mpc.init(1, 1);
		mpc::sequencer::Sequence seq(mpc);
		seq.init(1);
	    REQUIRE( seq.getInitialTempo() == 120.0 );
	}
}
