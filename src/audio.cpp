#include "audio.hpp"
#include <cstdio>
#include <cstdint>
#include <malloc/_malloc.h>

/* using namespace audio; */

int SuperColliderHeader::parseWaveHeader(){
	uint32_t cursor;

	bool dataFound = false;
	bool bextFound = false;
	
	char *bextChunk = 0; // ptr to store bext chunk
	
	// Don't assume the file has been opened
	if (wave == nullptr) {
		return 1;
	}

	// Read the first header part into struct

	fread(&cursor, 4, 1, wave);
	if (cursor == 0x46464952) { // Check if file is RIFF
		// Allocate memory for the WAVEHEADER
		waveheader.riffID = cursor;
		fread(&waveheader.fileSize, 4, 1, wave);
		fread(&waveheader.filetypeID, 4, 1, wave);

	} else {
		return 2;
	}
	
	while (!dataFound) {
		bool bextFound = false;
		// loop until 'data' chunk is found
		
		fread(&cursor, 4, 1, wave);

		if ( cursor == 0x20746d66 ) {
			// if fmt
			
			format.formatID = cursor;
			fread(&format.formatSize, 4, 1, wave);
			fread(&format.audioFormat, 2, 1, wave);
			fread(&format.numChan, 2, 1, wave);
			fread(&format.smplRate, 4, 1, wave);
			fread(&format.byteRate, 4, 1, wave);
			fread(&format.blockAlign, 2, 1, wave);
			fread(&format.bps, 2, 1, wave);

		} else if ( cursor == 0x74786562 ) { 
			// if bext
			
			b_extension.bextID = cursor;
			fread(&b_extension.bextSize, 4, 1, wave);
			// Unnecessary malloc and read of bext chunk already present in file
			bextChunk = (char*)malloc(b_extension.bextSize);
			fread(bextChunk, b_extension.bextSize, 1, wave);

		} else if ( cursor == 0x61746164 ) {
			// if data
			
			data.dataID = cursor;
			fread(&data.dataSize, 4, 1, wave);
			/* printf("%i\n", data.dataSize); */
			

			dataFound = 1;

		} else {
			// if some junk chunk is trailing bext chunk
			
			fread(&cursor, 4, 1, wave); // read size of junk chunk
			fseek(wave, cursor, SEEK_CUR); // skip junk chunk
		}
	}

	if (format.bps == 16) data.dataChunk = (int32_t*)malloc(4 * (data.dataSize / 16));
	else if (format.bps == 24) data.dataChunk = (int32_t*)malloc(4 * (data.dataSize / 24));

	if (data.dataChunk == nullptr) return 7;

	// should probably check if data has been read properly
	fread(data.dataChunk, data.dataSize, 1, wave);

	return 0;
}


int SuperColliderHeader::parseSCD() {
	uint32_t cursor = 0;
	char buffer[4]{0};
	unsigned char validBytes = 0;
	scdFile = 0;

	fseek(scdFile, 0L, SEEK_END);
	scdSize = ftell(scdFile);
	rewind(scdFile);

	// Allign to next 32bit block
	bext = (char*)malloc(scdSize + (scdSize % 4));

	validBytes = fread(bext, 4, 1, scdFile);
	if (validBytes != scdSize) {
		return 3;
	}

	// Pad the end of the file with NULL
	for (int i = scdSize, n = scdSize + (scdSize % 4); i < n; i++){
		bext[i] = 0x00;
	}
	return 0;
}

int SuperColliderHeader::writeNewFile() {
	// Compare current BEXT chunk size with the scdSize:
	
	int32_t sizeDiff = scdSize - b_extension.bextSize;
	waveheader.fileSize += sizeDiff;

	FILE * outFile;
	if ( outFile == nullptr ){
		return 8;
	}

	// WRITE NEW FILE TO DISK!

	outFile = fopen(path, "w");

	// TODO: error handling here:
	// check if number of objects is same as written, (which is '1' on all counts)
	if (fwrite(&waveheader, sizeof(WAVEHEADER), 1, outFile) < 1) return 10;
	if (fwrite(&format, sizeof(FORMAT), 1, outFile) < 1) return 12;
	if (fwrite(&b_extension, sizeof(B_EXTENSION), 1, outFile) < 1) return 13;
	if (fwrite(bext, sizeof(b_extension.bextSize), 1, outFile) < 1) return 14;
	if (fwrite(&data.dataID, 4, 1, outFile) < 1) return 15;
	if (fwrite(&data.dataSize, 4, 1, outFile) < 1) return 16;
	if (fwrite(data.dataChunk, data.dataSize, 1, outFile) < 1) return 17;

	return 0;
}
	
void SuperColliderHeader::error(int err) {
	switch(err){
		// Class initialization errors
		case 4: printf("An attempt to run process function before object initialization, object has not been passed a FILE*"); break;

		// Error messages for Wave file
		case 1: printf("File had not been opened successfully"); break;
		case 2: printf("File format not recognized\n"); break;
		case 5: printf("---> parseWaveHeader-function was not successful."); break;
		case 7: printf("Error occurred when allocating memory for data-chunk"); break;

		// Error messages for SCD file
		case 6: printf("---> parseSCD-function was not successful."); break;
		case 3: printf("An error occurred when attempting to read entire SuperCollider file to memory."); break;

		// Error messages for output file
		case 8: printf("Error when trying to initiate writing output file"); break;
		case 9: printf("Error occured when trying to write new file"); break;
		case 10: printf("Error occured when trying to write WAVEHEADER"); break;
		case 11: printf("Error occured when trying to write FORMAT"); break;
		case 12: printf("Error occured when trying to write BEXT-INFO"); break;
		case 13: printf("Error occured when trying to write BEXT-DATA ( embedded SuperCollider file )"); break;
		case 14: printf("Error occured when trying to write DATA-INFO ID"); break;
		case 15: printf("Error occured when trying to write DATA-INFO SIZE"); break;
		case 16: printf("Error occured when trying to write DATA ( Audio information )"); break;


	}
}

SuperColliderHeader::SuperColliderHeader(
		FILE* wave, FILE* scdFile, char* path) : wave { wave }, scdFile { scdFile }, path {path}
{};


int SuperColliderHeader::process() {
	if (wave == nullptr || scdFile == nullptr) {
		return 4;
	}
	int err;
	err = parseWaveHeader();
	if (err != 0) error(err); return 5;
	err = parseSCD();
	if (err != 0) error(err); return 6;
	err = writeNewFile();
	if (err != 0) error(err); return 9;

	return 0;
}


