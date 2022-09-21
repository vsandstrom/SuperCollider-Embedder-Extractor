#include <cstdio>
#include <cstdint>
#include <malloc/_malloc.h>

#pragma once

struct WAVEHEADER {
	int32_t riffID; //  0x46464952 'FFIR' ('RIFF')
	int32_t fileSize; // 
	int32_t filetypeID; // 0x45564157 'EVAW' ('WAVE')
};

struct B_EXTENSION {
	int32_t bextID; // 0x74786562 'txeb' ('bext')
	int32_t bextSize;
};

struct FORMAT {
	int32_t formatID; // 0x20746d66 'tmf' ('fmt')
	int32_t formatSize;
	short audioFormat;
	short numChan;
	int32_t smplRate;
	int32_t byteRate;
	short blockAlign;
	short bps; // bits per sample
};

struct DATA {
	int32_t dataID; // 0x61746164 = 'atad' ('data')
	int32_t dataSize;
	int32_t* dataChunk;
}; 

/* namespace audio { */

class SuperColliderHeader {
	private:
	FILE* wave, *scdFile;
	char* bext;
	uint32_t scdSize;
	uint32_t junkSize;
	WAVEHEADER waveheader;
	B_EXTENSION b_extension;
	FORMAT format;
	DATA data;
	// Path for the output file to write to.
	char* path;
	
	int parseWaveHeader();
	
	int parseSCD();

	int writeNewFile();

	void error(int err);

	public:
	SuperColliderHeader(FILE* wave, FILE* scdFile, char* path);

	~SuperColliderHeader();

	int process();
};
/* } */
