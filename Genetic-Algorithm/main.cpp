#include <iostream>
#include <vector>
#include <time.h>
#include <string>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <stdio.h>

using namespace std;

struct population
{
    vector <string> chromosome;
    vector <double> fitness;
    vector <int> FLM;
};

population Init_Population(population); // Create first population.
population Fitness_Calc(population); // Calculate fitness.
population Crossover(population pop); // Cross over of parents.

string roulette(population pop); // Roulette picking of parents.
string mutate(string chromo); // Mutate chromosome.
void Print_Stuff(population); // Print generations.


double S_RATE[5] = {0.15, 0.4, 0.6, 0.8, 1.0}; // Speed rates of processors.
double C_UTIL[4] = {0.45, 0.4, 0.4, 0.16}; // Core utilization after allocation
double C_RATE[4] = {0.6, 0.4, 0.4, 0.4}; // Core rate after allocation.
double T_UTIL[5] = {0.45, 0.4, 0.2, 0.2, 0.16}; //Task utilization.
int T_CORE_NUM[5] = {1, 2, 3, 3, 4}; // Core where each task is located.
double P[5] = {80, 170, 400, 900, 1600}; // Power per speed rate.

int MUTATE_RATE;

// Main function.
int main()
{
    int iterr;
    cout << "ENTER ITERRATIONS: ";
    cin >> iterr;
    cout << "ENTER MUTATE RATE: ";
    cin >> MUTATE_RATE;

    population pop;
    pop = Init_Population(pop);
    pop = Fitness_Calc(pop);

    cout << "INITIAL POPULATION" << endl;
    Print_Stuff(pop);

    for (int i = 0; i < iterr; i++)
    {
        pop = Crossover(pop);
        pop = Fitness_Calc(pop);

        cout << "\n\nGENERATION: " << i +1 << "\n" << endl;
        Print_Stuff(pop);
    }

    cout<< "\n\nBEST CHROMOSOME: " << pop.chromosome[pop.chromosome.size()-1] << endl;
    cout << "BEST SAVING: " << pop.fitness[pop.fitness.size()-1] <<"mW    "<<pop.FLM[pop.FLM.size()-1]<<"%"<< endl;

    getchar();
    return 0;
}

// Create initial population.
population Init_Population(population pop)
{
    srand(time(NULL));
    int pop_size = 0;
    for (int t = 0; t < 5; t++)
        pop_size = pop_size + int(T_UTIL[t]/0.01);

    double x, y;
    for (int TASK = 0; TASK < 5; TASK++) // For each task in task set do..
    {
        int t1 = int(T_UTIL[TASK]/0.01);  // This gives how many ways the task can be split given smallest 0.01.
        x = 0.0;
        for (int k = 0; k < t1; k++)
        {
            y = T_UTIL[TASK] - x; // Get split portion of given task.
            int RANDOM_CORE = (rand()% 4); // Randomly generate a processor number.
            int RANDOM_RATE = (rand()% 5); // randomly generate a speed rate.
            char buff[15];
            snprintf(buff, sizeof(buff),"%d %.2f %d %d", TASK, y, RANDOM_CORE, RANDOM_RATE); // Create chromosome.
            string str = buff;
            pop.chromosome.push_back(str);
            pop.fitness.push_back(0);
            pop.FLM.push_back(0);

            x = x + 0.01;
        }
    }

    return pop; // Return the population.
}


// Fitness calculator.
population Fitness_Calc(population pop)
{
    int POP_SIZE = pop.chromosome.size(); // Get the size of incoming population.

    for (int i = 0; i < POP_SIZE; i++) // for each chromosome in population do...
    {
        string cs = pop.chromosome[i];
        stringstream ss (cs);
        int TASK, RANDOM_CORE, RANDOM_RATE; double Ui1;
        ss >> TASK >> Ui1 >> RANDOM_CORE >> RANDOM_RATE;

        int m = T_CORE_NUM[TASK] - 1;
        double Sy = S_RATE[RANDOM_RATE];
        double Sx = C_RATE[m];
        double Uj = C_UTIL[RANDOM_CORE];
        double Ui = T_UTIL[TASK];
        double Ui2 = Ui - Ui1;
        double Syo = C_RATE[RANDOM_CORE];

        int Syn = distance(S_RATE, find(S_RATE, S_RATE + 5, Sy));
        int Sxn = distance(S_RATE, find(S_RATE, S_RATE+5, Sx));
        int Syoo = distance(S_RATE, find(S_RATE, S_RATE+5, Syo));

        double R = (Sx*(Sx-Ui))/(Sx-Sy);
        double L = (Sy - Uj);
        double y =  min(L,R); //

        double Pnew = (((Uj + Ui1)/Sy) * P[Syn]) + ((Ui2/Sx)*P[Sxn]);
        double Pold = ((Uj/Syoo)*P[Syoo]) + ((Ui/Sx)*P[Sxn]);
        double diff = Pold-Pnew;
        int savings = int((diff/Pold)*100);
//
//        if (diff > 0 )
//            pop.fitness.at(i) = diff;
//
//        else
//            pop.fitness.at(i) = 10;

            if (y <= Ui1 && y > 0.00 && diff > 0 )
            {
                pop.fitness.at(i) = diff;
                pop.FLM.at(i) = savings;
            }
            else
            {
                pop.fitness.at(i) = 10;
            }

    }

    return pop;
}


