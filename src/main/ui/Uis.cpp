#include <ui/Uis.hpp>

using namespace mpc::ui;
using namespace std;

Uis::Uis() 
{
	if (noteNames.size() != 128)
	{
		noteNames = vector<string>(128);
		int octave = -2;
		int noteCounter = 0;
		
		for (int j = 0; j < 128; j++)
		{
			string octaveString = to_string(octave);
		
			if (octave == -2)
			{
				octaveString = u8"\u00D2";
			}

			if (octave == -1)
			{
				octaveString = u8"\u00D3";
			}

			noteNames[j] = someNoteNames[noteCounter] + octaveString;
			noteCounter++;
			
			if (noteCounter == 12)
			{
				noteCounter = 0;
				octave++;
			}

		}
	}
}

vector<string> Uis::noteNames;
