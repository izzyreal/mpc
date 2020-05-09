#include "SequencerScreen.hpp"

#include <Mpc.hpp>
#include <sequencer/Sequencer.hpp>
#include <lcdgui/Parameter.hpp>

#include <lcdgui/Label.hpp>
#include <lcdgui/Field.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

SequencerScreen::SequencerScreen(std::vector<std::shared_ptr<Component>> components)
	: Component("sequencer-screen")
{
	for (auto component : components)
	{
		addChild(move(component));
	}
	displaySq();
}

void SequencerScreen::displaySq()
{
	auto param = dynamic_pointer_cast<Parameter>(findChild("sq").lock());

	if (!param)
	{
		return;
	}

	param->getField().lock()->setText("Sequence-01");

	string result{ "" };
	auto lSequencer = mpc::Mpc::instance().getSequencer().lock();
	if (lSequencer->isPlaying()) {
		result.append(moduru::lang::StrUtil::padLeft(to_string(lSequencer->getCurrentlyPlayingSequenceIndex() + 1), "0", 2));
		result.append("-");
		result.append(lSequencer->getCurrentlyPlayingSequence().lock()->getName());
		param->getLabel().lock()->setText(result);
	}
	else {
		result.append(moduru::lang::StrUtil::padLeft(to_string(lSequencer->getActiveSequenceIndex() + 1), "0", 2));
		result.append("-");
		result.append(lSequencer->getActiveSequence().lock()->getName());
		param->getField().lock()->setText(result);
	}
}
