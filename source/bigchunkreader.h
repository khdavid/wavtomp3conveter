#ifndef _BIGCHUNKREADER_H_
#define _BIGCHUNKREADER_H_
#include <istream>
#include <vector>
#include <cstdint>

class BigChunkReader
{
public:
    BigChunkReader( std::istream& stream, int32_t bufSize, int32_t chunkSize );
    //return empty vector if the chunkSize is exceeded
    const std::vector<char>& readPortion();
private:
    std::istream& stream_;
    const int32_t bufSize_ = 0;
    int32_t chunkSize_ = 0;
    std::vector<char> buffer_;

    int32_t getNumBytesToRead_() const;
    void reduceChunkSize_(int32_t size);
};
#endif // _BIGCHUNKREADER_H_