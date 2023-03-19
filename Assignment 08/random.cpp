#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cstdlib>
#include <ctime>
using namespace std;

void Random(int pages, int frames, vector<int> requests)
{
    vector<int> pageTable(pages, -1);
    int pageFaults = 0;
    int hit = 0;
    srand(time(NULL));
    for (int i = 0; i < requests.size(); i++)
    {
        int page = requests[i];
        if (pageTable[page] == -1)
        {
            pageFaults++;
            if (count(pageTable.begin(), pageTable.end(), 1) == frames)
            {
                int victimPage = rand() % pages;
                while (pageTable[victimPage] != 1)
                {
                    victimPage = rand() % pages;
                }
                pageTable[victimPage] = -1;
            }
            pageTable[page] = 1;
        } else {
            hit++;
        }
    }
    cout << "Random Page Faults: " << pageFaults << endl;
    cout << "Random Page Hits: " << hit << endl;
}

int main(int argc, char *argv[])
{
    int pages = atoi(argv[1]);
    int frames = atoi(argv[2]);
    int blocks = atoi(argv[3]);
    string filename = argv[4];
    vector<int> requests;
    ifstream infile(filename);
    int page;
    while (infile >> page)
    {
        requests.push_back(page);
    }
    infile.close();
    Random(pages, frames, requests);
    return 0;
}
