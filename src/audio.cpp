#include "audio.hpp"
#include <cstdio>
#include <cstdint>
#include <cstdlib>

int SuperColliderHeader::parseWaveHeader(){
	uint32_t cursor;

	bool dataFound = false;
	bool bextFound = false;

	junkSize = 0;
	
	if (wave == nullptr) {
		return 1;
	}

	// Read the first header part into struct
	fread(&cursor, 4, 1, wave);
	if (cursor == RIFF) { // Check if file is RIFF
		// Allocate memory for the WAVEHEADER
		waveheader.riffID = cursor;
		fread(&waveheader.fileSize, 4, 1, wave);
		fread(&waveheader.filetypeID, 4, 1, wave);

	} else {
		return 2;
	}
	
	while (!dataFound) {
		// loop until 'data' chunk is found
		
		fread(&cursor, 4, 1, wave);

		if ( cursor == FMT ) {
			// if fmt
			
			printf("fmt found\n");
			
			format.formatID = cursor;
			fread(&format.formatSize, sizeof(uint32_t), 1, wave);
			fread(&format.audioFormat, sizeof(uint16_t), 1, wave);
			fread(&format.numChan, sizeof(uint16_t), 1, wave);
			fread(&format.smplRate, sizeof(uint32_t), 1, wave);
			fread(&format.byteRate, sizeof(uint32_t), 1, wave);
			fread(&format.blockAlign, sizeof(uint16_t), 1, wave);
			fread(&format.bps, sizeof(uint16_t), 1, wave);

		} else if ( cursor == BEXT ) { 
			// if bext
			
			printf("bext found\n");
			b_extension.bextID = cursor;
			fread(&b_extension.bextSize, 4, 1, wave);
			bextChunk = (char*)malloc(sizeof(char) * b_extension.bextSize);
			fread(bextChunk, b_extension.bextSize, 1, wave);
			bextFound = true;

		} else if ( cursor == DATA ) {
			// if data
			
			printf("data found\n");
			data.dataID = cursor;
			fread(&data.dataSize, 4, 1, wave);
			printf("Data size: %i\n", data.dataSize);
			

			dataFound = 1;

		} else {
			// if some junk chunk is trailing bext chunk
			
			printf("junk found\n");
			fread(&cursor, 4, 1, wave); // read size of junk chunk
			junkSize += cursor;
			fseek(wave, cursor, SEEK_CUR); // skip junk chunk
		}
	}
	
	waveheader.fileSize -= junkSize;

	if (!bextFound) {
		b_extension.bextID = 0x74786562;
		b_extension.bextSize = 0;
	}

	if (format.bps == 16) {
		data.dataChunk = (char*)malloc((data.dataSize)); 
		printf("Allocated data-chunk %lu\n", sizeof(data.dataChunk));
	}
	else if (format.bps == 24) {
		data.dataChunk = (char*)malloc((data.dataSize));
		printf("Allocated data-chunk %lu\n", sizeof(data.dataChunk));
	}

	fread(data.dataChunk, data.dataSize, 1, wave);

	// check if data has been read properly
	if (ferror(wave)) {
		return 7;
	}

	return 0;
}


int SuperColliderHeader::parseSCD() {
	unsigned char validBytes = 0;

	// get scdFile size in bytes
	fseek(scdFile, 0L, SEEK_END);
	scdSize = ftell(scdFile);
	rewind(scdFile);

	// Allign to next 32bit block
	bext = (char*)malloc(scdSize + (scdSize % 4));

	validBytes = fread(bext, sizeof(unsigned char), scdSize, scdFile);
	printf("validBytes: %i\nscdsize: %i\n", validBytes, scdSize);
	// Check if written bytes correspond to scdSize
	if (ferror(scdFile)) {
		return 3;
	}

	printf("%s\n", bext);
	return 0;
}

