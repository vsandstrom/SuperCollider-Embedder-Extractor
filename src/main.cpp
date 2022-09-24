// Encode supercollider code into a wave file
#include "audio.hpp"
#include <cstdio>
#include <cstring>
#include <malloc/_malloc.h>
#include <string.h>

#define macro


int main(int argc, char** argv) {

	char* audio_path;
	char* scd_path;
	char* out_path;
	bool out = false;

	char usage[] = {"Usage: \n    -a <audio-file (wave)>\n    -s <supercollider-file>\n    [-o <output path>]\n\n"};

	if (argc == 1 || argc > 7) {
		printf("Wrong number of command line arguments\n\n%s", usage);
		return -1;
	}

	// Skip name of program
	argv++;
	argc--;

	// Parse CLI
	while (argc > 0)  {
		if (!strcmp(*argv, "-a\0"))// || !strcmp(*argv, "--audio") || !strcmp(*argv, "--A"))
		{
			argv++;
			audio_path = (char*)malloc(40);
			strcpy( audio_path, *argv );
			argc--;
		} else if (!strcmp(*argv, "-s\0")) //|| !strcmp(*argv, "--scd") || !strcmp(*argv, "-S"))
		{
			argv++;
			scd_path = (char*)malloc(40);
			strcpy(scd_path, *argv);
			argc--;
		} else if (!strcmp(*argv, "-o\0")) // || !strcmp(*argv, "--output") || !strcmp(*argv, "-O"))
		{
			argv++;
			out_path = (char*)malloc(40);
			strcpy(out_path, *argv);
			out = true;
			argc--;
		} else {
			argv++;
			argc--;
		}

	}

	// Proceed only if we have paths
	if (audio_path && scd_path) {
		FILE* audioFile = fopen(audio_path, "r");
		if (audioFile == nullptr) {
			printf("Path to audio-file was not correct\n\n%s", usage); 
			return 1;
		}
		FILE* scdFile = fopen(scd_path, "r");
		if (scdFile == nullptr) {
			printf("Path to scd-file was not correct\n\n%s", usage); 
			return 1;
		} 
	
		// If no out_path was supplied, create one
		if (!out) {
			int len = strlen(audio_path);
			char ending[15] = "_embedded.wav\0";
			// this is unwanted because we do not want to create arrays at runtime, 
			// should be better with a char*
			char *newPath = new char[len + 14];
			strcpy(newPath, audio_path);
			for (int i = 0, n = 15; i < n; i++) {
				newPath[i + len - 4] = ending[i];
			}
			out_path = newPath;
		}
		
		SuperColliderHeader sch(audioFile, scdFile, out_path);
		sch.process();

		fclose(audioFile);
		fclose(scdFile);

	} else {
		return -1;

	}


	return 0;
}
