//
// Created by z on 2024/5/26.
//

#ifndef LAB3MEMORY_OWN_PAGETABLE_H
#define LAB3MEMORY_OWN_PAGETABLE_H

#include<iostream>
#include<map>
#include<vector>
using namespace std;
class PageTable{
public:
    map<int, int>pageMap;           // a map for keep the information of occupied pages in memory, first refers to pageID, second refers to offset in that page
    void addPage(int pageNum, int offset){          // add a occupied page in this page table
        pageMap.insert(pair<int, int>(pageNum, offset));
    }
    void removePage(int pageNum){                   // delete a occupied page in this page table
        pageMap.erase(pageNum);
    }
};


#endif //LAB3MEMORY_OWN_PAGETABLE_H
