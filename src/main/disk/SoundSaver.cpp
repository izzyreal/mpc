#include <disk/SoundSaver.hpp>

#include <Mpc.hpp>
#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>
#include <sampler/Sound.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/window/SaveAProgramScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>

#include <lang/StrUtil.hpp>

#include <thread>

using namespace mpc::disk;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;
using namespace moduru::lang;
using namespace std;

SoundSaver::SoundSaver(mpc::Mpc& _mpc, vector<weak_ptr<mpc::sampler::Sound>> _sounds, bool _wav)
: mpc (_mpc), sounds (_sounds), wav (_wav), saveSoundsThread (std::thread(&SoundSaver::static_saveSounds, this))
{
}

void SoundSaver::static_saveSounds(void* this_p)
{
	static_cast<SoundSaver*>(this_p)->saveSounds();
}

void SoundSaver::saveSounds()
{
	string const ext = string(wav ? ".WAV" : ".SND");
    auto disk = mpc.getDisk().lock();
	
	for (auto s : sounds)
	{
		string fileName = StrUtil::replaceAll(s.lock()->getName(), ' ', "");
		
		mpc.getLayeredScreen().lock()->openScreen("popup");
		auto popupScreen = mpc.screens->get<PopupScreen>("popup");
		popupScreen->setText("Saving " + StrUtil::padRight(fileName, " ", 16) + ext);

		if (disk->checkExists(fileName + ext))
		{
			auto saveAProgramScreen = mpc.screens->get<SaveAProgramScreen>("save-a-program");

			if (saveAProgramScreen->replaceSameSounds)
                disk->getFile(fileName + ext)->del(); // possibly prepend auto success =
			else
				continue;
		}
		
		if (wav)
            disk->writeWav(s.lock(), "");
		else
            disk->writeSound(s.lock(), "");

        this_thread::sleep_for(chrono::milliseconds(300));
	}

	mpc.getLayeredScreen().lock()->openScreen("save");
}

SoundSaver::~SoundSaver()
{
	if (saveSoundsThread.joinable())
		saveSoundsThread.join();
}
