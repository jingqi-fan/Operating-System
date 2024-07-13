//
// Created by z on 2024/5/26.
//

#ifndef LAB3MEMORY_OWN_VMEMORYMANAGEMENT_H
#define LAB3MEMORY_OWN_VMEMORYMANAGEMENT_H


#include<iostream>
#include<map>
#include<vector>
#include <cstring>
#include "Process.h"
using namespace std;

enum AllocationStrategy {
    FIRST_FIT,
    BEST_FIT,
    WORST_FIT
};

class VMemoryManagement{
public:
    char *totalsize = NULL;                 // actual memory space, I take BLIPS as one byte, with is presented by char, '0' means not occupied, '1' means occupied
    int partitionSize = 1024;               // size for a page, default 1024
    int partitionCount = 100;               // page number, default 102400 / 1024 = 100
    vector<Process *>pVec;                  // a vector for processes registered in this VMemoryManagement
    vector<bool>isAllocVec;                 // signal for whether a page is occupied, iterator refers to pageID

    AllocationStrategy strategy = FIRST_FIT;  // Default strategy
    VMemoryManagement(int size, AllocationStrategy strat = FIRST_FIT) : partitionSize(size), strategy(strat) {
        totalsize = (char *)malloc(sizeof(char) * 102400);
        partitionCount = 102400 / partitionSize;
        memset(totalsize, '0', 102400 * sizeof(char));
        isAllocVec.resize(partitionCount, false);
    }

    ~VMemoryManagement() {
        free(totalsize);
        for (auto p : pVec) {
            delete p;
        }
    }

    VMemoryManagement(){
        totalsize = (char *)malloc(sizeof(char) * 102400);          // malloc for the memory space, and set all char as '0' which means not occupied
        memset(totalsize, '0', 102400 * sizeof(char));
    }

    int allocAPage() {
        // Depending on strategy, allocate a page
        switch (strategy) {
            case BEST_FIT:
                return allocAPageBestFit();
            case WORST_FIT:
                return allocAPageWorstFit();
                return -1;
            case FIRST_FIT:
            default:
                for (int i = 0; i < isAllocVec.size(); i++) {
                    if (!isAllocVec[i]) {
                        isAllocVec[i] = true;
                        return i;
                    }
                }
                return -1;  // No page available
        }
    }

    void allcoAProcess(Process * p){                    // do the action for derive spaces for a process
        int pageNum = p->requiredSize / partitionSize + 1;      // get the number of pages of this process
        int requiredSize = p->requiredSize;
        map<int, int>retMap;
        for (int i = 0; i < pageNum; i++){
            if (requiredSize > partitionSize){          // if this page is all used, set offset as the size of page
                int pageID = allocAPage();
                if (pageID == -1) {
                    cout << "No enough room for Process " << p->id << endl;
                    releaseAProcess(p);                 // rollback
                    return;
                }
                p->addPage(pageID, partitionSize);
                for (int index = 0; index < partitionSize; index++){            // set the used space as '1'
                    *(totalsize + pageID * partitionSize + index) = '1';
                }
                requiredSize -= partitionSize;
            }
            else{                                        // if this page is partial used, set offset as the used size of page
                int pageID = allocAPage();
                if (pageID == -1) {
                    cout << "No enough room for Process " << p->id << endl;
                    releaseAProcess(p);  // rollback
                    return;
                }
                p->addPage(pageID, requiredSize);
                for (int index = 0; index < requiredSize; index++){  // set the used space as '1'
                    *(totalsize + pageID * partitionSize + index) = '1';
                }
            }
        }
        pVec.push_back(p);
    }
    void releaseAProcess(Process *p){//release a process
        for (map<int, int>::iterator it = p->pTable.pageMap.begin(); it != p->pTable.pageMap.end(); it++){  //change the signal for each occupied page, and change the memory space to '0'
            isAllocVec[it->first] = false;
            for (int i = 0; i < it->second; i++){
                *(totalsize + it->first * partitionSize + i) = '0';
            }
        }
        for (vector<Process *>::iterator it = pVec.begin(); it != pVec.end(); it++){            //delete this process
            if (*it == p){
                it = pVec.erase(it);
                break;
            }
        }
        delete(p);
    }

    int allocAPageBestFit() {
        int index = -1;
        int minSize = INT_MAX;  // Initialize to the maximum possible value to ensure any smaller valid size is considered

        for (int i = 0; i < isAllocVec.size(); i++) {
            // Check if the block is free and the size is less than the current minimum
            if (!isAllocVec[i] && (partitionSize < minSize)) {
                minSize = partitionSize;
                index = i;
            }
        }

        // If a suitable block is found, mark it as occupied
        if (index != -1) {
            isAllocVec[index] = true;
        }
        return index;  // Return the index of the allocated block or -1 if no block was suitable
    }


    int allocAPageWorstFit() {
        int index = -1;
        int maxSize = 0;  // Start with the smallest possible size to ensure any larger valid size is considered

        for (int i = 0; i < isAllocVec.size(); i++) {
            // Check if the block is free and if its size is greater than the current maximum found
            if (!isAllocVec[i] && (partitionSize > maxSize)) {
                maxSize = partitionSize;
                index = i;
            }
        }

        // If a suitable block is found, mark it as occupied
        if (index != -1) {
            isAllocVec[index] = true;
        }
        return index;  // Return the index of the allocated block or -1 if no suitable block was found
    }



};


#endif //LAB3MEMORY_OWN_VMEMORYMANAGEMENT_H
