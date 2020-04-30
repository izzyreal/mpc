#include <ui/sampler/PurgeObserver.hpp>

#include <Mpc.hpp>
#include <Util.hpp>
#include <sampler/Sampler.hpp>
#include <lcdgui/Label.hpp>

using namespace mpc::ui::sampler;
using namespace std;

PurgeObserver::PurgeObserver() 
{
	
	sampler = Mpc::instance().getSampler();
	valueLabel = Mpc::instance().getLayeredScreen().lock()->lookupLabel("value");
	displayValue();
}

void PurgeObserver::displayValue()
{
	valueLabel.lock()->setTextPadded(sampler.lock()->getUnusedSampleAmount(), " ");
}

void PurgeObserver::update(moduru::observer::Observable* o, nonstd::any arg)
{
}

PurgeObserver::~PurgeObserver() {
}
