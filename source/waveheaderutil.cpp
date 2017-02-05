#include "waveheaderutil.h"
#include <vector>
#include <istream>
#include "bitspersampleutil.h"
namespace
{
const int cChunkIdSize = 4;
const int cFmtChuncSize = 16;
template<typename T>
void read(std::istream& inputStream, T& number)
{
    std::vector<char> buf(sizeof(T));
    if ( inputStream.read( buf.data(), sizeof( T ) ) )
    {
        number = *reinterpret_cast<T*>( buf.data() );
    }
}
template<>
void read<std::string>(std::istream& inputStream, std::string& str)
{
    std::vector<char> buf(cChunkIdSize);
    if ( inputStream.read( buf.data(), cChunkIdSize ) )
    {
        str = std::string( buf.data(), cChunkIdSize );
    }
}
void readChunkHeader(std::istream& stream, WaveHeaderUtil::ChunkHeader& header)
{
    read(stream, header.chunkID);
    read(stream, header.chunkSize);
}
bool findChunkHeader(std::istream& stream, const std::string& chunkId, WaveHeaderUtil::ChunkHeader& header)
{
    WaveHeaderUtil::ChunkHeader chunkHeader;
    do
    {
        readChunkHeader(stream, chunkHeader);
        if (chunkHeader.chunkID == chunkId)
        {
            header = chunkHeader;
            return true;
        }
    } while (chunkHeader.chunkSize > 0 && stream.ignore(chunkHeader.chunkSize));
    return false;
}

void readRIFFChunk(std::istream& stream, WaveHeaderUtil::RIFFChunk& riffChunk)
{
    readChunkHeader(stream, riffChunk.header);
    read(stream, riffChunk.format);
}

void readFmtChunk(std::istream& stream, WaveHeaderUtil::FmtChunk& fmtChunk)
{
    findChunkHeader(stream, "fmt ", fmtChunk.header);
    read(stream, fmtChunk.audioFormat);
    read(stream, fmtChunk.numChannels);
    read(stream, fmtChunk.sampleRate);
    read(stream, fmtChunk.byteRate);
    read(stream, fmtChunk.blockAlign);
    read(stream, fmtChunk.bitsPerSample);
    stream.ignore( fmtChunk.header.chunkSize - cFmtChuncSize );//24 bits have extra info in fmt chunk
}

void ignoreChunk(std::istream& stream, WaveHeaderUtil::ChunkHeader& header)
{
    stream.ignore(header.chunkSize);
}

bool isValid(const WaveHeaderUtil::RIFFChunk& riffChunk)
{
    return (riffChunk.header.chunkID == "RIFF") &&
        (riffChunk.format == "WAVE");

}
bool isValid(const WaveHeaderUtil::FmtChunk& fmtChunk)
{
    return (fmtChunk.header.chunkID == "fmt ") &&
        //(fmtChunk.audioFormat == 1) && //I've found in internet wav 24 bit wav files with format -2. 
        (fmtChunk.numChannels > 0) &&
        (fmtChunk.numChannels < 3) &&
        (fmtChunk.sampleRate > 0) &&
        (fmtChunk.byteRate == fmtChunk.sampleRate * fmtChunk.numChannels * fmtChunk.bitsPerSample / 8) &&
        (fmtChunk.blockAlign == fmtChunk.numChannels * fmtChunk.bitsPerSample / 8) &&
        BitsPerSampleUtil::isValidBitsPerSample(fmtChunk.bitsPerSample);
}


}

namespace WaveHeaderUtil
{
bool isValid(const WaveHeader& wh)
{
    return
        ::isValid(wh.riffChunk) &&
        ::isValid(wh.FMTChunk) &&
        (wh.dataChunkHeader.chunkID == "data") &&
        (wh.dataChunkHeader.chunkSize > 0);
}

WaveHeader read(std::istream& stream)
{
    WaveHeader res;
    ::readRIFFChunk(stream, res.riffChunk);
    ::readFmtChunk(stream, res.FMTChunk);
    ::findChunkHeader(stream, "data", res.dataChunkHeader);
    return res;

}

}
