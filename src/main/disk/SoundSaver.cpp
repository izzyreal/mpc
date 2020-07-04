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

SoundSaver::SoundSaver(vector<weak_ptr<mpc::sampler::Sound>> sounds, bool wav) 
{	
	disk = Mpc::instance().getDisk();
	disk.lock()->setBusy(true);
	this->sounds = sounds;
	this->wav = wav;
	saveSoundsThread = thread(&SoundSaver::static_saveSounds, this);
}

void SoundSaver::static_saveSounds(void* this_p)
{
	static_cast<SoundSaver*>(this_p)->saveSounds();
}

void SoundSaver::saveSounds()
{
	string const ext = string(wav ? ".WAV" : ".SND");
	auto lDisk = disk.lock();
	
	for (auto s : sounds)
	{
		string fileName = StrUtil::replaceAll(s.lock()->getName(), ' ', "");
		
		Mpc::instance().getLayeredScreen().lock()->openScreen("popup");
		auto popupScreen = dynamic_pointer_cast<PopupScreen>(Screens::getScreenComponent("popup"));
		popupScreen->setText("SAVING " + StrUtil::padRight(fileName, " ", 16) + ext);

		if (lDisk->checkExists(fileName))
		{
			auto saveAProgramScreen = dynamic_pointer_cast<SaveAProgramScreen>(Screens::getScreenComponent("save-a-program"));
			if (saveAProgramScreen->replaceSameSounds)
			{
                lDisk->getFile(fileName)->del(); // possibly prepend auto success =
			}
			else
			{
				continue;
			}
		}
		
		if (!wav)
		{
			lDisk->writeSound(s);
		}
		else
		{
			lDisk->writeWav(s);
		}
		try
		{
			this_thread::sleep_for(chrono::milliseconds(300));
		}
		catch (exception e)
		{
			e.what();
		}
	}

	Mpc::instance().getLayeredScreen().lock()->openScreen("save");
	lDisk->setBusy(false);
}

SoundSaver::~SoundSaver()
{
	if (saveSoundsThread.joinable())
	{
		saveSoundsThread.join();
	}
}
