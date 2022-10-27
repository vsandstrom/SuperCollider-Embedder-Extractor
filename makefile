make: scd_embed.o scd_extract.o audio.o
	g++ -o scd_extract scd_extract.o audio.o
	g++ -o scd_embed scd_embed.o audio.o

scd_extract.o: 
	g++ -o scd_extract.o -c ./src/scd_extract.cpp

scd_embed.o: 
	g++ -o scd_embed.o -c ./src/scd_embed.cpp 

audio.o:
	g++ -o audio.o -c ./src/audio.cpp


