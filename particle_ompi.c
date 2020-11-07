/*
 * Genetic algorithm for 2D Lennard Jones particle simulation
 * M. Kuttel October 2020
 */                                                                                                                                             

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <omp.h>

#define DEFAULT_POP_SIZE 300 //bigger population is more costly
#define DEFAULT_NUM_PARTICLES 30 //more PARTICLES is more costly

// consts
static const int X_DEFAULT = 20; //width of box
static const int Y_DEFAULT = 20; //length of box
static const double MUTATION_RATE = 0.10; //how often random mutations occur
static const double MAX_GEN = 1000; // maximum number of generations
static const double ITERATIONS = 10; //number of times the whole process is run
static const double TOLERANCE = 50; //not used... yet


//each particle has x and y location in box
typedef struct
{
    int x_pos;
    int y_pos;
} position;

// box pattern
typedef struct
{
    position *particle;
    double fitness;
} box_pattern;

//display the box pattern
void printbox(box_pattern box,int num_particles)
{
    int i;
    for(i = 0; i < num_particles - 1; i++)
    {
        printf("%d,%d\t", box.particle[i].x_pos, box.particle[i].y_pos);
    }
    printf("%d,%d\t:fitness %f\n", box.particle[i].x_pos, box.particle[i].y_pos, box.fitness);
}

//print the box pattern to file
void printboxFile(box_pattern box,FILE *f,int num_particles )
{
    int i;
    for(i = 0; i < num_particles - 1; i++)
    {
        fprintf(f,"%d,%d\t", box.particle[i].x_pos, box.particle[i].y_pos);
    }
    fprintf(f,"%d,%d\n", box.particle[i].x_pos, box.particle[i].y_pos);
}

/* FITNESS FUNCTION  - this is key*/
double calcFitness(box_pattern box,int num_particles)
{
    double fitness = 0.0;
    int i,j;
    double x,y,r,tmp;
    for(i = 0; i < num_particles - 1; i++)
    {
        for(j = i + 1; j < num_particles; j++)
        {   //cycle through all pairs to calc distances
            x = (double)box.particle[i].x_pos - (double)box.particle[j].x_pos;
            y = (double)box.particle[i].y_pos - (double)box.particle[j].y_pos;
            r=sqrt((x*x)+(y*y));
            if(r == 0)
                r = 0.9;
            tmp=2.0/r;
            //fitness-= 1.0/r; // electric repulsion
            //fitness-= pow(tmp,6); //purely repulsive function
            fitness+= (pow(tmp,12)-pow(tmp,6)); //Lennard-Jones function
        }
    }
    return fitness;
}

/* Creates initial random population */
void initPopulation(box_pattern * box, int population_size,int xmax,int ymax,int num_particles)
{
    int i,p;
    for(p = 0; p < population_size; p++)
    {
        for(i=0; i<num_particles; i++)
        {
            box[p].particle[i].x_pos=(rand()%(xmax + 1));
            box[p].particle[i].y_pos=(rand()%(ymax + 1));
        }
        box[p].fitness=calcFitness(box[p],num_particles);
    }
}

/* create child from parents */
box_pattern crossover(box_pattern child, box_pattern parentOne, box_pattern parentTwo, int splitPoint,int num_particles){
    int i=0;
    for(i=0; i<splitPoint; i++)
    {   //copy over parentOne up to splitPoint
        child.particle[i].x_pos=parentOne.particle[i].x_pos;
        child.particle[i].y_pos=parentOne.particle[i].y_pos;
    }
    i--;
    if((rand()%(2) == 1) && (i < num_particles) &&(i >= 0)) //50% of time split in middle of particle, more mixing
        child.particle[i].y_pos=parentTwo.particle[i].y_pos; //don't know if I like this tbh. Seems redundant.
    
    for(i = splitPoint; i < num_particles; i++)
    {   //copy over parentTwo from splitPoint to end
        child.particle[i].x_pos=parentTwo.particle[i].x_pos;
        child.particle[i].y_pos=parentTwo.particle[i].y_pos;
    }
    child.fitness=calcFitness(child,num_particles); //calculate fitness
    return child;
}

