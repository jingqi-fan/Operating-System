#include <iostream>
#include <ctime>
#include <cmath>
#include "VMemoryManagement.h"

using namespace std;
#define BASICSIZE 128
#define OPERATIONCOUNT 20  // Number of operations per test case

int main() {
    srand(static_cast<unsigned>(time(nullptr)));  // Set the random seed

    // Define the different strategies
    AllocationStrategy strategies[] = {FIRST_FIT, BEST_FIT, WORST_FIT};
    string strategyNames[] = {"First Fit", "Best Fit", "Worst Fit"};

    for (int strategyIndex = 0; strategyIndex < 3; strategyIndex++) {  // Loop through strategies
        AllocationStrategy strategy = strategies[strategyIndex];
        cout << "Testing with " << strategyNames[strategyIndex] << " strategy." << endl;

        for (int i = 0; i < 4; i++) {  // Loop through different page sizes
            int ProcessID = 0;
            VMemoryManagement *sys = new VMemoryManagement(BASICSIZE * static_cast<int>(pow(2, i)), strategy);
            cout << "This is a case with page size as " << BASICSIZE * static_cast<int>(pow(2, i)) << " using " << strategyNames[strategyIndex] << ":" << endl;
            cout << "PageSize: " << sys->partitionSize << endl;
            cout << "PageCount: " << sys->partitionCount << endl;
            cout << endl;

            for (int j = 0; j < OPERATIONCOUNT; j++) {
                int CreateOrDelete = rand() % 2;  // Randomly decide whether to create or delete a process
                if (CreateOrDelete == 1) {
                    int ProcessAllocation = rand() % 1023 + 2;  // Random size from 2 - 1024
                    Process *p = new Process(ProcessID++, ProcessAllocation);
                    cout << "Create a process ID: " << p->id << ", Allocation Size: " << p->requiredSize << endl;
                    sys->allcoAProcess(p);  // Do the request for memory
                } else {
                    if (!sys->pVec.empty()) {
                        Process *p = sys->pVec[rand() % sys->pVec.size()];  // Randomly select a process to delete
                        cout << "Delete a process ID: " << p->id << ", Allocation Size: " << p->requiredSize << endl;
                        sys->releaseAProcess(p);  // Do the release for a process
                    } else {
                        cout << "No Process to delete, skip!" << endl;
                    }
                }
                cout << "There are " << sys->pVec.size() << " processes in the system:" << endl;
                for (auto &proc : sys->pVec) {
                    cout << "Process: " << proc->id << endl;
                    for (const auto &entry : proc->pTable.pageMap) {
                        cout << "PageID:" << entry.first << ", Allocation: 0 -- " << entry.second << endl;
                    }
                }
                cout << endl;
            }
            delete sys;  // Clean up memory
        }
        cout << "-----------------------------------" << endl;
    }

    system("pause");  // Pause the console window
    return 0;
}
