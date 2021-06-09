#ifndef RESEARCH_JSGAME_H_
#define RESEARCH_JSGAME_H_
#include <vector>
#include <string>
#include <pthread.h>
using namespace std;

struct Job
{ 
    int player_id;
    int machine_id;
    int processing_time;
    Job();
    Job(int);
};

struct Machine
{
    vector<Job> jobs;
    int load;
};

struct Player
{
    vector<Job> jobs;
    Player();
    Player(int, int);
};

struct Thread_info
{
    int id;
    int num_of_cases;
    int left_cases;
    int total_jobs;
    double strict_ir_min = 1;
    double strict_ir_avg = 1;
    double loose_ir_avg = 1;
    double ir_all = 1;
    string schedule_str;
    string strict_ir_min_str;
    string strict_ir_avg_str;
    string loose_ir_avg_str;
    string ir_all_str;
    vector<Player> players;
    vector<Machine> machines;
};

void *thread_func(void *info);

// assigment algorithms
void leastLoaded(vector<Player> &, vector<Machine> &);

// equalibria validation
bool checkWE(vector<Machine> &);

// print functions
void printPlayers(vector<Player> &);
void printMachines(vector<Machine> &);



// tools
string getSchedule(vector<Machine> &);
vector<vector<int>> combination(int, int);
vector<int> toBase(int, int, int);

#endif