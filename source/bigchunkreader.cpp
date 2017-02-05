#include "bigchunkreader.h"
#include <algorithm>

BigChunkReader::BigChunkReader( std::istream& stream, int32_t bufSize, int32_t chunkSize ) :
stream_( stream ), 
bufSize_( bufSize ), 
chunkSize_( chunkSize )
{
}

int32_t BigChunkReader::getNumBytesToRead_() const
{
    auto res =  std::min( chunkSize_, bufSize_ );
    return res;
}

void BigChunkReader::reduceChunkSize_(int32_t size)
{
    chunkSize_ -= size;
}


const std::vector<char>& BigChunkReader::readPortion()
{
    auto size = getNumBytesToRead_();
    buffer_.resize( size );
    stream_.read( buffer_.data(), bufSize_ );
    auto numReadBytes = (int32_t) stream_.gcount();
    buffer_.resize( numReadBytes );
    return buffer_;
}

