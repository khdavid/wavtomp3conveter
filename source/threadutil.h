#ifndef threadutil_h__
#define threadutil_h__
#include <string>
#include <vector>
namespace ThreadUtil
{
//!returns false if gAllFileNames is empty;
bool threadSafePopFileName(std::vector<std::string>& fileNamesVector, std::string& fileName);
void threadSafeLogging(const std::string& message);

};

#endif // threadutil_h__
