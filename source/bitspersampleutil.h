#ifndef bitspersampleutil_h__
#define bitspersampleutil_h__
#include <cstdint>
#include <vector>
namespace BitsPerSampleUtil
{

bool isValidBitsPerSample(int16_t bitsPerSample);
std::vector<int16_t> convert(const std::vector<char>& buffer, int16_t bitsPerSample);


}
#endif // bitspersampleutil_h__