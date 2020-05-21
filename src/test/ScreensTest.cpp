#include <catch2/catch.hpp>

#include <Mpc.hpp>

#include <sequencer/Sequencer.hpp>

#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/Layer.hpp>


#include <vector>
#include <string>

using namespace std;

vector<string> screenNames = {
	"trmove",
"black",
"mpc2000xl",
"sequencer",
"nextseq",
"nextsq",
"sequencer_step_selection",
"mixer_stereo",
"mixer_indiv",
"mixer_fxsend",
"mixer_stereo_link",
"mixer_indiv_link",
"mixer_fxsend_link",
"mixerv2",
"mixer",
"sequencer_step",
"load",
"save",
"selectdrum",
"selectdrum_mixer",
"mixersetup",
"programassign",
"programparams",
"format",
"setup",
"edit",
"barcopy",
"trmove",
"trmove_notselected",
"trmove_selected",
"user",
"drum",
"purge",
"fxedit",
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
"copynoteparameters",
"assignmentview",
"initpadassign",
"directory",
"loadaprogram",
"saveallfile",
"saveapsfile",
"saveaprogram",
"loadapsfile",
"loadasequence",
"loadasequencefromall",
"saveasequence",
"editmultiple",
"pasteevent",
"step_tc",
"insertevent",
"loadasound",
"saveasound",
"keeporretry",
"formatdisk",
"channelsettings",
"program",
"sound",
"velocitymodulation",
"veloenvfilter",
"velopitch",
"muteassign",
"autochromaticassignment",
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
"startfine",
"zonestartfine",
"endfine",
"zoneendfine",
"looptofine",
"loopendfine",
"editsound",
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
"numberofzones",
"cantfindfile",
"name",
"filealreadyexists",
"recordjam",
"deleteallsequences",
"deletealltracks",
"deleteprogram",
"deletefolder",
"deletefile",
"createnewprogram",
"copyprogram",
"copysound",
"deleteallprograms",
"deletesound",
"deleteallsound",
"convertsound",
"monotostereo",
"stereotomono",
"convertnodoit",
"resample",
"bpmmatch",
"deletesong",
"metronomesound",
"deletetrack",
"copytrack",
"copysequence",
"deletesequence",
"midimonitor",
"deleteallfiles"
};

SCENARIO("All screens can be opened", "[gui]") {

	GIVEN("An initialized Mpc with an initialized Sequence") {
		mpc::Mpc& mpc = mpc::Mpc::instance();
		mpc.init(44100, 1, 5);
		mpc.getSequencer().lock()->getActiveSequence().lock()->init(1);
		auto ls = mpc.getLayeredScreen().lock();

		for (auto& screenName : screenNames)
		{
			int layerIndex = ls->openScreen(screenName);
			
			if (layerIndex == -1)
			{
				printf("%s could not be opened!\n", screenName.c_str());
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

			if (blackPixelCount > 0)
			{
				printf("%s has %i black pixels\n", screenName.c_str(), blackPixelCount);
			}
			else
			{
				printf("%s is completely empty!\n", screenName.c_str());
			}

		}

	}
}
