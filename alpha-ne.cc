#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <ctime>
#include "jsgame.h"
using namespace std;

int main(int argc, char *argv[])
{        
    int num_of_players = atoi(argv[1]);     
    int num_of_machines = atoi(argv[2]);  
    int max_jobs = atoi(argv[3]);        
    int max_processing_time = atoi(argv[4]);
    int num_of_interations = atoi(argv[5]);
    int num_of_threads = atoi(argv[6]);

    vector<Player> players;   // each player
    vector<Machine> machines; // each machine

    double strict_ir_min = 1;
    double strict_ir_avg = 1;
    double loose_ir_avg = 1;
    double ir_all = 1;
    string strict_ir_min_str = "no case\n";
    string strict_ir_avg_str = "no case\n";
    string loose_ir_avg_str = "no case\n";
    string ir_all_str = "no case\n";

    srand(time(NULL));

    float progress = 0.0;
    for (int iter = 0; iter < num_of_interations; iter++)
    {   
        int barWidth = 50;
        cout << "[";
        int pos = barWidth * progress;
        for (int i = 0; i < barWidth; ++i)
        {
            if (i < pos)
                cout << "=";
            else if (i == pos)
                cout << ">";
            else
                cout << " ";
        }
        cout << "] " << int(progress * 100.0) << " %\r";
        cout.flush();

        // initialize palyers
        for (int i = 0; i < num_of_players; i++)
        {
            players.push_back(Player(max_jobs, max_processing_time));
        }

        
        // initialize machines
        for (int i = 0; i < num_of_machines; i++)
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

        // thread infos
        int total_cases = pow(machines.size(), number_of_job);
        int num_of_cases = total_cases / num_of_threads;
        Thread_info *thread_info = (Thread_info *)malloc(sizeof(Thread_info) * num_of_threads);
        pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * (num_of_threads - 1));

        // start threads
        for (int i = 0; i < num_of_threads - 1; i++)
        {
            thread_info[i].id = i;
            thread_info[i].num_of_cases = num_of_cases;
            thread_info[i].total_jobs = number_of_job;
            thread_info[i].left_cases = 0;
            thread_info[i].machines = machines;
            thread_info[i].players = players;
            pthread_create(&threads[i], NULL, thread_func, (void *)&thread_info[i]);
        }

        // join threads
        for (int i = 0; i < num_of_threads - 1; i++)
        {
            pthread_join(threads[i], NULL);
        }

        // main thread
        thread_info[num_of_threads - 1].id = num_of_threads - 1;
        thread_info[num_of_threads - 1].num_of_cases = num_of_cases;
        thread_info[num_of_threads - 1].left_cases = total_cases % num_of_threads;
        thread_info[num_of_threads - 1].total_jobs = number_of_job;
        thread_info[num_of_threads - 1].machines = machines;
        thread_info[num_of_threads - 1].players = players;
        thread_func((void *)&thread_info[num_of_threads - 1]);


        for (int i = 0; i < num_of_threads; i++)
        {

            if (thread_info[i].strict_ir_min > strict_ir_min)
            {
                strict_ir_min = thread_info[i].strict_ir_min;
                strict_ir_min_str = thread_info[i].strict_ir_min_str;
            }
            if (thread_info[i].strict_ir_avg > strict_ir_avg)
            {
                strict_ir_avg = thread_info[i].strict_ir_avg;
                strict_ir_avg_str = thread_info[i].strict_ir_avg_str;
            }
            if (thread_info[i].loose_ir_avg > loose_ir_avg)
            {
                loose_ir_avg = thread_info[i].loose_ir_avg;
                loose_ir_avg_str = thread_info[i].loose_ir_avg_str;
            }
            if (thread_info[i].ir_all > ir_all)
            {
                ir_all = thread_info[i].ir_all;
                ir_all_str = thread_info[i].ir_all_str;
            }
        }
        
        // if(strict_ir_min != 1 || strict_ir_avg != 1 || loose_ir_avg != 1)
        // {
        //     cout << iter << " : ";
        //     cout << strict_ir_min << " " << strict_ir_avg << " " << loose_ir_avg << endl;
        //     cout << "--------------\n";
        // }   
    
        // clear players
        players.clear();

        // clear machines
        machines.clear();

        // update progress
        progress = (iter + 2)/(float)num_of_interations;
    }
    cout << endl;
    cout << strict_ir_min << " " << strict_ir_avg << " " << loose_ir_avg << " " << ir_all << endl;

    return 0;
}