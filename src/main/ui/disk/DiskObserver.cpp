#include <ui/disk/DiskObserver.hpp>

#include <Mpc.hpp>
#include <Paths.hpp>
#include <Util.hpp>
#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <ui/NameGui.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>
#include <ui/disk/DiskGui.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/Track.hpp>
#include <mpc/MpcSoundPlayerChannel.hpp>

#include <lang/StrUtil.hpp>
#include <file/FileUtil.hpp>
#include <file/File.hpp>

using namespace mpc::ui::disk;
using namespace moduru::lang;
using namespace std;

DiskObserver::DiskObserver()
{	
	if (csn.compare("cantfindfile") == 0) {
		fileField.lock()->setText(*diskGui->getCannotFindFileName());
	}
}
