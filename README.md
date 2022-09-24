# SuperCollider Embedder/Extractor
Embeds a file, preferably a text-file, into the BEXT-chunk of a wave-file header.

This comes from the idea if there could be a way to ship the source code of a supercollider composition together with the sounding medium in a single file.

## Compile

```bash
# clone this repo
$ git clone https://github.com/vsandstrom/SuperCollider-Embedder-Extractor.git 

# move into repo
$ cd SuperCollider-Embedder-Extractor/

# run compiler
$ g++ -o scd_embed src/main.cpp src/audio.cpp
```

## Usage

To use the embedder: 
```bash
$ ./scd_embed -a <audio-file> -s <supercollider-file> [ -o <output filename> ] [ -h <prints usage help> ]
```


Reverse the process by extracting information into a separate file

(( the extraction is a work in progress))
To make sure that the text has been embedded, use:
```bash
$ xxd -c 40 -l 5000 <path/to/file.wav>
```

or use the free software [hex fiend](https://hexfiend.com/).