/* deep copy b into a [does a=b] */
void copybox(box_pattern *a, box_pattern *b,int num_particles)
{
    int i;
    for (i = 0; i < num_particles; i++){
        (*a).particle[i].x_pos = (*b).particle[i].x_pos;
        (*a).particle[i].y_pos = (*b).particle[i].y_pos;
    }
    (*a).fitness=(*b).fitness;
}

/* Main GA function - does selection, breeding, crossover and mutation */
int breeding(box_pattern * box, int population_size, int x_max, int y_max,int num_particles)
{
        int highest;
        box_pattern max_parent; //keep track of highest from previous generation
        max_parent.particle = malloc(num_particles*sizeof(position));
        copybox(&max_parent, &box[0], num_particles); //set max to first one
        int i;
        box_pattern * new_generation = (box_pattern*) malloc(sizeof(box_pattern)*(population_size));
        for(i = 0; i < population_size; i++)
            new_generation[i].particle = malloc(num_particles*sizeof(position));

        for (i = 0; i < population_size; i += 2)
        {   //two children
            // Determine breeding pair, with tournament of 2 (joust)
            int one, two, splitPoint;
            do
            {
                one = rand()%(population_size), two = rand()%(population_size);
            } while(one == two);
            int parentOne = two;
            if(box[one].fitness > box[two].fitness)
                parentOne = one; //joust
        
            do
            {
                one = rand()%(population_size), two = rand()%(population_size);
            } while(one == two);
            int parentTwo = two;
            if(box[one].fitness > box[two].fitness)
                parentTwo = one; //joust
        
            do
            {
                splitPoint = rand()%num_particles; //spl/* code */it chromosome at point
            } while(splitPoint == 0 || splitPoint == num_particles - 1);
            new_generation[i] = crossover(new_generation[i], box[parentOne], box[parentTwo], splitPoint, num_particles); //first child

            new_generation[i+1] = crossover(new_generation[i+1], box[parentTwo], box[parentOne], splitPoint, num_particles); //second child
        
            // Mutation first child
            double mutation = rand()/(double)RAND_MAX;
            if(mutation <= MUTATION_RATE)
            {
                int mutated;
                do
                {
                    mutated = rand() % num_particles;
                } while(mutated == num_particles);
                new_generation[i].particle[mutated].x_pos = (rand()%(x_max + 1));
                new_generation[i].particle[mutated].y_pos = (rand()%(y_max + 1));
                new_generation[i].fitness = calcFitness(new_generation[i], num_particles);
            }
            mutation = rand()/(double)RAND_MAX; //mutation second child
            if(mutation <= MUTATION_RATE)
            {
                int mutated;
                do
                {
                    mutated = rand() % num_particles;
                } while(mutated == num_particles);
                new_generation[i+1].particle[mutated].x_pos = (rand()%(x_max + 1));
                new_generation[i+1].particle[mutated].y_pos = (rand()%(y_max + 1));
                new_generation[i+1].fitness = calcFitness(new_generation[i+1], num_particles);
            }
        }
  
        //find maximum parent fitness to keep and minimum new generation to throw away
        new_generation[0].fitness = calcFitness(new_generation[0],num_particles);
        double min_fitness = new_generation[0].fitness;
        int min_box = 0;
        double max_fitness = new_generation[0].fitness;
        highest = 0;
        for(i = 1; i < population_size; i++)
        {
            if(box[i].fitness > max_parent.fitness)
            {
                copybox(&max_parent, &box[i], num_particles); //replace lowest fitness with highest parent
            }
            new_generation[i].fitness=calcFitness(new_generation[i],num_particles);
            if(new_generation[i].fitness < min_fitness)
            {
                min_fitness = new_generation[i].fitness;
                min_box = i;
            }
            if(new_generation[i].fitness > max_fitness)
            {
                max_fitness = new_generation[i].fitness;
                highest = i;
            }
        }
    
        // printf("max fitness should be: %f\n",max_parent.fitness);
        //copies
        for(i = 0; i < population_size; i++)
        {
            //printbox(new_generation[i]);
            if(i == min_box)
            {
                copybox(&box[i], &max_parent, num_particles);
            } 
            else
            {
                copybox(&box[i], &new_generation[i], num_particles);
            }
           // printbox(box[i]);
        }
        if(max_parent.fitness > max_fitness)
        {   //previous generation has the best
            max_fitness=max_parent.fitness;
            highest=min_box;
            //printf("max fitness should be: %f",max_parent.fitness);
        }
        for(i = 0;i < population_size; i++)
            free(new_generation[i].particle); //release memory
        free(new_generation); //release memory
        free(max_parent.particle);
        return highest;
}


