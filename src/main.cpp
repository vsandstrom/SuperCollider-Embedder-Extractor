// Encode supercollider code into a wave file
#include "audio.hpp"
#include <string.h>


int main(int argc, char** argv) {

	char* audio_path;
	char* scd_path;
	char* out_path;
	bool out;

	// Skip name of program
	argv++;
	argc--;

	// Parse CLI
	while (argc!=0)  {
		if (!strcmp(*argv, "-a") || !strcmp(*argv, "--audio") || !strcmp(*argv, "--A")){
			argv++;
			audio_path = *argv;
			argc--;
		}
		if (!strcmp(*argv, "-a") || !strcmp(*argv, "--scd") || !strcmp(*argv, "-S")){
			argv++;
			scd_path = *argv;
			argc--;
		}

		if (!strcmp(*argv, "-o") || !strcmp(*argv, "--output") || !strcmp(*argv, "-O")){
			argv++;
			out_path = *argv;

			argc--;

		}

	}
	
	/* SuperColliderHeader sch = new SuperColliderHeader(wave, scdFile, path); */


	return 0;
}
