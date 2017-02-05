#include <iostream>
#include "lame.h"
#include <stdio.h>
#include "wavtomp3.h"
#include "filesindirectoryfinder.h"
#include <thread>
#include <pthread.h>
#include "threadutil.h"
#include <algorithm>
using namespace std;
namespace
{
vector<string> gAllWavFiles;
struct ConvertContainer
{
    vector<string> allFileNames;
    string folder;
};

std::string extractFolderName(char **argv)
{
    string folder = string(argv[1]);
    std::replace(begin(folder), end(folder), '\\', '/');
    return folder;
}

std::string removeExtension(std::string fileName)
{
    for (int i = 0; i < 4; i++)
    {
        fileName.pop_back();
    }
    return fileName;
}

void* convert(void* args)
{
    auto container = static_cast<ConvertContainer*>(args);
    string fileName;
    while (ThreadUtil::threadSafePopFileName(container->allFileNames, fileName))
    {
        auto pathWav = container->folder + "/" + fileName;
        auto pathMp3 = removeExtension(pathWav) + ".mp3";
        WavToMP3::convert(pathWav, pathMp3);
    }
    return nullptr;
}

}
int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        cout << "Please specify the folder as a first command line argument" << endl;
        return 0;
    }
    const string cFolder = extractFolderName(argv);
    int concurentThreadsSupported = std::max(1, (int)std::thread::hardware_concurrency());
    cout << "Number of cores detected: " << concurentThreadsSupported << endl;
    std::vector<pthread_t> threads(concurentThreadsSupported);
    auto allFileNames = FilesInDirectoryFinder::getAllWavFiles(cFolder);
    if (allFileNames.empty())
    {
        cout << "Can't find .wav files in the folder " << cFolder << endl;
    }
    ConvertContainer convertContainer = { allFileNames,cFolder };
    for (auto& thread : threads)
    {
        pthread_create(&thread, nullptr, &convert, &convertContainer);
    }
    for (auto& thread : threads)
    {
        pthread_join(thread, {});
    }
    cout << "Convertion is finished";
    return 0;

}