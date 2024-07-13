//
// Created by z on 2024/5/28.
//
#include "filesystem.h"
#include <iostream>
#include <algorithm>
using namespace std;

// Constructor for the Entry class to initialize file or directory entries.
Entry::Entry(bool isfile, std::string name, Entry* parent) : isfile(isfile), name(name), parent(parent) {
    subentries = new std::vector<Entry*>();
    if (parent != NULL) {
        parent->subentries->push_back(this);
    }
    permission = 766; // Default permission settings
    size = isfile ? 1024 : 512; // Default size for file or directory
}

// Recursively frees memory allocated for an Entry and its subentries.
void freeEntry(Entry* entry) {
    if (!entry) return;
    for (auto subentry : *entry->subentries) {
        freeEntry(subentry);
    }
    delete entry->subentries;
    delete entry;
}

// Finds an Entry by name under a given parent directory.
Entry* findEntryByName(Entry* parent, const std::string& name) {
    for (auto& entry : *parent->subentries) {
        if (entry->name == name) return entry;
    }
    return nullptr;
}

// Splits a string by a given separator and returns a vector of substrings.
std::vector<std::string> split(const std::string &text, char sep) {
    std::vector<std::string> tokens;
    std::size_t start = 0, end = 0;
    while ((end = text.find(sep, start)) != std::string::npos) {
        tokens.push_back(text.substr(start, end - start));
        start = end + 1;
    }
    tokens.push_back(text.substr(start));
    return tokens;
}

// Checks if the write permission is granted.
bool hasWritePermission(unsigned int permission) {
    return (permission / 100) % 10 >= 2;
}

// Checks if the read permission is granted.
bool hasReadPermission(unsigned int permission) {
    return (permission / 100) % 10 >= 4;
}

// Converts a permission number to a human-readable string (rwx format).
std::string permissionstr(unsigned int a) {
    return std::string() + (a >= 4 ? 'R' : '-') + (a % 4 >= 2 ? 'W' : '-') + (a % 2 >= 1 ? 'X' : '-');
}

// Generates a full permission string for user, group, and others.
std::string permissionstring(unsigned int permission) {
    return permissionstr(permission / 100) + permissionstr(permission / 10 % 10) + permissionstr(permission % 10);
}

// Lists the contents of the current directory with optional detailed format.
void ls(const std::string& options) {
    if (options.empty()) { // Simple listing
        for (auto& entry : *curDir->subentries) {
            cout << entry->name << endl;
        }
    } else if (options == "-l") { // Detailed listing
        for (auto& entry : *curDir->subentries) {
            char timestr[64];
            strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", localtime(&entry->lastModify));
            cout << (entry->isfile ? "-" : "d") << " " << permissionstring(entry->permission) << " " << entry->subentries->size() << " " << entry->size << " " << timestr << " " << entry->name << endl;
        }
    } else {
        cout << "unknown parameter " << options << endl;
    }
}

// Prints the current working directory path and lists contents.
void pwd() {
    vector<string> path;
    for (Entry* temp = curDir; temp != root; temp = temp->parent) {
        path.push_back(temp->name);
    }
    reverse(path.begin(), path.end());
    cout << "/";
    for (const auto& p : path) {
        cout << p << "/";
    }
    cout << endl;
    ls(""); // List current directory
}

// Changes the current working directory.
void cd(const std::string& dirname) {
    if (dirname == ".." && curDir->parent) { // Go up one level
        curDir = curDir->parent;
    } else {
        Entry* entry = findEntryByName(curDir, dirname);
        if (!entry) {
            cout << "couldn't find directory " << dirname << endl;
        } else if (!entry->isfile && hasReadPermission(entry->permission)) {
            curDir = entry;
        } else {
            cout << "Permission denied or not a directory" << endl;
        }
    }
}

// Implementation for creating a directory (mkdir command).
void mkdir(const std::string& dirname) {
    Entry* entry = findEntryByName(curDir, dirname);
    if(entry == NULL) { // No directory or file with the same name exists.
        if(hasWritePermission(curDir->permission)) { // Check for write permission.
            Entry* t = new Entry(false, dirname, curDir); // Create a new directory entry.
            time_t timep;
            time(&timep);
            t->lastModify = timep; // Set the modification time to current.
        } else {
            cout << "Permission denied" << endl; // Deny action if no write permission.
        }
    } else {
        cout << "entry " << dirname << " exists" << endl; // Notify if the entry already exists.
    }
}

// Implementation for removing a directory (rmdir command).
void rmdir(const std::string& dirname) {
    Entry* entry = findEntryByName(curDir, dirname);
    if(entry == NULL) { // No directory found with the given name.
        cout << "dir " << dirname << " not exist" << endl;
    } else if(entry->isfile) { // Check if the found entry is not a directory.
        cout << "entry " << dirname << " is a file! use rm to remove a file!" << endl;
    } else {
        if(hasWritePermission(entry->permission)) { // Check for write permission.
            // Remove directory entry.
            for (auto it = entry->parent->subentries->begin(); it != entry->parent->subentries->end(); ) {
                if(*it == entry){
                    it = entry->parent->subentries->erase(it); // Remove and update iterator.
                } else{
                    ++it;
                }
            }
            entry->parent = NULL;
            freeEntry(entry); // Free memory allocated for the directory.
        } else {
            cout << "Permission denied" << endl; // Deny action if no write permission.
        }
    }
}

// Implementation for the touch command to create or update a file.
void touch(const std::string& filename) {
    Entry* entry = findEntryByName(curDir, filename);
    if(entry == NULL) {
        if(hasWritePermission(curDir->permission)) { // Check for write permission.
            Entry* t = new Entry(true, filename, curDir); // Create a new file entry.
            time_t timep;
            time(&timep);
            t->lastModify = timep; // Set the modification time to current.
        } else {
            cout << "Permission denied" << endl; // Deny action if no write permission.
        }
    } else { // Notify if the entry already exists.
        cout << "entry " << filename << " exists" << endl;
    }
}

// Implementation for removing a file (rm command).
void rm(const std::string& filename) {
    Entry* entry = findEntryByName(curDir, filename);
    if(entry == NULL) { // No directory or file found with the given name.
        cout << "file " << filename << " not exist" << endl;
    } else if(!entry->isfile) { // Check if the found entry is not a file.
        cout << "entry " << filename << " is a directory! use rmdir to remove a directory!" << endl;
    } else {
        if(hasWritePermission(entry->permission)) { // Check for write permission.
            // Remove file entry.
            for (auto it = entry->parent->subentries->begin(); it != entry->parent->subentries->end(); ) {
                if(*it == entry){
                    it = entry->parent->subentries->erase(it); // Remove and update iterator.
                } else{
                    ++it;
                }
            }
            entry->parent = NULL;
            freeEntry(entry); // Free memory allocated for the file.
        } else {
            cout << "Permission denied" << endl; // Deny action if no write permission.
        }
    }
}

// Returns the current path from the root to the current directory.
std::string getCurrentPath() {
    if (curDir == nullptr) {
        return "/"; // Return root path if current directory is not set.
    }
    std::vector<std::string> paths;
    for (Entry* temp = curDir; temp != root; temp = temp->parent) {
        paths.push_back(temp->name); // Collect all directory names from current to root.
    }
    paths.push_back(root->name); // Include the root directory name.
    std::reverse(paths.begin(), paths.end()); // Reverse to format the path correctly.
    std::string fullPath = "";
    for (const auto& path : paths) {
        fullPath += "/" + path; // Build the full path string.
    }
    return fullPath;
}
