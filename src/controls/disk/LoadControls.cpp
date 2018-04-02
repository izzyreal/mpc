#include <controls/disk/LoadControls.hpp>

#include <Mpc.hpp>
#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>
#include <file/mid/MidiReader.hpp>
#include <ui/disk/DiskGui.hpp>
#include <ui/disk/window/DirectoryGui.hpp>
#include <ui/disk/window/DiskWindowGui.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>

#include <file/FileUtil.hpp>
#include <lang/StrUtil.hpp>

using namespace moduru::lang;
using namespace mpc::controls::disk;
using namespace std;

LoadControls::LoadControls(mpc::Mpc* mpc)
	: AbstractDiskControls(mpc)
{
}

void LoadControls::function(int i)
{
	init();
	auto lDisk = disk.lock();
	string ext;
	switch (i) {
	case 1:
		ls.lock()->openScreen("save");
		break;
	case 2:
		ls.lock()->openScreen("format");
		break;
	case 3:
		ls.lock()->openScreen("setup");
		break;
	case 5:
		if (!lDisk) return;
		if (lDisk->getFileNames().size() == 0) return;

		selectedFile = diskGui->getSelectedFile();
		ext = moduru::file::FileUtil::splitName(selectedFile->getName())[1];
		if (StrUtil::eqIgnoreCase(ext, "snd") || StrUtil::eqIgnoreCase(ext, "wav")) {
			mpc->loadSound(false);
			return;
		}
		else if (StrUtil::eqIgnoreCase(ext, "pgm")) {
			ls.lock()->openScreen("loadaprogram");
			return;
		}
		else if (StrUtil::eqIgnoreCase(ext, "mid")) {
			auto lSequencer = sequencer.lock();
			auto newSeq = lSequencer->createSeqInPlaceHolder().lock();
			newSeq->init(2);
			auto mr = mpc::file::mid::MidiReader(mpc, selectedFile, newSeq);
			mr.parseSequence();
			ls.lock()->openScreen("loadasequence");
			auto usedSeqs = lSequencer->getUsedSequenceIndexes();
			int index;
			for (index = 0; index < 99; index++) {
				bool contains = false;
				for (int i : usedSeqs) {
					if (i == index) {
						contains = true;
						break;
					}
				}
				if (!contains) break;
			}
			diskWindowGui->setLoadInto(index);
		}
		else if (StrUtil::eqIgnoreCase(ext, "all")) {
			ls.lock()->openScreen("mpc2000xlallfile");
			return;
		}
		else if (StrUtil::eqIgnoreCase(ext, "aps")) {
			ls.lock()->openScreen("loadapsfile");
			return;
		}

		if (diskGui->isSelectedFileDirectory()) {
			if (lDisk->moveForward(diskGui->getSelectedFile()->getName())) {
				diskGui->setView(diskGui->getView());
			}
			return;
		}
		break;
	}
}

void LoadControls::openWindow()
{
	init();
	auto lDisk = disk.lock();
	if (param.compare("directory") == 0 || param.compare("file") == 0) {
		if (!lDisk) return;
		directoryGui->setPreviousScreenName("load");
		directoryGui->findYOffset0();
		directoryGui->setYOffset1(diskGui->getFileLoad());
		ls.lock()->openScreen("directory");
		return;
	}
}

void LoadControls::turnWheel(int i)
{
	init();
	if (param.compare("view") == 0) {
		diskGui->setView(diskGui->getView() + i);
	}
	else if (param.compare("file") == 0) {
		diskGui->setSelectedFileNumberLimited(diskGui->getFileLoad() + i);
	}
}
