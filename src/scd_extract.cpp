#include "audio.hpp"
#include <cstdio>
#include <cstring>
#include <malloc/_malloc.h>
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

		"    -e  <audio-file (wave)>				  | [ --extract, -E ]\n"
		"   [-o  <output path>]					      | [ --output, -O ]\n"
		"   [-h  <print this usage message>]		  | [ --help, -H ]\n\n"
	};

	if (argc < 3 || argc > 6) {
		printf("Wrong number of arguments\n\n%s", usage);
		return 1;
	}

	argc--;
	argv++;

	while (argc > 0) {
		if (!strcmp(*argv, "-e") || !strcmp(*argv, "--extract") || !strcmp(*argv, "-E")) {
			argv++;
			strcpy(path, *argv);
			argc--;
		} else if (!strcmp(*argv, "-o") || !strcmp(*argv, "--output") || !strcmp(*argv, "-O")) {
			argv++;
			strcpy(outpath, *argv);
			out = true;
			argc--;
		} else if (!strcmp(*argv, "-h") || !strcmp(*argv, "--help") || !strcmp(*argv, "-H")) {
			printf("%s", usage);
		} else {
			argc--;
			argv++;
		}
	}

	FILE* wave = fopen(path, "r");

	if ( wave == nullptr ) {
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

	if( output == nullptr ) {
		printf("Unable to write to new file or an output file with the same name already exists.\n");
		return 3;
	} else {
		printf("|-----> Opened file successfully\n");
	};

	fread(&cursor, 4, 1, wave);
	if (cursor == RIFF) { // Check if file is RIFF

		mainHeader.riffID = cursor;
		fread(&mainHeader.fileSize, 4, 1, wave);
		fread(&mainHeader.filetypeID, 4, 1, wave);

	} else {
		printf("File format not recognized\n");
	}

	int flag = 0;
	char *bextChunk = 0; // ptr to store bext chunk

	while( flag != 1 ) {
		// loop until 'data' chunk is found
		
		fread(&cursor, 4, 1, wave);

		if ( cursor == FMT ) {
			// if fmt
			
			fmtHeader.formatID = cursor;
			fread(&fmtHeader.formatSize, 4, 1, wave);
			fread(&fmtHeader.audioFormat, 2, 1, wave);
			fread(&fmtHeader.numChan, 2, 1, wave);
			fread(&fmtHeader.smplRate, 4, 1, wave);
			fread(&fmtHeader.byteRate, 4, 1, wave);
			fread(&fmtHeader.blockAlign, 2, 1, wave);
			fread(&fmtHeader.bps, 2, 1, wave);

		} else if ( cursor == BEXT ) { 
			// if bext
			
			bextHeader.bextID = cursor;
			fread(&bextHeader.bextSize, 4, 1, wave);
			bextChunk = (char*)malloc(sizeof(char) * bextHeader.bextSize);
			fread(bextChunk, bextHeader.bextSize, 1, wave);

			char blank = ' ';

			for(int i = 0, n = bextHeader.bextSize; i < n; ++i) {
				if (bextChunk[i] == 0x00) {
					fwrite(&blank, sizeof(char), 1, output);
				} else {
					fwrite(&bextChunk[i], sizeof(char), 1, output);
				}
			}
			break;

		} else if ( cursor == 0x61746164 ) {
			// if data
			
			fread(&dataHeader.dataSize, 4, 1, wave);

			flag = 1;

		} else {
			// if some junk chunk is trailing bext chunk
			
			fread(&cursor, 4, 1, wave); // read size of junk chunk
			fseek(wave, cursor, SEEK_CUR); // skip junk chunk
		}
	} 

	free(bextChunk);
	fclose(wave);
	fclose(output);

	return 0;

}
