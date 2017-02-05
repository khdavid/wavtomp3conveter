#include "bitspersampleutil.h"
#include <vector>
#include <stdlib.h>

namespace
{
std::vector<int16_t> convert8bit(const std::vector<char>& buffer)
{
    std::vector<int16_t> res;
    res.reserve( buffer.size() );
    for (const auto& element : buffer)
    {
        auto element16b = (const int16_t *) reinterpret_cast<const unsigned char*>(&element); 
        res.push_back((*element16b - 128) * 256); // 128 corresponds to 0 amplitude. 
    }
    return res;
}
std::vector<int16_t> convert16bit(const std::vector<char>& buffer)
{
    auto size = buffer.size() / 2;
    auto firstEl = reinterpret_cast<const int16_t*>(&buffer[0]);
    std::vector<int16_t> res(firstEl, firstEl + size);
    return res;
}

std::vector<int16_t> convert24bit( const std::vector<char>& buffer )
{
    std::vector<int16_t> res;
    res.reserve( buffer.size() / 3 );
    for ( size_t ind = 0; ind < buffer.size(); ind += 3 )
    {
        res.push_back( *reinterpret_cast<const int16_t*>( &buffer[ind + 1] ) );
    }
    return res;
}

std::vector<int16_t> convert32bit( const std::vector<char>& buffer )
{
    std::vector<int16_t> res;
    res.reserve( buffer.size() / 4 );
    for ( size_t ind = 0; ind < buffer.size(); ind += 4 )
    {
        res.push_back( *reinterpret_cast<const int16_t*>( &buffer[ind + 2] ) );
    }
    return res;
}


}
namespace BitsPerSampleUtil
{
bool isValidBitsPerSample(int16_t bitsPerSample)
{
    return (bitsPerSample == 8) ||
        (bitsPerSample == 16) ||
        (bitsPerSample == 24) ||
        (bitsPerSample == 32);
}
std::vector<int16_t> convert(const std::vector<char>& buffer, int16_t bitsPerSample)
{
    switch (bitsPerSample)
    {
    case 8: return convert8bit(buffer);
    case 16: return convert16bit( buffer );
    case 24: return convert24bit( buffer );
    case 32: return convert32bit( buffer );
    default: return{};
    };
}


}
