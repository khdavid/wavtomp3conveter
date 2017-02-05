#include "threadutil.h"
#include "pthread.h"
#include <iostream>
using namespace std;
namespace
{
pthread_mutex_t gVectorMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t gLoggingMutex = PTHREAD_MUTEX_INITIALIZER;

}
namespace ThreadUtil
{

bool threadSafePopFileName(vector<string>& fileNamesVector, std::string& fileName)
{
    pthread_mutex_lock(&gVectorMutex);
    auto size = fileNamesVector.size();
    bool result = false;
    if (size)
    {
        fileName = fileNamesVector.back();
        fileNamesVector.pop_back();
        result = true;
    }
    pthread_mutex_unlock(&gVectorMutex);
    return result;
}

void threadSafeLogging(const string& message)
{
    pthread_mutex_lock(&gLoggingMutex);
    cout << message << endl;
    pthread_mutex_unlock(&gLoggingMutex);

}




}