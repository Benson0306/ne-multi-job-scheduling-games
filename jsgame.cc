#include <algorithm>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <cstdlib> 
#include <ctime> 
#include <string>
#include "jsgame.h"

Job::Job() {}

Job::Job(int processing_time)
{
    this->processing_time = processing_time;
}

Player::Player() {}

Player::Player(int max_jobs, int max_processing_time)
{   
    int num_of_jobs = rand() % (max_jobs - 1) + 2;
    for (int i = 0; i < num_of_jobs; i++)
    {
        // time range from 1 to max_processing_time
        jobs.push_back(Job(rand() % max_processing_time + 1));
    }
}

void *thread_func(void *info)
{

    Thread_info *thread_info = (Thread_info *)info;
    int start_case = thread_info->id * thread_info->num_of_cases;
    int end_case = start_case + thread_info->num_of_cases + thread_info->left_cases - 1;

    // traverse all assignments
    for (int a = start_case; a <= end_case; a++)
    {
        // generate each assignment
        vector<int> assignment = toBase(a, thread_info->machines.size(), thread_info->total_jobs);

        // assigning jobs
        int current_job = 0;
        for (int i = 0; i < thread_info->players.size(); i++)
        {

            for (auto &&j : thread_info->players[i].jobs)
            {
                j.machine_id = assignment[current_job];
                j.player_id = i;
                thread_info->machines[j.machine_id].jobs.push_back(j);
                thread_info->machines[j.machine_id].load += j.processing_time;
                current_job++;
            }
        }

        if (checkWE(thread_info->machines))
        {
            // for each player
            for (int i = 0; i < thread_info->players.size(); i++)
            {
                // strict condition for every member
                double strict_ir_min_coalition;
                double strict_ir_avg_coalition;

                // loose condition for every member
                double loose_ir_avg_coalition;

                // ir of all jobs
                double ir_all_coalition;

                // coalition starts from size 2
                for (int k = 2; k <= thread_info->players[i].jobs.size(); k++)
                {
                    vector<vector<int>> coalition = combination(thread_info->players[i].jobs.size(), k);

                    // for each coalition
                    for (const auto &c : coalition)
                    {
                        
                        vector<int> c_machine_set; // all machines involved in coalition 
                        vector<int> machine_set;

                        for (const auto &j : c)
                        {
                            c_machine_set.push_back(thread_info->players[i].jobs[j].machine_id);
                        }
                        sort(c_machine_set.begin(), c_machine_set.end());
                        vector<int>::iterator last = unique(c_machine_set.begin(), c_machine_set.end());
                        c_machine_set.resize(distance(c_machine_set.begin(), last));

                        // all jobs reside in the same machine
                        if (c_machine_set.size() == 1)
                        {
                            continue;
                        }

                        for (int m = 0; m < thread_info->machines.size(); m++)
                        {
                            machine_set.push_back(m);
                        }

                        // target machines
                        vector<vector<int>> c_targets(c.size(), vector<int>(0, 0));
                        vector<vector<int>> targets(c.size(), vector<int>(0, 0));

                        for (int j = 0; j < c.size(); j++)
                        {
                            for (const auto m : c_machine_set)
                            {
                                if (thread_info->players[i].jobs[c[j]].machine_id == m)
                                    continue;
                                c_targets[j].push_back(m);
                            }

                            for (const auto m : machine_set)
                            {
                                if (thread_info->players[i].jobs[c[j]].machine_id == m)
                                    continue;
                                targets[j].push_back(m);
                            }
                        }

                        // to keep track of next element in each job
                        vector<int> c_indices(c.size(), 0);

                        while (true)
                        {

                            // current deviation
                            vector<int> deviation;
                            vector<int> m_load_after;

                            for (int j = 0; j < c.size(); j++)
                            {
                                deviation.push_back(c_targets[j][c_indices[j]]);
                            }

                            // initial load
                            for (const auto &m : thread_info->machines)
                            {
                                m_load_after.push_back(m.load);
                            }

                            // deviating
                            for (int j = 0; j < c.size(); j++)
                            {
                                // before
                                m_load_after[thread_info->players[i].jobs[c[j]].machine_id] -= thread_info->players[i].jobs[c[j]].processing_time;
                                // after
                                m_load_after[deviation[j]] += thread_info->players[i].jobs[c[j]].processing_time;
                            }

                            // compute IR
                            bool strict = true;
                            double ir;
                            int after = 0, before = 0;
                            strict_ir_min_coalition = INT16_MAX;
                            string case_buff = "";

                            for (int j = 0; j < c.size(); j++)
                            {
                                // Individual Improvement Ratio
                                ir = (double)thread_info->machines[thread_info->players[i].jobs[c[j]].machine_id].load / m_load_after[deviation[j]];

                                // Collective Improvement Ratio
                                before += thread_info->machines[thread_info->players[i].jobs[c[j]].machine_id].load;
                                after += m_load_after[deviation[j]];

                                if (ir <= 1)
                                    strict = false;

                                if (ir < strict_ir_min_coalition)
                                    strict_ir_min_coalition = ir;

                                case_buff += "Job " + to_string(c[j]) + "(" + to_string(thread_info->players[i].jobs[c[j]].processing_time) + ")" + " from M" + to_string(thread_info->players[i].jobs[c[j]].machine_id + 1) + "(" + to_string(thread_info->machines[thread_info->players[i].jobs[c[j]].machine_id].load) + ") to M" + to_string(deviation[j] + 1) + "(" + to_string(m_load_after[deviation[j]]) + ")" + " IR =" + to_string(ir) + "\n";
                            }

                            if (strict)
                            {
                                if (strict_ir_min_coalition > thread_info->strict_ir_min)
                                {
                                    thread_info->strict_ir_min = strict_ir_min_coalition;
                                    thread_info->strict_ir_min_str = getSchedule(thread_info->machines);
                                    thread_info->strict_ir_min_str += case_buff;
                                    thread_info->strict_ir_min_str += "Player " + to_string(i + 1) + ", ";
                                    thread_info->strict_ir_min_str += to_string(strict_ir_min_coalition) + "\n";
                                }

                                strict_ir_avg_coalition = (double)before / after;
                                if (strict_ir_avg_coalition > thread_info->strict_ir_avg)
                                {
                                    thread_info->strict_ir_avg = strict_ir_avg_coalition;
                                    thread_info->strict_ir_avg_str = getSchedule(thread_info->machines);
                                    thread_info->strict_ir_avg_str += case_buff;
                                    thread_info->strict_ir_avg_str += "Player " + to_string(i + 1) + ", ";
                                    thread_info->strict_ir_avg_str += to_string(strict_ir_avg_coalition) + "\n";
                                }
                            }

                            // find the rightmost array that has more
                            // elements left after the current element
                            // in that array
                            int next = c.size() - 1;
                            while (next >= 0 && (c_indices[next] + 1 >= c_targets[next].size()))
                                next--;

                            // no such array is found so no more
                            // combinations left
                            if (next < 0)
                            {
                                break;
                            }

                            // if found move to next element in that
                            // array
                            c_indices[next]++;

                            // for all arrays to the right of this
                            // array current index again points to
                            // first element
                            for (int j = next + 1; j < c.size(); j++)
                                c_indices[j] = 0;
                        }

                        // to keep track of next element in each job
                        vector<int> indices(c.size(), 0);

                        while (true)
                        {
                            // current deviation
                            vector<int> deviation;
                            vector<int> m_load_after;

                            for (int j = 0; j < c.size(); j++)
                            {
                                deviation.push_back(targets[j][indices[j]]);
                            }

                            // initial load
                            for (const auto &m : thread_info->machines)
                            {
                                m_load_after.push_back(m.load);
                            }

                            // deviating
                            for (int j = 0; j < c.size(); j++)
                            {
                                // update 
                                m_load_after[thread_info->players[i].jobs[c[j]].machine_id] -= thread_info->players[i].jobs[c[j]].processing_time;
                                m_load_after[deviation[j]] += thread_info->players[i].jobs[c[j]].processing_time;
                            }

                            // compute IR
                            double ir;
                            int c_after = 0, c_before = 0;
                            int all_after = 0, all_before = 0;
                            string case_buff = "";
                            Player player_after = thread_info->players[i];

                            // load for player before reschedule
                            for (int j = 0; j < thread_info->players[i].jobs.size(); j++)
                            {
                                all_before += thread_info->machines[thread_info->players[i].jobs[j].machine_id].load;
                            }
                            
                            for (int j = 0; j < c.size(); j++)
                            {
                                // Improvement Ratio
                                ir = (double)thread_info->machines[thread_info->players[i].jobs[c[j]].machine_id].load / m_load_after[deviation[j]];

                                // Load Improvement Ratio
                                c_before += thread_info->machines[thread_info->players[i].jobs[c[j]].machine_id].load;
                                c_after += m_load_after[deviation[j]];

                                player_after.jobs[c[j]].machine_id = deviation[j];

                                case_buff += "Job " + to_string(c[j]) + "(" + to_string(thread_info->players[i].jobs[c[j]].processing_time) + ")" + " from M" + to_string(thread_info->players[i].jobs[c[j]].machine_id + 1) + "(" + to_string(thread_info->machines[thread_info->players[i].jobs[c[j]].machine_id].load) + ") to M" + to_string(deviation[j] + 1) + "(" + to_string(m_load_after[deviation[j]]) + ")" + " IR =" + to_string(ir) + "\n";
                            }

                            loose_ir_avg_coalition = (double)c_before / c_after;

                            if (loose_ir_avg_coalition > 1)
                            {
                                if (loose_ir_avg_coalition > thread_info->loose_ir_avg)
                                {
                                    thread_info->loose_ir_avg = loose_ir_avg_coalition;
                                    thread_info->loose_ir_avg_str = getSchedule(thread_info->machines);
                                    thread_info->loose_ir_avg_str += case_buff;
                                    thread_info->loose_ir_avg_str += "Player " + to_string(i + 1) + ", ";
                                    thread_info->loose_ir_avg_str += to_string(loose_ir_avg_coalition) + "\n";
                                }
                            }

                            // load for player after reschedule
                            for (int j = 0; j < player_after.jobs.size(); j++)
                            {
                                all_after += m_load_after[player_after.jobs[j].machine_id];
                            }

                            ir_all_coalition = (double)all_before / all_after;

                            if (ir_all_coalition > 1)
                            {
                                if (ir_all_coalition > thread_info->ir_all)
                                {
                                    thread_info->ir_all = ir_all_coalition;
                                    thread_info->ir_all_str = getSchedule(thread_info->machines);
                                    thread_info->ir_all_str += case_buff;
                                    thread_info->ir_all_str += "Player " + to_string(i + 1) + ", ";
                                    thread_info->ir_all_str += to_string(ir_all_coalition) + "\n";
                                }
                            }

                            // find the rightmost array that has more
                            // elements left after the current element
                            // in that array
                            int next = c.size() - 1;
                            while (next >= 0 && (indices[next] + 1 >= targets[next].size()))
                                next--;

                            // no such array is found so no more
                            // combinations left
                            if (next < 0)
                            {
                                break;
                            }

                            // if found move to next element in that
                            // array
                            indices[next]++;

                            // for all arrays to the right of this
                            // array current index again points to
                            // first element
                            for (int j = next + 1; j < c.size(); j++)
                                indices[j] = 0;
                        }
                    }
                }
            }
        }
        // clear machines
        for (auto &&m : thread_info->machines)
        {
            m.jobs.clear();
            m.load = 0;
        }
    }
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
    { // print each player : time pair
        cout << "M" << m + 1 << "(" << setw(4) << machines[m].load << ") : ";
        for (auto &&j : machines[m].jobs)
        {
            cout << j.player_id << ":" << j.processing_time << " ";
        }
        cout << endl;
    }
}

string getSchedule(vector<Machine> &machines)
{
    string buff;
    // for each machine
    for (int m = 0; m < machines.size(); m++)
    { // print each player : time pair
        buff += "M" + to_string(m + 1) + "(" + to_string(machines[m].load) + ") : ";
        for (auto &&j : machines[m].jobs)
        {
            buff += to_string(j.player_id) + ":" + to_string(j.processing_time) + " ";
        }
        buff += "\n";
    }
    return buff;
}

vector<vector<int>> combination(int n, int k)
{
    string bitmask(k, 1); // K leading 1's
    bitmask.resize(n, 0); // N-K trailing 0's
    vector<vector<int>> comb;
    // print integers and permute bitmask
    do
    {
        vector<int> permu;
        for (int i = 0; i < n; ++i) // [0..N-1] integers
        {
            if (bitmask[i])
                permu.push_back(i);
        }
        comb.push_back(permu);
    } while (prev_permutation(bitmask.begin(), bitmask.end()));
    return comb;
}

vector<int> toBase(int dividend, int base, int size)
{
    vector<int> vec(size, 0);

    do
    {
        vec[--size] = dividend % base;
        dividend /= base;

    } while (dividend != 0);

    return vec;
}