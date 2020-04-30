#include "DataWheel.hpp"

#include <Mpc.hpp>
#include <controls/BaseControls.hpp>
#include <disk/AbstractDisk.hpp>

using namespace mpc::hardware;
using namespace std;

DataWheel::DataWheel()
{
	
}

void DataWheel::turn(int increment) {
	if (!Mpc::instance().getDisk().lock()->isBusy()) {
		Mpc::instance().getActiveControls()->turnWheel(increment);
	}
	notifyObservers(increment);
}

DataWheel::~DataWheel() {
}
