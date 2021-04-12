#ifndef RESEARCH_JSGAME_H_
#define RESEARCH_JSGAME_H_
#include <vector>
using namespace std;

struct Job
{
    int player_id;
    int machine_id;
    int processing_time;
    Job(int, int);
    ~Job();
};

struct Machine
{
    vector<Job> jobs;
    int load;
};

struct Player
{
    vector<Job> jobs;
    Player(int ,int);
    ~Player();
};

// assigment algorithms
void leastLoaded(vector<Player> &, vector<Machine> &);

// equalibria validation
bool checkWE(vector<Machine> &);

// print functions
void printPlayers(vector<Player> &);
void printMachines(vector<Machine> &);

// tools
vector<vector<int>> combination(int , int);
vector<int> toBase(int, int, int);

#endif 