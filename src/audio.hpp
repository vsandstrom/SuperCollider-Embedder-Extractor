#include <cstdio>
#include <cstdint>
#include <malloc/_malloc.h>

#pragma once

struct WAVEHEADER {
	uint32_t riffID; //  0x46464952 'FFIR' ('RIFF')
	uint32_t fileSize; // 
	uint32_t filetypeID; // 0x45564157 'EVAW' ('WAVE')
};

struct B_EXTENSION {
	uint32_t bextID; // 0x74786562 'txeb' ('bext')
	uint32_t bextSize;
};

struct FORMAT {
	uint32_t formatID; // 0x20746d66 'tmf' ('fmt')
	uint32_t formatSize;
	short audioFormat;
	short numChan;
	uint32_t smplRate;
	uint32_t byteRate;
	short blockAlign;
	short bps; // bits per sample
};

struct DATA {
	uint32_t dataID; // 0x61746164 = 'atad' ('data')
	uint32_t dataSize;
	char* dataChunk;
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
