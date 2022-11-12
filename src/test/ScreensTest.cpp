#include <catch2/catch_test_macros.hpp>

#include <Mpc.hpp>

#include <sequencer/Sequencer.hpp>
#include <sampler/Sampler.hpp>

#include <lcdgui/Layer.hpp>

#include <vector>
#include <string>

using namespace std;

vector<string> screenNames = {
"black",
"mpc2000xl",
"sequencer",
"next-seq",
"mixer",
"step-editor",
"step-edit-options",
"load",
"save",
"select-drum",
"select-mixer-drum",
"mixer-setup",
"program-assign",
"program-params",
"events",
"bars",
"tr-move",
"user",
"drum",
"purge",
"fx-edit",
"others",
"init",
"ver",
"sync",
"midi-sw",
"sample",
"trim",
"loop",
"zone",
"params",
"song",
"punch",
"trans",
"second-seq",
"assign",
"track-mute",
"next-seq-pad",
"vmpc-settings",
"vmpc-auto-save",
"vmpc-keyboard",
"vmpc-reset-keyboard",
"vmpc-disks",
"vmpc-direct-to-disk-recorder",
"vmpc-recording-finished",
"vmpc-record-jam",
"vmpc-file-in-use",
"vmpc-midi",
"vmpc-midi-presets",
"vmpc-discard-mapping-changes",
"vmpc-warning-settings-ignored",
"mpc2000xl-all-file",
"copy-note-parameters",
"assignment-view",
"init-pad-assign",
"directory",
"load-a-program",
"save-all-file",
"save-aps-file",
"save-a-program",
"load-aps-file",
"load-a-sequence",
"load-a-sequence-from-all",
"save-a-sequence",
"edit-multiple",
"paste-event",
"step-timing-correct",
"insert-event",
"load-a-sound",
"save-a-sound",
"keep-or-retry",
"channel-settings",
"program",
"sound",
"velocity-modulation",
"velo-env-filter",
"velo-pitch",
"mute-assign",
"auto-chromatic-assignment",
"sound-memory",
"start-fine",
"end-fine",
"loop-to-fine",
"loop-end-fine",
"zone-start-fine",
"zone-end-fine",
"edit-sound",
"song-window",
"ignore-tempo-change",
"loop-song",
"loop-bars-window",
"time-display",
"transpose-permanent",
"erase",
"tempo-change",
"timing-correct",
"change-tsig",
"change-bars",
"change-bars-2",
"count-metronome",
"erase-all-off-tracks",
"transmit-program-changes",
"multi-recording-setup",
"midi-input",
"midi-input-monitor",
"midi-output",
"midi-output-monitor",
"edit-velocity",
"assign-16-levels",
"sequence",
"track",
"number-of-zones",
"cant-find-file",
"name",
"file-exists",
"delete-all-sequences",
"delete-all-tracks",
"delete-program",
"delete-folder",
"delete-file",
"create-new-program",
"copy-program",
"copy-sound",
"delete-all-programs",
"delete-sound",
"delete-all-sound",
"convert-sound",
"mono-to-stereo",
"stereo-to-mono",
"resample",
"delete-song",
"metronome-sound",
"delete-track",
"copy-track",
"copy-sequence",
"delete-sequence",
"delete-all-files",
"popup",
"vmpc-convert-and-load-wav"
};

SCENARIO("All screens can be opened", "[gui]") {

	GIVEN("An initialized Mpc with an initialized Sequence") {
		mpc::Mpc mpc;
		mpc.init(1, 5);
        
        
        auto s = mpc.getSampler()->addSound().lock();
        s->setMono(true);
        s->setName("test");
        for (int i = 0; i < 1000; i ++)
            s->insertFrame(std::vector<float>{0.0}, s->getSampleData()->size());
		
        mpc.getSampler()->setSoundIndex(0);

		auto ls = mpc.getLayeredScreen();

		vector<string> good;
		vector<string> bad;

		for (auto& screenName : screenNames)
		{
			int layerIndex = ls->openScreen(screenName);
			
			// We do a check for the most important screen
			if (screenName.compare("sequencer") == 0)
				REQUIRE(layerIndex == 0);

			if (layerIndex == -1)
			{
				bad.push_back(screenName + " could not be opened.");
				continue;
			}
            
			auto layer = ls->getFocusedLayer();
			vector<vector<bool>> pixels(248, vector<bool>(60));
			layer->Draw(&pixels);
			int blackPixelCount = 0;

			for (auto& column : pixels)
			{
				for (auto b : column)
				{
					if (b)
						blackPixelCount++;
				}
			}

			// And another check for the most important screen
			if (screenName.compare("sequencer") == 0)
				REQUIRE(blackPixelCount > 0);

			if (blackPixelCount > 0)
				good.push_back(screenName + " has " + to_string(blackPixelCount) + " black pixels");
			else
				bad.push_back(screenName + " is openable, but has 0 black pixels");
		}

		MLOG("These screens are fine:");
		for (auto& msg : good)
			MLOG(msg);

		MLOG("\nThese screens are broken:");
		for (auto& msg : bad)
			MLOG(msg);

		printf("%i screens are fine and %i screens are broken. Check ~/Documents/VMPC2000XL/vmpc.log for more details.\n", good.size(), bad.size());

		REQUIRE(good.size() >= 129); // This will be increased as the screens get refactored.

	}
}
