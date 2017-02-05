#include "iostream"
#include "fstream"
#include <vector>
#include <assert.h>
#include <array>
#include "waveheaderutil.h"
#include "lame.h"
#include "wavtomp3.h"
#include "threadutil.h"
#include "bitspersampleutil.h"
#include "bigchunkreader.h"
using namespace std;
namespace
{
const int cBufSize = 5000;
const int cMp3BufSize = 15000; //worst case is 1.25*num_samples + 7200.

lame_t constructLame(const WaveHeaderUtil::WaveHeader& waveHeader)
{
    lame_t lame = lame_init();
    lame_set_in_samplerate(lame, waveHeader.FMTChunk.sampleRate);
    lame_set_num_channels(lame, waveHeader.FMTChunk.numChannels);
    if (waveHeader.FMTChunk.numChannels == 1)
    {
        lame_set_mode(lame, MONO);
    }
    else
    {
        lame_set_mode(lame, STEREO);
    }

    auto error = lame_init_params(lame);
    if (error < 0)
    {
        ThreadUtil::threadSafeLogging("can't init the encoder");
        return nullptr;
    }
    return lame;
}

int getBufferSize(int16_t blockAlign)
{
    return (cBufSize / blockAlign) * blockAlign;
}

void convert(const WaveHeaderUtil::WaveHeader& waveHeader, std::istream& wavFile, std::ostream& mp3File)
{
    auto lame = constructLame(waveHeader);
    const auto cPCMBufSize = getBufferSize(waveHeader.FMTChunk.blockAlign);
    std::vector<unsigned char> mp3Buffer(cMp3BufSize);
    BigChunkReader bigChunkReader(wavFile, cPCMBufSize, waveHeader.dataChunkHeader.chunkSize);
    int numReadBytes = 0;
    do
    {
        const auto& portion = bigChunkReader.readPortion();
        numReadBytes = portion.size();
        if (!numReadBytes)
        {
            break;
        }
        auto PCMBuffer16 = BitsPerSampleUtil::convert(portion, waveHeader.FMTChunk.bitsPerSample);
        int written = 0;
        if (waveHeader.FMTChunk.numChannels == 1)
        {
            written = lame_encode_buffer(lame, PCMBuffer16.data(), PCMBuffer16.data(), numReadBytes / (waveHeader.FMTChunk.bitsPerSample / 8), mp3Buffer.data(), cMp3BufSize);
        }
        else
        {
            written = lame_encode_buffer_interleaved(lame, PCMBuffer16.data(), numReadBytes / (waveHeader.FMTChunk.bitsPerSample / 8) / 2, mp3Buffer.data(), cMp3BufSize);
        }
        mp3File.write(reinterpret_cast<char*>(mp3Buffer.data()), written);
    } while (wavFile);
    auto written = lame_encode_flush(lame, mp3Buffer.data(), cMp3BufSize);
    mp3File.write(reinterpret_cast<char*>(mp3Buffer.data()), written);
    lame_close(lame);

}

}
namespace WavToMP3
{
bool convert(const std::string& pathLoad, const std::string& pathSave)
{
    ThreadUtil::threadSafeLogging("Converting:" + pathLoad);
    std::ifstream wavFile;
    wavFile.open(pathLoad.c_str(), std::ios::binary);
    if (!wavFile.is_open())
    {
        ThreadUtil::threadSafeLogging("ERROR: Can't open the file:" + pathLoad);
        return false;
    }
    const auto waveHeader = WaveHeaderUtil::read(wavFile);
    if (wavFile.bad())
    {
        ThreadUtil::threadSafeLogging("ERROR: Error happened while reading the file  " + pathLoad);
        return false;
    }
    if (!WaveHeaderUtil::isValid(waveHeader))
    {
        ThreadUtil::threadSafeLogging("ERROR: Doesn't support the format of the file:" + pathLoad);
        return false;
    }
    std::ofstream mp3File;
    mp3File.open(pathSave.c_str(), std::ios::binary);
    if (!mp3File.is_open())
    {
        ThreadUtil::threadSafeLogging("ERROR: Can't save the file: " + pathSave);
        return false;
    }
    ::convert(waveHeader, wavFile, mp3File);
    if (wavFile.bad())
    {
        ThreadUtil::threadSafeLogging("ERROR: Error happened while reading the file  " + pathLoad);
        return false;
    }
    if (mp3File.bad())
    {
        ThreadUtil::threadSafeLogging("ERROR: Error happened while saving the file  " + pathSave);
        return false;
    }
    ThreadUtil::threadSafeLogging("SUCCESS: " + pathLoad);
    return true;

}

}

