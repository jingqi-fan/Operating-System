#include <iostream>
#include <vector>
#include <algorithm>
#include "filesystem.h"

using namespace std;

// Initialize root and current directory pointers.
Entry* root = nullptr;
Entry* curDir = nullptr;

// Set the username and hostname for the shell prompt.
const string USERNAME = "username";
const string HOSTNAME = "hostname";

int main() {
    // Create the root directory and set it as the current directory.
    root = new Entry(false, "root", nullptr);
    curDir = root;

    std::string line;
    while (true) {
        // Display the command prompt.
        cout << USERNAME << "@" << HOSTNAME << ":" << getCurrentPath() << "$ ";
        getline(cin, line); // Read a line from user input.
        vector<string> arr = split(line, ' '); // Split the input based on spaces.
        string cmd = arr[0]; // Extract the command from the input.

        // Process commands.
        if(cmd == "pwd"){
            pwd(); // Print the current directory path.
        } else if(cmd == "cd"){
            cd(arr[1]); // Change the current directory.
        } else if(cmd == "ls"){
            ls(""); // List contents of the current directory.
        } else if(cmd == "mkdir"){
            mkdir(arr[1]); // Create a new directory.
        } else if(cmd == "rmdir"){
            rmdir(arr[1]); // Remove a directory.
        } else if(cmd == "touch"){
            touch(arr[1]); // Create a new file.
        } else if(cmd == "rm"){
            rm(arr[1]); // Remove a file.
        } else if(cmd == "exit" || cmd == "quit"){
            return 0; // Exit the program.
        } else {
            cout << "unknown command!" << endl; // Handle unknown commands.
        }
    }

    return 0;
}
