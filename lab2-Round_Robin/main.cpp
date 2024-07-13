#include <iostream>
#include <fstream>
#include <queue>
#include "simProcess.h"
#include <map>

using namespace std;

//Original code was setup to allow FIFO, Round Robin, or Feedback
//enum {
//    S_FIFO,
//    S_RR,
//    S_FB
//};

//select an algorithm for scheduling
//const int ALGORITHM = S_RR;
//int timeslice = 1;
//a counter to count the time which is running,one second in a time slice
int counter = 0;

// Define priority queues
vector<deque<simProcess>> priorityQueues(3);

int main() {
    //setup (double ended) queues for processes (based on state)
    deque<simProcess> newProcs;
    deque<simProcess> readyProcs;
    deque<simProcess> exitProcs;

    simProcess runningProc; //can only be one process running at a time
    bool isRunning = false; //initially nothing is running

    //load simulation data
    unsigned int arrival, duration;


    map<unsigned int, unsigned int> firstResponseTimes;
    map<unsigned int, unsigned int> startTimes;
    double totalResponseTime = 0;



    ifstream fin;
    fin.open("input");

    // Check if the file could not be opened
    if (!fin) {
        cout << "Error opening input file." << endl;
        return -1;  // Return error code if file is not accessible
    }

    string alg;
    int choice;
    cout << "Input the algorithm that you want (FIFO, RR): " << endl;
    cin >> alg; // Read the scheduling algorithm type from user input

    // Determine the scheduling algorithm based on user input
    if (alg == "FIFO") {
        choice = 0;  // Assign 0 for FIFO
    } else if (alg == "RR") {
        choice = 1;  // Assign 1 for Round Robin
    }

    int customTimeslice, timeslice;
    cout << "Enter the length of the time quantum (time slice in units): ";
    cin >> customTimeslice; // Read the length of time slice from user input

    // Check if the input for timeslice is invalid
    if (cin.fail() || customTimeslice <= 0) {
        cout << "Invalid input. Please enter a positive integer." << endl;
        return -1;  // Return error code if input is invalid
    }
    timeslice = customTimeslice;  // Set the timeslice based on user input

    // Read arrival and duration for each process from the file
    while (fin >> arrival) {
        fin >> duration;
        simProcess sp = simProcess(arrival, duration);
        newProcs.push_back(sp); // Add new process to the list of new processes
    }

    fin.close(); // Close the file after reading all data


    //////////////////////////////ready for simulation
    //continue until there are no new or ready processes (everything has finished running)
    unsigned int simTime = 0; //cap at 999999 to prevent accidental infinite loop
    while (!(newProcs.empty() && readyProcs.empty() && !isRunning) && (simTime < 999999)) {
        switch(choice) {
            case 0: //FIFO
                simTime++; //update simulation time
//cout << "Sim time now " << simTime << endl;
                //any arriving processes (per simulation time) can go into the ready queue
                while (simTime == newProcs.front().getArrival()) {
                    readyProcs.push_back(newProcs.front());
                    newProcs.pop_front();
//cout << "Popped from newProcs " << readyProcs.back().getArrival() << endl;
                }
                if (!isRunning && !readyProcs.empty()) { //if no process WAS running, BUT we have one
                    runningProc = readyProcs.front(); //pull first available process
                    readyProcs.pop_front(); //no longer "ready" - it's "running"
                    isRunning = true;
//cout << "Running process " << runningProc.getArrival() << endl;
                }
                if (isRunning) { //"process" one work unit
                    if (runningProc.process()) { //if job has completed, move to finished queue
                        runningProc.setFinished(simTime+1); //finishes *after* this time unit completes
                        exitProcs.push_back(runningProc); //switch queues
                        isRunning = false; //prepare for the next process
/*cout << "Finished process with normalized Turn-around time "
     << exitProcs.back().getNormalizedTurnaround() << endl
     << exitProcs.back().getArrival() << endl
     << exitProcs.back().getFinish() << endl
     << exitProcs.back().getDuration() << endl;
*/
                    }
//cout << runningProc.getRemaining() << " work left on running process" << endl;
                }

                if (!isRunning && !readyProcs.empty()) {
                    runningProc = readyProcs.front();
                    readyProcs.pop_front();
                    isRunning = true;
                    unsigned int currentTime = simTime;

                    // Track start and first response times
                    if (firstResponseTimes.find(runningProc.getArrival()) == firstResponseTimes.end()) {
                        firstResponseTimes[runningProc.getArrival()] = currentTime;
                        startTimes[runningProc.getArrival()] = currentTime;
                    }
                }


                break;
                /////////////////TO DO ... round-robin scheduling
            case 1: // RR
                simTime++; // Update simulation time

                while (simTime == newProcs.front().getArrival()) {
                    readyProcs.push_back(newProcs.front());
                    newProcs.pop_front();

                }

                // If the current process's time slice has expired, move it to the back of the ready queue
                if (isRunning && counter >= timeslice) {
                    // Move the current running process to the end of the ready queue
                    readyProcs.push_back(runningProc);
                    isRunning = false; // Mark as not running
                    counter = 0; // Reset the time slice counter
                }
                counter++; // Increment the time slice counter

                // If no process is currently running and there are processes ready to run
                if (!isRunning && !readyProcs.empty()) {
                    runningProc = readyProcs.front(); // Take the first process from the ready queue
                    readyProcs.pop_front(); // Remove it from the ready queue
                    isRunning = true; // Mark as currently running
                }

                // If a process is currently running
                if (isRunning) {
                    if (runningProc.process()) { // Process one work unit; check if the job has completed
                        runningProc.setFinished(simTime + 1); // Set finish time as the current time plus one unit
                        exitProcs.push_back(runningProc); // Move completed process to the exit queue
                        isRunning = false; // No longer running
                        counter = 0; // Reset counter
                    }
                }

                if (!isRunning && !readyProcs.empty()) {
                    runningProc = readyProcs.front();
                    readyProcs.pop_front();
                    isRunning = true;
                    unsigned int currentTime = simTime;

                    // Track start and first response times
                    if (firstResponseTimes.find(runningProc.getArrival()) == firstResponseTimes.end()) {
                        firstResponseTimes[runningProc.getArrival()] = currentTime;
                    }
                    if (startTimes.find(runningProc.getArrival()) == startTimes.end()) {
                        startTimes[runningProc.getArrival()] = currentTime;
                    }
                }

                break;


                /////////////////TO DO ... feedback scheduling
//            case 2: //FB
//                break;
//                ///////////////////////////////////////////////////////////////////
//                ///////////Intentionally no default block - no scheduling alg == error
            default:
                cout << "ERROR - need a scheduling algorithm!" << endl << endl;
                return -1;
        }

    }

    cout << "There are " << exitProcs.size() << " finished processes" << endl;
    unsigned int avgNTaT = 0;
    for (deque<simProcess>::iterator it = exitProcs.begin(); it != exitProcs.end(); it++) {
        avgNTaT += (*it).getNormalizedTurnaround();
    }
    cout << " with an average normalized turnaround time of "
         << static_cast<double>(avgNTaT) / exitProcs.size() << endl;

    unsigned int totalProcesses = exitProcs.size();
    for (auto &proc : exitProcs) {
        unsigned int arr = proc.getArrival();
        totalResponseTime += firstResponseTimes[arr] - arr;
    }

    cout << "Average Response Time: " << totalResponseTime / totalProcesses << endl;


    return 0;
}
