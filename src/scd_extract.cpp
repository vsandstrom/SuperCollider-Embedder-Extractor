#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <err.h>

// TODO: check on fopen function.
// TODO: solve infinite loop

#define RIFF 0x46464952
#define WAVE 0x45564157
#define FMT 0x20746d66
#define BEXT 0x74786562
#define DATA 0x61746164

typedef struct  {
	int riffID; //  0x46464952 'FFIR' ('RIFF')
	int headerSize;
	int filetypeID; // 0x45564157 'EVAW' ('WAVE')
} WaveHeader;

typedef struct  {
	int bextID; // 0x74786562 'txeb' ('bext')
	int bextSize;
} Bextension;

typedef struct {
	int formatID; // 0x20746d66 'tmf' ('fmt')
	int formatSize;
	short audioFormat;
	short numChan;
	int smplRate;
	int byteRate;
	short blockAlign;
	short bps; // bits per sample
} Format;

typedef struct {
	int dataID; // 0x61746164 = 'atad' ('data')
	int dataSize;
	int32_t* dataChunk;
} Data; 


int main(int argc, char** argv) {
	WaveHeader mainHeader;
	Format fmtHeader;
	Bextension bextHeader;
	Data dataHeader;
	int cursor;

	if (argc != 2) {
		printf("wrong number of arguments\n");
		return 1;
	}
	

	char path[40];
	sprintf(path, "./%s", argv[1]); // file opened must be in same directory as c program
	printf("\n%s\n", path);
	
	FILE* wave = fopen(path, "r");
	FILE* output = fopen("parsed.scd", "w");

	if(wave == NULL) {
		printf("unable to open file\n");
		return 3;
	} else {
		printf("opened file successfully\n");
	};
	// fread(&mainHeader, sizeof(struct WAVEHEADER), 1, wave);
	fread(&cursor, 4, 1, wave);
	if (cursor == RIFF) { // Check if file is RIFF

		mainHeader.riffID = cursor;
		fread(&mainHeader.headerSize, 4, 1, wave);
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
			
			printf("\nbext\n\n");
			bextHeader.bextID = cursor;
			fread(&bextHeader.bextSize, 4, 1, wave);
			bextChunk = (char*)malloc(sizeof(char) * bextHeader.bextSize);
			fread(bextChunk, bextHeader.bextSize, 1, wave);

			/* output = malloc(sizeof(char) * bextHeader.bextSize); */

			char blank = ' ';

			for(int i = 0, n = bextHeader.bextSize; i < n; ++i) {
				if (bextChunk[i] == 0x00) {
					fwrite(&blank, sizeof(char), 1, output);
//					printf(" ");
				} else {
					fwrite(&bextChunk[i], sizeof(char), 1, output);
//					printf("%c", bextChunk[i]);
					

				}
			}
			printf("\n");

		} else if ( cursor == 0x61746164 ) {
			// if data
			
			fread(&dataHeader.dataSize, 4, 1, wave);
			printf("%i\n", dataHeader.dataSize);

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

};
