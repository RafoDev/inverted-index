#include <iostream>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>
#include <set>
#include <chrono>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <mutex>

using namespace std;
namespace fs = std::filesystem;

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

vector<unordered_map<string, int>> counter;
unordered_map<string, int> mergedCounter;

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

typedef long long ll;
struct Indexes
{
	ll begin;
	ll end;
	int mapIndex;
	ifstream filePointer;
	Indexes()
	{
		begin = 0;
		end = 0;
	}
};

ll getValidEndOfWord(ifstream &filePointer, ll endPos)
{
	char tmpChar;
	filePointer.seekg(endPos, filePointer.beg);
	while (filePointer.get(tmpChar))
	{
		if (tmpChar == ' ')
			return endPos;
		endPos++;
	}
	return endPos;
}

void *countWorker(void *arg)
{
	struct Indexes *args = (struct Indexes *)arg;
	// cout << "worker: " << args->begin << " " << args->end << "\n";

	ifstream *partialPointer = &args->filePointer;

	string currWord;
	for (ll pos = args->begin; pos <= args->end; pos++)
	{
		char tmpChar;
		partialPointer->get(tmpChar);

		if (tmpChar != ' ')
		{
			currWord += tmpChar;
		}
		else if (tmpChar == ' ' && !currWord.empty())
		{
			counter[args->mapIndex][currWord]++;
			currWord.clear();
		}
	}
	if (!currWord.empty())
		counter[args->mapIndex][currWord]++;

	return NULL;
}

void count(string &filename)
{
	ifstream file(filename, ios::binary | ios::ate);

	if (!file.is_open())
	{
		return;
	}
	file.seekg(0, file.end);
	ll fileSize = file.tellg();
	file.seekg(0, file.beg);

	int nThreads = sysconf(_SC_NPROCESSORS_ONLN);
	int chunkSize = ceil(float(fileSize) / nThreads);

	vector<Indexes> indexes(nThreads);
	vector<ifstream> filePointers(nThreads);

	file.seekg(0);

	for (int i = 0, j = 0; i < nThreads; i++)
	{
		indexes[i].begin = j;
		indexes[i].mapIndex = i;
		indexes[i].filePointer.open(filename, ios::binary);
		indexes[i].filePointer.seekg(j);
		int tmpEnd = j + chunkSize;
		ll validEnd = getValidEndOfWord(file, tmpEnd);
		indexes[i].end = validEnd;
		j = validEnd + 1;
	}

	indexes[nThreads - 1].end = fileSize - 1;
	pthread_t threads[nThreads];

	counter.resize(nThreads);

	for (int i = 0; i < nThreads; i++)
	{
		pthread_create(&threads[i], NULL, countWorker, (void *)&indexes[i]);
	}
	for (int j = 0; j < nThreads; j++)
		pthread_join(threads[j], NULL);

	for (const auto &map : counter)
	{
		for (const auto &entry : map)
			mergedCounter[entry.first] += entry.second;
	}

	// for (const auto &entry : mergedCounter)
	// 	cout << entry.first << ": " << entry.second << endl;
}

int main()
{
	string dir = "../files";
	unordered_map<string, vector<int>> invIndex;
	vector<string> filenames = getFilenames(dir);

	Timer totalTime, fileTime, countingTime, indexingTime;
	int nFiles = filenames.size();
	for (int i = 0; i < nFiles; i++)
	{
		fileTime.Reset();
		// cout << "**" << filenames[i] << '\n';

		ifstream file(filenames[i]);
		set<string> words;
		string currWord;

		countingTime.Reset();
		// while (file >> currWord)
		// 	words.insert(currWord);
		count(filenames[i]);
		// cout << "Counting Time: " << countingTime.Elapsed() << '\n';

		indexingTime.Reset();
		for (auto &word : mergedCounter)
		{
			invIndex[word.first].push_back(i);
		}
		// cout << "Indexing Time: " << indexingTime.Elapsed() << '\n';
		// cout << "File time: " << fileTime.Elapsed() << '\n';
		// cout << '\n';
	}

	if (invIndex.empty())
		cout << "VACIO\n";

	// for (const auto &entry : invIndex)
	// {
	// 	cout << entry.first << " : ";
	// 	for (const auto &file : entry.second)
	// 		cout << file << " ";
	// 	cout << '\n';
	// }
	cout << "Total time: " << totalTime.Elapsed() << '\n';
}