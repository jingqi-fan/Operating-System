//
// Created by z on 2024/5/26.
//

#ifndef LAB3MEMORY_OWN_PROCESS_H
#define LAB3MEMORY_OWN_PROCESS_H


#include<iostream>
#include<map>
#include "PageTable.h"
using namespace std;
class Process{
public:
    int id;             //Process ID
    PageTable pTable;       // a page table for this process
    int requiredSize;           // the occupied memory size of this process
    Process(int proID, int size){
        id = proID;
        requiredSize = size;
    }
    void addPage(int pageNum, int offset){      // add a page for this process
        pTable.addPage(pageNum, offset);
    }
    void delPage(int pageNum){                  // delete a page for this process
        pTable.removePage(pageNum);
    }
};

#endif //LAB3MEMORY_OWN_PROCESS_H
