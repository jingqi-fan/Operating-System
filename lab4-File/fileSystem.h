//
// Created by z on 2024/5/28.
//

#ifndef LAB4FILE_OWN_FILESYSTEM_H
#define LAB4FILE_OWN_FILESYSTEM_H


#include <string>
#include <vector>
#include <ctime>

struct Entry {
    Entry(bool isfile, std::string name, Entry* parent);
    std::string name;
    bool isfile;
    std::vector<Entry*>* subentries;
    Entry* parent;
    unsigned int permission;
    unsigned int size;
    time_t lastModify;
};

extern Entry* root;
extern Entry* curDir;

void freeEntry(Entry* entry);
Entry* findEntryByName(Entry* parent, const std::string& name);
std::vector<std::string> split(const std::string& text, char sep);
bool hasWritePermission(unsigned int permission);
bool hasReadPermission(unsigned int permission);
std::string permissionstr(unsigned int a);
std::string permissionstring(unsigned int permission);
void ls(const std::string& options);
void pwd();
void cd(const std::string& dirname);
void mkdir(const std::string& dirname);
void rmdir(const std::string& dirname);
void touch(const std::string& filename);
void rm(const std::string& filename);
std::string getCurrentPath(); // New function to get current path

#endif //LAB4FILE_OWN_FILESYSTEM_H