int main(int argc, char *argv[])
{
    omp_set_num_threads(2);
    int population_size = DEFAULT_POP_SIZE;
    int x_max = X_DEFAULT;
    int y_max = Y_DEFAULT;
    int num_particles = DEFAULT_NUM_PARTICLES;
    int iter = ITERATIONS;
    int k,i;
    int exchange_amount = population_size/10;

    if(argc >= 2)
    {
        population_size = atoi(argv[1]); //size population first command line argument
        if(argc >= 4)
        {
            x_max = atoi(argv[2]); //x dimension
            y_max = atoi(argv[3]); //x dimension
        }
        if(argc >= 5)
            num_particles = atoi(argv[4]);
        if(argc >= 6)
            iter = atoi(argv[5]);
        if(argc == 7)
            omp_set_num_threads(atoi(argv[6]));
    }

    printf("Starting optimization with particles = %d, population=%d, width=%d,length=%d for %d iterations\n", num_particles, population_size, x_max, y_max, iter);

    int gen_count = 0;
    double total_time = 0;

    FILE *f = fopen("solution_omp.txt", "w");
    printf("Writing dimensions to file\n");
    fprintf(f, "%d,%d\n", x_max, y_max); //write box dimensions as first line of file
    box_pattern * population;

    population = (box_pattern*) malloc(sizeof(box_pattern)*population_size); //allocate memory
    for(i=0;i<population_size;i++)
        population[i].particle = malloc(num_particles*sizeof(position));//allocate memory

    for(k=0; k<iter; k++)
    {   //k is number of times whole simulation is run
        // populate with initial population
        printf("Initializing population\n");
        initPopulation(population, population_size, x_max, y_max, num_particles);
        printf("=========%d\n", k);

        #pragma omp parallel private(gen_count)
        {
            printf("running!")
            double max_fitness = 0;
            // main loop
            int stop = 0;
            int gen = 0,highest = 0;
            int current_stagnant = 0;
            int max_stagnant = MAX_GEN/10;
            bool exchanged = false;

            box_pattern pop_copy[population_size];
            for(int j = 0; j < population_size; j++)
            {
                copybox(&pop_copy[j], &population[j], num_particles);
            }

            while(gen < MAX_GEN)
            {
                if(current_stagnant >= max_stagnant || gen == MAX_GEN/2)
                {
                    if(exchanged == false)
                    {
                        printf("Initiating exchange\n");
                        //perform exchange
                        current_stagnant = 0;
                        exchanged = true;
                    }
                    if(gen != MAX_GEN/2)
                    {
                        printf("Stopping due to no improvements for %d generations after exchange", max_stagnant);
                        break;
                    }
                }

                int current_best = breeding(pop_copy, population_size, x_max, y_max, num_particles);
                if(current_best > highest)
                {
                    highest = current_best;
                    current_stagnant = 0;
                }
                else
                    current_stagnant += 1;
                
                gen += 1;
            }
            #pragma omp critical
            {
                printf("GA for thread %d:\n", omp_get_thread_num());
                printf("# generations = %d\n", gen);
                printf("Best solution:\n");
                printbox(pop_copy[highest], num_particles);
                if (f == NULL)
                {
                    printf("Error opening file!\n");
                    exit(1);
                }
                printboxFile(pop_copy[highest], f, num_particles);
                printf("---------\n");
            }
            gen_count += gen;
        }
    }
    fclose(f);

    for(i = 0; i < population_size; i++)
        free(population[i].particle); //release memory
    free(population); //release memory

    printf("Average generations: %f\n", (double)gen_count/(double)k);
    return 0;
}

