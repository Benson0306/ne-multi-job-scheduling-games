#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include "jsgame.h"
using namespace std;


int main()
{
    int number_of_players = 2;
    int number_of_machines = 3;
    int max_jobs = 3;
    int max_processing_time = 5;

    vector<Player> players; // each player
    vector<Machine> machines; // each machine

    srand(time(NULL));

    // initialize palyers
    for (int i = 0; i < number_of_players; i++)
    {
        players.push_back(Player(max_jobs, max_processing_time));
    }

    // initialize machines
    for (int i = 0; i < number_of_machines; i++)
    {
        machines.push_back(Machine());
    }

    int total;
    int machine_id;

    // count the number of jobs
    int number_of_job = 0;
    for (const auto &i : players)
    {
        number_of_job += i.jobs.size();
    }

    // each job has many choice 
    int total_case = pow(machines.size(), number_of_job);

    // traverse all assignments
    for (int a = 0; a < total_case; a++)
    {   
        // generate each assignment
        vector<int> assignment = toBase(a, machines.size(), number_of_job);

        // assigning jobs
        int current_job = 0;
        for (int i = 0; i < players.size(); i++)
        {   
            
            for (auto &&j : players[i].jobs)
            {
                j.machine_id = assignment[current_job];
                j.player_id = i;
                machines[j.machine_id].jobs.push_back(j);
                machines[j.machine_id].load += j.processing_time;
                
                current_job++;
            }
        }

        if (checkWE(machines)) 
        {
            printMachines(machines);
            cout << "-----------------" << endl;
            
            // for each player
            for (int i = 0; i < players.size(); i++)
            {   
                double ir_min_schedule = INT16_MIN;
                double ir_min_coalition = INT16_MAX;

                // coalition starts from size 2
                for (int k = 2; k <= players[i].jobs.size(); k++)
                {  
                    vector<vector<int>> coalition = combination(players[i].jobs.size(), k);

                    // for each coalition
                    for (const auto &c : coalition)
                    {  
                        // find all machines involved in coalition
                        vector<int> machine_set;

                        for (const auto &j : c)
                        {
                            machine_set.push_back(players[i].jobs[j].machine_id);
                        }
                        vector<int>::iterator last = unique(machine_set.begin(), machine_set.end());

                        machine_set.resize(distance(machine_set.begin(), last));

                        // all jobs reside in the same machine
                        if (machine_set.size() == 1) 
                        {
                            cout << "all jobs reside in the same machine" << endl;
                            cout << "-----------------" << endl;
                            continue;
                        }
                        // target machine for coalition member
                        vector<vector<int>> targets(c.size(), vector<int>(0,0));

            
                        for (int j = 0; j < c.size(); j++)
                        {
                             for (const auto m: machine_set)
                            {

                                if (players[i].jobs[c[j]].machine_id == m) continue;
                                
                                targets[j].push_back(m);
                            }
                        }
                    
                        // to keep track of next element in each job
                        vector<int> indices(c.size(), 0);

                        while(true) 
                        {   
                            // current deviation
                            vector<int> deviation;
                            vector<int> load_after;

                            for (int j = 0; j < c.size(); j++)
                            {
                                deviation.push_back(targets[j][indices[j]]);
                            }
                            
                            // initial load
                            for (const auto &m: machines)
                            {
                                load_after.push_back(m.load);
                            }
                            
                            // deviating
                            for (int j = 0; j < c.size(); j++)
                            {   
                                // before
                                load_after[players[i].jobs[c[j]].machine_id] -= players[i].jobs[c[j]].processing_time;
                                // after
                                load_after[deviation[j]] += players[i].jobs[c[j]].processing_time;
                            }

                            // compute IR
                            for (int j = 0; j < c.size(); j++)
                            {       
                                    
                                double ir = (double)machines[players[i].jobs[c[j]].machine_id].load/load_after[deviation[j]]; 
                        
                                cout << "Job " << c[j] << "(" << players[i].jobs[c[j]].processing_time << ")"
                                    << " from M" << players[i].jobs[c[j]].machine_id + 1
                                    << "(" << machines[players[i].jobs[c[j]].machine_id].load  << ") to M" << deviation[j] + 1 
                                    << "(" << load_after[deviation[j]] << ")" << " IR =" << ir << endl;

                                if (ir <= 1)
                                    
                                if (ir < ir_min_coalition)
                                    ir_min_coalition = ir;
                            }
                            
                            if (ir_min_coalition > ir_min_schedule)                       
                                ir_min_schedule = ir_min_coalition;
                            
                            
                            //cout << ir_min_coalition << " " << ir_min_schedule << endl; 
                            
                            // find the rightmost array that has more
                            // elements left after the current element
                            // in that array
                            int next = c.size() - 1;
                            while (next >= 0 && indices[next] + 1 >= targets[next].size())
                                next--;
                    
                            // no such array is found so no more
                            // combinations left
                            if (next < 0)
                            {
                                cout << "-----------------" << endl;
                                break;
                            }
                            // if found move to next element in that
                            // array
                            indices[next]++;
                    
                            // for all arrays to the right of this
                            // array current index again points to
                            // first element
                            for (int j = next + 1; j < c.size(); j++)
                                indices[j];
                        }
                    }
                }
                cout << "======>Player" << i << "'s" << " IR(min)= " << ir_min_schedule << endl;
            }
            cout << "\n*********************************************\n";
        }

        // clear each machine
        for (auto &&i : machines)
        {
            i.jobs.clear();
            i.load = 0;
        }
    }


    return 0;
}