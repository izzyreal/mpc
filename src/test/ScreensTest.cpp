#include <catch2/catch.hpp>

#include <Mpc.hpp>
#include <Paths.hpp>

#include <sequencer/Sequencer.hpp>
#include <sampler/Sampler.hpp>

#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/Layer.hpp>

#include <file/File.hpp>

#include <disk/MpcFile.hpp>
#include <disk/MpcFile.hpp>
#include <disk/ApsLoader.hpp>
#include <disk/AbstractDisk.hpp>

#include <vector>
#include <string>

using namespace std;

vector<string> screenNames = {
	"trmove",
"black",
"mpc2000xl",
"sequencer",
"nextseq",
"mixer",
"step",
"load",
"save",
"select-drum",
"select-mixer-drum",
"mixer-setup",
"program-assign",
"programparams",
"format",
"setup",
"edit",
"barcopy",
"trmove",
"user",
"drum",
"purge",
"fx-edit",
"others",
"init",
"ver",
"sync",
"dump",
"midisw",
"sample",
"trim",
"loop",
"zone",
"params",
"song",
"punch",
"trans",
"2ndseq",
"assign",
"trackmute",
"nextseqpad",
"audio",
"audio_no_start",
"midi",
"disk",
"settings",
"directtodiskrecorder",
"recordingfinished",
"buffersize",
"audiomididisabled",
"mpc2000xlallfile",
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
"editmultiple",
"pasteevent",
"step-timing-correct",
"insertevent",
"load-a-sound",
"save-a-sound",
"keep-or-retry",
"formatdisk",
"channel-settings",
"program",
"sound",
"velocity-modulation",
"velo-env-filter",
"velo-pitch",
"mute-assign",
"auto-chromatic-assignment",
"copyeffectsettings",
"distortionringmod",
"4bandfilter",
"effectmixer",
"modulation",
"modulation_rotary",
"modulation_fmodautopan",
"modulation_pitchshift",
"modulation_pitchfeedback",
"delayecho",
"delayecho_stereo",
"reverb",
"reverb_gated",
"soundmemory",
"start-fine",
"end-fine",
"loop-to-fine",
"loop-end-fine",
"zone-start-fine",
"zone-end-fine",
"edit-sound",
"edittimestretch",
"songname",
"ignoretempochange",
"loopsongwindow",
"loopbarswindow",
"convertsongtoseq",
"timedisplay",
"transposepermanent",
"locate",
"erase",
"tempochange",
"timingcorrect",
"changetsig",
"changebars",
"changebars2",
"countmetronome",
"eraseallofftracks",
"transmitprogramchanges",
"multirecordingsetup",
"midiinput",
"midiinputmonitor",
"midioutput",
"midioutputmonitor",
"editvelocity",
"assign16levels",
"sequence",
"track",
"number-of-zones",
"cant-find-file",
"name",
"file-already-exists",
"recordjam",
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
"convertnodoit",
"resample",
"bpmmatch",
"deletesong",
"metronome-sound",
"delete-track",
"copy-track",
"copy-sequence",
"delete-sequence",
"midi-monitor",
"delete-all-files"
};

SCENARIO("All screens can be opened", "[gui]") {

	GIVEN("An initialized Mpc with an initialized Sequence") {
		mpc::Mpc& mpc = mpc::Mpc::instance();
		mpc.init(44100, 1, 5);
		mpc.getSequencer().lock()->getActiveSequence().lock()->init(1);
		mpc.getDisk().lock()->moveForward("TEST1");
		mpc.getDisk().lock()->initFiles();

		auto f = mpc.getDisk().lock()->getFile("ALL_PGMS.APS");
		
		auto apsLoader = mpc::disk::ApsLoader(f);

		this_thread::sleep_for(chrono::milliseconds(500));
		
		mpc.getSampler().lock()->setSoundIndex(0);

		auto ls = mpc.getLayeredScreen().lock();

		vector<string> good;
		vector<string> bad;

		for (auto& screenName : screenNames)
		{
			int layerIndex = ls->openScreen(screenName);
			
			// We do a check for the most important screen
			if (screenName.compare("sequencer") == 0)
			{
				REQUIRE(layerIndex == 0);
			}

			if (layerIndex == -1)
			{
				bad.push_back(screenName + " could not be opened.");
				continue;
			}

			auto layer = ls->getFocusedLayer().lock();
			vector<vector<bool>> pixels(248, vector<bool>(60));
			layer->Draw(&pixels);
			int blackPixelCount = 0;

			for (auto& column : pixels)
			{
				for (auto b : column)
				{
					if (b)
					{
						blackPixelCount++;
					}
				}
			}

			// And another check for the most important screen
			if (screenName.compare("sequencer") == 0)
			{
				REQUIRE(blackPixelCount > 0);
			}

			if (blackPixelCount > 0)
			{
				good.push_back(screenName + " has " + to_string(blackPixelCount) + " black pixels");
			}
			else
			{
				bad.push_back(screenName + " is openable, but has 0 black pixels");
			}

		}

		MLOG("These screens are fine:");
		for (auto& msg : good)
		{
			MLOG(msg);
		}

		MLOG("\nThese screens are broken:");
		for (auto& msg : bad)
		{
			MLOG(msg);
		}
		printf("%i screens are fine and %i screens are broken. Check vmpc.log in ~/vMPC for more details.\n", good.size(), bad.size());

		REQUIRE(good.size() >= 42); // This will be increased as the screens get refactored.

	}
}
