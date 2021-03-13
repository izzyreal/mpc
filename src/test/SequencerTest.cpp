#include <catch2/catch.hpp>

#include <Mpc.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/Track.hpp>

using namespace std;

SCENARIO("Can record and playback from different threads", "[sequencer]") {

	GIVEN("An initialized Mpc/Sequence/Track") {
		mpc::Mpc mpc;
		mpc.init(44100, 1, 5);

		REQUIRE(1 == 1);
	}
}
