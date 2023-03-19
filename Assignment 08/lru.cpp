#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cstdlib>
using namespace std;

void LRU(int pages, int frames, vector<int> requests)
{
    vector<int> pageTable(pages, -1); // array of size eq to num of addressable pages
    vector<int> pageList; // to store pages in order of their use
    int pageFaults = 0;
    int hit = 0;
    // cout << "Total Requests: "<< requests.size() << endl;
    for (int i = 0; i < requests.size(); i++)
    {
        int page = requests[i] - 1;
        // cout << "Requesting " << page+1 <<" ...";
        if (pageTable[page] == -1)
        {
            pageFaults++;
            // cout << "FAULT";
            if (pageList.size() == frames) // no empty slots left
            {
                int victimPage = pageList.back();
                pageTable[victimPage] = -1; // eviction by lru policy
                pageList.pop_back(); // evict
            }
            pageList.insert(pageList.begin(), page); // insert at beg
            pageTable[page] = 1;
        } else {
            // cout << "HIT";
            hit++;
            pageList.erase(find(pageList.begin(), pageList.end(), page));
            pageList.insert(pageList.begin(), page);
        }
        // cout << endl;
    }
    cout << "LRU Page Faults: " << pageFaults << endl;
    cout << "LRU Page Hits: " << hit << endl;
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
    while (infile >> page) {
        requests.push_back(page);
    }
    infile.close();
    LRU(pages, frames, requests);
    return 0;
}
