#include "audio.hpp"
#include <cstdio>
#include <cstring>
#include <string.h>

#define SUFFIX_LEN 4

int main(int argc, char** argv) {
	WaveHeader mainHeader;
	Format fmtHeader;
	Bext bextHeader;
	Data dataHeader;
	int cursor;
	char path[40];
	char outpath[40];
	bool out = false;

	char usage[] = {
		"Usage: \n\n"
		"    -e  <audio-file (wave)>\t\t| [ --extract, -E ]\n"
		"   [-o  <output path>]\t\t\t| [ --output, -O ]\n"
		"   [-h  <print this usage message>]\t| [ --help, -H ]\n\n"
	};

	if (argc < 2 || argc > 6) {
		printf("Wrong number of arguments\n\n%s", usage);
        printf("%i \n", argc);
		return 1;
	}

	argc--;
	argv++;

    // TODO: if supplying only a 'flag' but no string, the program crashes with a segmentation fault.
	while (argc > 0) {
		if (
                !strcmp(*argv, "-e") || 
                !strcmp(*argv, "--extract") || 
                !strcmp(*argv, "-E")) {
			argv++;
            if (*argv[0] == '-') return -1;
			strcpy(path, *argv);
			argc--;
		} else if (
                !strcmp(*argv, "-o") || 
                !strcmp(*argv, "--output") || 
                !strcmp(*argv, "-O")) {
			argv++;
            if (*argv[0] == '-') return -1;
			strcpy(outpath, *argv);
			out = true;
			argc--;
		} else if (
                !strcmp(*argv, "-h") || 
                !strcmp(*argv, "--help") || 
                !strcmp(*argv, "-H")) {
			printf("%s", usage);
            return 0;
		} else {
			argc--;
			argv++;
		}
	}
    


	FILE* wave = fopen(path, "r");

	if ( wave == NULL ) {
		printf("Unable to open input file for extraction\n");
		return 3;
	} else {
		printf("|-----> Opened input file successfully\n");
	};

	FILE* output;

	if (out) {
		output = fopen(outpath, "wx");
	} else {
		int len = strlen(path);
		char ending[15] = "_parsed.scd\0";
		char* newPath = new char[len + 15];
		strcpy(newPath, path);

		for (int i = 0, n = 15; i < n; ++i) {
			newPath[i + len - SUFFIX_LEN] = ending[i];
		}


		// Append "_parsed.scd" to input file name
		output = fopen(newPath, "wx");
	}

	if( output == NULL ) {
		printf("Unable to write to new file or an output file with the same name already exists.\n");
		return 3;
	} else {
		printf("|-----> Opened file successfully\n");
	};

	int err;

	SuperColliderHeader sch(wave, output);

	err = sch.extract();
	if (err) {
		sch.error(err); return err;
	}

	fclose(wave);
	fclose(output);

	return 0;

}
