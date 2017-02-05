#ifndef waveheaderutil_h__
#define waveheaderutil_h__

#include <string>
#include <tuple>
#include <cstdint>
namespace WaveHeaderUtil
{

//The specification of wav format and comments below are taken from here http://soundfile.sapp.org/doc/WaveFormat/
struct ChunkHeader
{
    std::string chunkID;
    int32_t chunkSize = 0;
};
struct FmtChunk
{
    ChunkHeader header;
    int16_t audioFormat = 0;      //PCM = 1 (i.e. Linear quantization). Values other than 1 indicate some form of compression.
    int16_t numChannels = 0;      //Mono = 1, Stereo = 2, etc. (>2 is not supported by Lame library and by this programm)
    int32_t sampleRate = 0;       //8000, 44100, etc.
    int32_t byteRate = 0;         //== SampleRate * NumChannels * BitsPerSample/8
    int16_t blockAlign = 0;       //== NumChannels * BitsPerSample/8. The number of bytes for one sample including all channels.
    int16_t bitsPerSample = 0;    //8 bits = 8, 16 bits = 16, etc.
};

struct RIFFChunk
{
    ChunkHeader header;           //Contains the letters "RIFF"
    std::string format;           //Contains the letters "WAVE" 

};

struct WaveHeader
{
    RIFFChunk riffChunk;   
    FmtChunk FMTChunk;
    ChunkHeader dataChunkHeader;   //Contains the letters "data" 
                                   //size == NumSamples * NumChannels * BitsPerSample/8. This is the number of bytes in the data. 
 };


bool isValid(const WaveHeader& wh);
WaveHeader read(std::istream& stream);
}

#endif // waveheaderutil_h__
