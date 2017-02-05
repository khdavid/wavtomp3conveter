#include "filesindirectoryfinder.h"
#include <algorithm>
using namespace std;
//unfortunately in a C++ standard there is not a way to find all files in a folder, so different
//code for windows and linux is needed.
namespace
{
bool isWavFile(string fileName)
{
    std::transform(fileName.begin(), fileName.end(), fileName.begin(), ::tolower);
    string wavString = ".wav";
    const auto wSize = wavString.size();
    if(fileName.size() < wSize)
    {
        return false;
    }
    const auto fNameSize = fileName.size();
    bool res = true;
    for(size_t i = 1; i <= wSize;++i)
    {
        res = res && (wavString[wSize-i] == fileName[fNameSize-i]);
    }
    return res;
}
std::vector<string> getWavFiles(const std::vector<string>& fileNames)
{
    std::vector<string> result;
    for(auto fileName: fileNames)
    {
        if(isWavFile(fileName))
        {
            result.push_back(fileName);
        }
    }
    return result;
}
}
namespace FilesInDirectoryFinder
{

#ifdef __linux__ 
#include "dirent.h"
 //linux
    vector<string> getAllWavFiles(const string& folder)
    {
      vector<string> result;
      DIR *dir;
      dirent *ent;
      if ((dir = opendir (folder.c_str())) != nullptr) 
      {
        while ((ent = readdir (dir)) != nullptr) 
        {
            result.push_back(ent->d_name);
        }
      }
      closedir (dir);
      return getWavFiles(result);
    }

#elif _WIN32
//windows
#include <Windows.h>
vector<string> getAllWavFiles(const string& folder)
{
    vector<string> names;
    string searchPath = folder + "/*.wav";
    WIN32_FIND_DATA fd;
    HANDLE hFind = FindFirstFile(searchPath.c_str(), &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
            {
                std::string fileName = fd.cFileName;
                names.push_back(fileName);
            }
        } while (FindNextFile(hFind, &fd));
        FindClose(hFind);
    }
    return names;
}
#else

#endif

}