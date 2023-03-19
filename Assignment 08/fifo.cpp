#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cstdlib>
#include <ctime>
using namespace std;

void FIFO(int pages, int frames, vector<int> requests)
{
    vector<int> pageTable(pages, -1); // array of size eq to num of addressable pages
    queue<int> pageQueue; // to store pages in queue, i.e. which are in main memory
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
            if (pageQueue.size() == frames) // no empty slots left
            {
                int victimPage = pageQueue.front();
                pageTable[victimPage] = -1; // eviction by fifo policy
                pageQueue.pop();
            }
            pageQueue.push(page);
            pageTable[page] = 1;
        } else {
            // cout << "HIT";
            hit++;
        }
        // cout << endl;
    }
    cout << "FIFO Page Faults: " << pageFaults << endl;
    cout << "FIFO Page Hits: " << hit << endl;
    
}

// hits: 5 12 16 for 10 20 30
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
    FIFO(pages, frames, requests);
    return 0;
}
