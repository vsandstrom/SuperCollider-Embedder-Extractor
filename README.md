# SuperCollider Embedder/Extractor
A silly project for embedding a file into another file and then extracting it. What if you could hide the SuperCollider source code of your music as an easter egg inside the music it has created? 

## Compile
A requirement seems to be g++-11 or later

```bash
# clone this repo
$ git clone https://github.com/vsandstrom/SuperCollider-Embedder-Extractor.git 

# move into repo
$ cd SuperCollider-Embedder-Extractor/

# run compiler
$ g++ -o scd_embed src/scd_embed.cpp src/audio.cpp
$ g++ -o scd_extract src/scd_extract.cpp src/audio.cpp
```

## Usage

To use the embedder: 
```bash
$ ./scd_embed -a <audio-file> -s <supercollider-file> [ -o <output filename> ] [ -h <prints usage help> ]
```

To make sure that the text has been embedded, use:
```bash
$ xxd -c 40 -l 5000 <path/to/file.wav>
```

or use the free software [hex fiend](https://hexfiend.com/).


Reverse the process by extracting information into a separate file

To use the extractor:
```bash
$ ./scd_extract -e <target audio-file> [ -o <output filename> ] [ -h <prints usage help> ]
```
This could also be used to parse any BEXT-chunk, if you are curious what it usually contains.