population Crossover(population pop)
{
    population pop2;
    int POP_SIZE = pop.chromosome.size();
    string parentA;
    string parentB;
    string child;
    string mutant;
    for (int i = 0; i < POP_SIZE; i++)
    {
        parentA = roulette(pop);
        parentB = roulette(pop);

        stringstream ss (parentA);
        int TASK1, RANDOM_CORE1, RANDOM_RATE1; double Ui11;
        ss >> TASK1 >> Ui11 >> RANDOM_CORE1 >> RANDOM_RATE1;

        stringstream ss2 (parentB);
        int TASK2, RANDOM_CORE2, RANDOM_RATE2; double Ui12;
        ss2 >> TASK2 >> Ui12 >> RANDOM_CORE2 >> RANDOM_RATE2;

        int x = (rand()% 100);

        if (x % 2 == 0)
        {
            char buff[15];
            snprintf(buff, sizeof(buff),"%d %.2f %d %d", TASK1, Ui11, RANDOM_CORE2, RANDOM_RATE2);
            child = buff;
        }
        else
        {
            char buff[15];
            snprintf(buff, sizeof(buff),"%d %.2f %d %d", TASK2, Ui12, RANDOM_CORE1, RANDOM_RATE1);
            child = buff;
        }

        mutant = mutate(child);
        pop2.chromosome.push_back(mutant);
        pop2.fitness.push_back(0);
        pop2.FLM.push_back(0);
    }

   return pop2;
}

// Roulette wheel for picking a parent.
string roulette(population pop)
{
    double tot_fitness = 0;

    for (int i = 0; i < pop.fitness.size(); i++)
    {
       tot_fitness = tot_fitness + pop.fitness[i];
    }

    int POP_SIZE = pop.chromosome.size();
    double RAND_NUM = ((double)rand()/(RAND_MAX+1));
    double slice = (double)(RAND_NUM*tot_fitness);
    double FitSoFar = 0.0;

    for (int i = 0; i < POP_SIZE; i++)
    {
        FitSoFar += pop.fitness[i];
        if(FitSoFar >= slice)
        {
            return pop.chromosome[i];
        }
    }

    return "";
}

// Print output.
void Print_Stuff(population pop)
{
    int POP_SIZE = pop.chromosome.size();

    cout << setprecision(2) << fixed;
    cout << "#chrom" << "\t" << "Chromosome" << "\t" << "Fitness" << endl;
    for (int i = 0; i < POP_SIZE; i++)
    {
        cout << i << "\t" << pop.chromosome[i] << "\t" << pop.fitness[i] << "\t"<< pop.FLM[i] << endl;
    }

}

string mutate(string chromo)
{
    int c, r;
    float x = ((float)rand()/(RAND_MAX+1));

    if ( x < MUTATE_RATE)
    {
        stringstream ss (chromo);
        int TASK, RANDOM_CORE, RANDOM_RATE; double Ui1;
        ss >> TASK >> Ui1 >> RANDOM_CORE >> RANDOM_RATE;

        do {
             c = rand()%4;
        }while (c == RANDOM_CORE);

        int NEW_CORE = c;

        do {
             r = rand()%5;
        }while(r == RANDOM_RATE);

        int NEW_RATE = r;

        string child;
        char buff[15];
        snprintf(buff, sizeof(buff),"%d %.2f %d %d", TASK, Ui1, NEW_CORE, NEW_RATE);
        child = string(buff);

        return child;
    }
    else

    {
        return chromo;
    }
}


