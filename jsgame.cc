#include <algorithm>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include "jsgame.h"

Job::Job(int player_id, int processing_time)
{
    this->player_id = player_id;
    this->processing_time = processing_time;
}

Job::~Job()
{
}

Player::Player(int number_of_jobs, int max_processing_time)
{
    for (int i = 0; i < number_of_jobs; i++)
    {
        // time range from 1 to max_processing_time
        jobs.push_back(Job(i, rand() % max_processing_time + 1));
    }
    
}

Player::~Player()
{
}

void leastLoaded(vector<Player> &players, vector<Machine> &machines)
{
    // for each player
    for (int i = 0; i < players.size(); i++)
    {   
        // for each job
        for (auto &&j : players[i].jobs)
        {
            // find least loaded machine
            int least_loaded_machine = 0;
            int min_load = INT32_MAX;
            for (int m = 0; m < machines.size(); m++)
            {
                if (machines[m].load < min_load)
                {
                    least_loaded_machine = m;
                    min_load = machines[m].load;
                }
            }
            
            // assign job
            j.machine_id = least_loaded_machine;
            j.player_id = i + 1;
            machines[least_loaded_machine].jobs.push_back(j);
            machines[least_loaded_machine].load += j.processing_time;
        }
    }
}

bool checkWE(vector<Machine> &machines)
{
    // flag
    bool is_WE = true;

    // for each machine
    for (int m = 0; m < machines.size(); m++)
    {
        for (const auto &j : machines[m].jobs)
        {
            for (int m2 = 0; m2 < machines.size(); m2++)
            {
                if (m != m2)
                {
                    if ((machines[m].load - machines[m2].load) > j.processing_time)
                    {
                        is_WE = false;
                        break;
                    }
                }
            }
        }
    }

    return is_WE;
}

void printPlayers(vector<Player> &players)
{
    for (int i = 0; i < players.size(); i++)
    {
        cout << "Player " << i << " : ";
        for (const auto &j : players[i].jobs)
        {  
            cout << j.processing_time << ", ";
        }
        cout << endl;
    }
}

void printMachines(vector<Machine> &machines)
{
    // for each machine
    for (int m = 0; m < machines.size(); m++)
    {   // print each player : time pair
        cout << "M" << m + 1 << "(" << setw(4) << machines[m].load << ") : ";
        for (auto &&j : machines[m].jobs)
        {
            cout << j.player_id << ":" << j.processing_time << " ";
        }
        cout << endl;
    }
}

vector<vector<int>> combination(int n, int k)
{
    string bitmask(k, 1); // K leading 1's
    bitmask.resize(n, 0); // N-K trailing 0's
    vector<vector<int>> comb;
    // print integers and permute bitmask
    do {
        vector<int> permu;
        for (int i = 0; i < n; ++i) // [0..N-1] integers
        {     
            if (bitmask[i]) permu.push_back(i);
        }
        comb.push_back(permu);
    } while (prev_permutation(bitmask.begin(), bitmask.end()));
    return comb;
}

vector<int> toBase(int dividend, int base, int size)
{
    vector<int> vec(size, 0);
    int remainder;

    do
    {
        vec[--size] = dividend % base;
        dividend /= base;

    } while (dividend != 0);
    
    return vec;
}