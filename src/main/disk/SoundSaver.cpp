#include <disk/SoundSaver.hpp>

#include <Mpc.hpp>
#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>
#include <ui/disk/DiskGui.hpp>
#include <sampler/Sound.hpp>

#include <lang/StrUtil.hpp>

#include <thread>

using namespace mpc::disk;
using namespace moduru::lang;
using namespace std;

SoundSaver::SoundSaver(mpc::Mpc* mpc, vector<weak_ptr<mpc::sampler::Sound> > sounds, bool wav) 
{
	this->mpc = mpc;
	disk = mpc->getDisk();
	diskGui = mpc->getUis().lock()->getDiskGui();
	disk.lock()->setBusy(true);
	this->sounds = sounds;
	this->wav = wav;
	if (saveSoundsThread.joinable()) saveSoundsThread.join();
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
	for (auto s : sounds) {
		auto skip = false;
		string fileName = StrUtil::replaceAll(s.lock()->getName(), ' ', "") + ext;
		mpc->getLayeredScreen().lock()->createPopup("Writing " + StrUtil::toUpper(fileName), 85);
		if (lDisk->checkExists(fileName)) {
			if (diskGui->getSaveReplaceSameSounds()) {
                lDisk->getFile(fileName)->del(); // possibly prepend auto success =
			}
			else {
				skip = true;
			}
		}
		if (skip) continue;

		if (!wav) {
			lDisk->writeSound(s);
		}
		else {
			lDisk->writeWav(s);
		}
		try {
			this_thread::sleep_for(chrono::milliseconds(500));
		}
		catch (exception e) {
			e.what();
		}
	}
    mpc->getLayeredScreen().lock()->removePopup();
	mpc->getLayeredScreen().lock()->openScreen("save");
	lDisk->setBusy(false);
}

SoundSaver::~SoundSaver() {
	if (saveSoundsThread.joinable()) saveSoundsThread.join();
}