int SuperColliderHeader::writeNewFile() {
	
	// Compare current BEXT chunk size with the scdSize:
	waveheader.fileSize += (scdSize - b_extension.bextSize);
	b_extension.bextSize = scdSize;

	FILE * outFile = fopen(path, "wx");
	if ( outFile == nullptr ){
		return 8;
	}

<<<<<<< HEAD
	if (fwrite(&waveheader, 4, 3, outFile) < 1) return 10;

	// 'fwrite' was real picky about type sizes here;
    // should be solved by an __attribute__((packed)) on format-struct
=======
	// WRITE NEW FILE TO DISK!

	if (fwrite(&waveheader, 4, 3, outFile) < 1) return 10;

	// 'fwrite' was real picky about type sizes here;
	// Should be solved with an __attribute__((packed)) on format struct
>>>>>>> bbab1c012932d73cedfa18d4bcdbc95669d87c4c
	if (fwrite(&format.formatID, sizeof(uint32_t), 1, outFile) < 1 ||
		fwrite(&format.formatSize, sizeof(uint32_t), 1, outFile) < 1 ||
		fwrite(&format.audioFormat, sizeof(uint16_t), 1, outFile) < 1 ||
		fwrite(&format.numChan, sizeof(uint16_t), 1, outFile) < 1 ||
		fwrite(&format.smplRate, sizeof(uint32_t), 1, outFile) < 1 ||
		fwrite(&format.byteRate, sizeof(uint32_t), 1, outFile) < 1 ||
		fwrite(&format.blockAlign, sizeof(uint16_t), 1, outFile) < 1 ||
		fwrite(&format.bps, sizeof(uint16_t), 1, outFile) < 1
			) return 12;


	if (fwrite(&b_extension, 4, 2, outFile) < 1) return 13;
	if (fwrite(bext, scdSize, 1, outFile) < 1) return 14;
	if (fwrite(&data.dataID, 4, 1, outFile) < 1) return 15;
	if (fwrite(&data.dataSize, 4, 1, outFile) < 1) return 16;
	if (fwrite(data.dataChunk, data.dataSize, 1, outFile) < 1) return 17;

	return 0;
}

int SuperColliderHeader::writeParsedFile() {
	char blank = ' ';

	for (int i = 0, n = b_extension.bextSize; i < n; ++i) {
		if (bextChunk[i] == 0x00) {
			if (!fwrite(&blank, sizeof(char), 1, outputFile)) return 17;
		}
		else {
			if (!fwrite(&bextChunk[i], sizeof(char), 1, outputFile)) return 17;
		}
	}


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
	
		// Error messages for extraction
		case 17: printf("Error occured when trying to write BEXT to scd-file"); break;
		case 18: printf("Error occured during extraction"); break;


	}
}

SuperColliderHeader::SuperColliderHeader(
		FILE* wave, FILE* scdFile, char* path) : wave { wave }, scdFile { scdFile }, path {path}
{};

// Overloaded constructor for extraction only
SuperColliderHeader::SuperColliderHeader(
		FILE* wave, FILE* outputFile) : wave { wave }, outputFile { outputFile }
{};

SuperColliderHeader::~SuperColliderHeader() {
	free(bextChunk);
	free(data.dataChunk);
}


int SuperColliderHeader::process() {
	if (wave == nullptr || scdFile == nullptr) {
		return 4;
	}
	int err;
	err = parseWaveHeader();
	if (err != NO_ERR) {
		error(err); return 5;
	}
	err = parseSCD();
	if (err != NO_ERR) {
		error(err); return 6;
	}
	err = writeNewFile();
	if (err != NO_ERR) {
		error(err); return 9;
	}


	free(bext);

	return 0;
}

int SuperColliderHeader::extract() {
	if (wave == nullptr || scdFile == nullptr) {
		return 4;
	}

	int err;
	err = parseWaveHeader();
	if (err != NO_ERR) {
		error(err); return 5;
	}
	err = writeParsedFile();
	if (err != NO_ERR) {
		error(err); return 18;
	}

	return 0;
}


