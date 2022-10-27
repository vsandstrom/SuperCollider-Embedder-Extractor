#include <cstdio>
#include <cstdint>

#pragma once

#define RIFF 0x46464952
#define FMT 0x20746d66
#define BEXT 0x74786562
#define DATA 0x61746164

#define NO_ERR 0

struct WaveHeader {
	uint32_t riffID; //  0x46464952 'FFIR' ('RIFF')
	int32_t fileSize; // 
	uint32_t filetypeID; // 0x45564157 'EVAW' ('WAVE')
};

struct Bext {
	uint32_t bextID; // 0x74786562 'txeb' ('bext')
	uint32_t bextSize;
};

struct Format {
	uint32_t formatID; // 0x20746d66 'tmf' ('fmt')
	uint32_t formatSize;
	short audioFormat;
	short numChan;
	uint32_t smplRate;
	uint32_t byteRate;
	short blockAlign;
	short bps; // bits per sample
};

struct Data {
	uint32_t dataID; // 0x61746164 = 'atad' ('data')
	int32_t dataSize;
	char* dataChunk;
}; 

class SuperColliderHeader {
	private:
	FILE* wave, *scdFile, *outputFile;
	char* bext, *bextChunk;
	int32_t scdSize;
	int32_t junkSize;
	WaveHeader waveheader;
	Bext b_extension;
	Format format;
	Data data;
	// Path for the output file to write to.
	char* path;
	
	int parseWaveHeader();
	
	int parseSCD();

	int writeNewFile();

	int writeParsedFile();

	public:
	SuperColliderHeader(FILE* wave, FILE* scdFile, char* path);
	
	// Overloaded constructor for extraction only
	SuperColliderHeader(FILE* wave, FILE* scdFile);

	~SuperColliderHeader();

	int process();
	int extract();
	void error(int err);
};
