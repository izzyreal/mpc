#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <Mpc.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc;

TEST_CASE("Mpc is instantiated and booted", "[mpc-boot]") {
	Mpc mpc;
	mpc.init(44100, 1, 1);
	REQUIRE(mpc.getSequencer().lock());
}
