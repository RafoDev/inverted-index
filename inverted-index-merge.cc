#include <iostream>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>
#include <set>
#include <chrono>
#include <math.h>
#include <pthread.h>
#include <mutex>
#include <unistd.h>

using namespace std;
namespace fs = std::filesystem;

unordered_map<string, set<int>> invIndexMerged;
vector<unordered_map<string, set<int>>> invIndexes;

vector<string> getFilenames(string &dir)
{
    vector<string> filenames;
    int c = 0;
    fs::path dirPath(dir);
    if (fs::exists(dirPath) && fs::is_directory(dirPath))
    {
        for (const auto entry : fs::directory_iterator(dirPath))
        {
            if (fs::is_regular_file(entry))
                filenames.push_back(entry.path().generic_u8string());
        }
    }
    return filenames;
}

class Timer
{
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;

public:
    Timer() : start_time_(std::chrono::high_resolution_clock::now()) {}

    void Reset()
    {
        start_time_ = std::chrono::high_resolution_clock::now();
    }

    double Elapsed()
    {
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end_time - start_time_;
        return duration.count();
    }
};

struct File
{
    int i;
    ifstream pointer;

    File(int _i, string &filename)
    {
        i = _i;
        pointer.open(filename);
    }
    File(){};
};

mutex indexMtx;

void *indexer(void *arg)
{
    struct File *args = (struct File *)arg;

    ifstream *partialPointer = &args->pointer;
    int i = args->i;

    string currWord;
    while (*partialPointer >> currWord)
        invIndexes[i][currWord].insert(i);

    return NULL;
}

int main()
{
    string dir = "../files";
    vector<string> filenames = getFilenames(dir);

    int nThreads = sysconf(_SC_NPROCESSORS_ONLN);
    pthread_t threads[nThreads];
    invIndexes.resize(nThreads);

    Timer fileTime, totalTime, countingTime, indexingTime;
    int nFiles = filenames.size();
    vector<File> files(nFiles);

    for (int i = 0; i < nFiles; i++)
    {
        files[i].i = i;
        files[i].pointer.open(filenames[i]);
    }

    for (int i = 0; i < nFiles; i++)
        pthread_create(&threads[i], NULL, indexer, (void *)&files[i]);

    for (int i = 0; i < nFiles; i++)
        pthread_join(threads[i], NULL);

    for (const auto &map : invIndexes)
    {
        for (const auto &entry : map)
            invIndexMerged[entry.first].insert(entry.second.begin(), entry.second.end());
    }
    
    // for (const auto &entry : invIndexMerged)
    // {
    //     cout << entry.first << " : ";
    //     for (const auto &file : entry.second)
    //         cout << file << " ";
    //     cout << '\n';
    // }

    cout << "Total time: " << totalTime.Elapsed() << '\n';
}