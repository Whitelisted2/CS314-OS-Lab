#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <unordered_map>
#include <cstring>

using namespace std;

const int MAXN = 1 << 21;
int U, L;
int freeBlocks[MAXN];
bool inUse[MAXN];
unordered_map<char, int> processMap;
vector<pair<char, int>> processes;

void printBuddy() {
    bool inBlock = false;
    int blockStart = 0;
    for (int i = L; i <= U; ++i) {
        if (!inUse[i] && !inBlock) {
            inBlock = true;
            blockStart = i;
        }
        else if (inUse[i] && inBlock) {
            inBlock = false;
            int blockSize = i - blockStart;
            cout << "Free Block :" << blockSize << endl;
        }
    }
    if (inBlock) {
        int blockSize = (1 << U) - blockStart;
        cout << "Free Block :" << blockSize << endl;
    }

    for (const auto& p : processes) {
        cout << p.first << " : " << p.second << endl;
    }
}

void freeBuddy(int index) {
    while (index > 0 && freeBlocks[index ^ 1] == index) {
        index &= index ^ 1;
        freeBlocks[index] = index ^ 1;
        index >>= 1;
    }
    freeBlocks[index] = index;
}

int allocateBuddy(int size) {
    int level = U;
    while (freeBlocks[level] == -1 || (1 << level) < size) {
        if (--level < L) {
            return -1;
        }
    }

    int index = freeBlocks[level];
    freeBlocks[level] = freeBlocks[index];
    inUse[index] = true;
    while (index > 0) {
        index >>= 1;
        freeBlocks[index] = (freeBlocks[index << 1] == freeBlocks[(index << 1) | 1]) ? freeBlocks[index << 1] : -1;
    }

    return index;
}

int main() {
    int t;
    cin >> t;
    while (t--) {
        cin >> U >> L;
        memset(freeBlocks, -1, sizeof(freeBlocks));
        memset(inUse, false, sizeof(inUse));
        processMap.clear();
        processes.clear();

        int initialSize = 1 << U;
        freeBlocks[L] = 0;
        for (int i = L; i < U; ++i) {
            freeBlocks[i + 1] = freeBlocks[i] + (initialSize >> i);
        }

        queue<char> terminationQueue;
        char process;
        int size;
        while (cin >> process >> size) {
            if (size == 0) {
                terminationQueue.push(process);
            }
            else {
                int index = allocateBuddy(size);
                processMap[process] = index;
                processes.push_back({process, size});
            }

            while (!terminationQueue.empty() && processMap.find(terminationQueue.front()) != processMap.end()) {
                int index = processMap[terminationQueue.front()];
                freeBuddy(index);
                inUse[index] = false;
                terminationQueue.pop();
            }

            if (terminationQueue.empty()) {
                printBuddy();
                cout << endl;
            }
        }
    }

    return 0;
}
