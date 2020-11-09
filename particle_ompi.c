/*
 * Genetic algorithm for 2D Lennard Jones particle simulation
 * M. Kuttel October 2020
 */                                                                                                                                             

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <mpi.h>

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
            {
                printf("INSIDE THE r==0!, %f\n", r);
                r = 0.9;
            }
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
    printf("Here 1\n");
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
    printf("Here 2\n");
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

void send_boxes(int start, int count, int num_particles, box_pattern* data, int dest, int tag, MPI_Comm comm)
{
    //find correct size for buffer
    int size = 0;
    int tmpsize;
    MPI_Pack_size(count, MPI_DOUBLE, comm, &tmpsize);
    size += tmpsize;
    MPI_Pack_size(count*num_particles*2, MPI_INT, comm, &tmpsize);
    size += tmpsize;

    char buf[size];
    int pos = 0;

    for(int b = 0; b < count; ++b)
    {
        //for each box, pack fitness
        MPI_Pack(&data[start+b].fitness, 1, MPI_DOUBLE, buf, size, &pos, comm);

        //for each box, pack particle postions
        for(int p = 0; p < num_particles; ++p)
        {
            MPI_Pack(&data[start+b].particle[p].x_pos, 1, MPI_INT, buf, size, &pos, comm);
            MPI_Pack(&data[start+b].particle[p].y_pos, 1, MPI_INT, buf, size, &pos, comm);
        }
    }
    MPI_Request request;
    MPI_Isend(buf, pos, MPI_PACKED, dest, tag, comm, &request);
}

void recv_boxes(int start, int count, int num_particles, box_pattern* data, int src, int tag, MPI_Comm comm)
{
    MPI_Status status;
    MPI_Probe(src, tag, comm, &status);
    int size;
    MPI_Get_count(&status, MPI_PACKED, &size);
    char buf[size];
    MPI_Recv(buf, size, MPI_PACKED, src, tag, comm, &status);

    int pos = 0;

    for(int b = 0; b < count; ++b)
    {
        //for each box, unpack fitness
        MPI_Unpack(buf, size, &pos, &data[start+b].fitness, 1, MPI_DOUBLE, comm);

        //for each box, unpack particle postions
        for(int p = 0; p < num_particles; ++p)
        {
            MPI_Unpack(buf, size, &pos, &data[start+b].particle[p].x_pos, 1, MPI_INT, comm);
            MPI_Unpack(buf, size, &pos, &data[start+b].particle[p].y_pos, 1, MPI_INT, comm);
        }
    }
}

int main(int argc, char *argv[])
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int population_size = DEFAULT_POP_SIZE;
    int x_max = X_DEFAULT;
    int y_max = Y_DEFAULT;
    int num_particles = DEFAULT_NUM_PARTICLES;
    int iter = ITERATIONS;
    int k,i;

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
    }

    int subpopulation_size = population_size/size;
    int exchange_amount = subpopulation_size/10;
    int exchange_freq = iter/200;
    int stagnantcheck_freq = exchange_freq*5;

    FILE *f = fopen("solution_omp.txt", "w");

    if(rank == 0)
    {
        printf("Starting optimization with particles = %d, population=%d, width=%d,length=%d for %d iterations\n", num_particles, population_size, x_max, y_max, iter);
        printf("Writing dimensions to file\n");
        fprintf(f, "%d,%d\n", x_max, y_max); //write box dimensions as first line of file
    }
    int gen_count = 0;
    double total_time = 0;

    box_pattern * population;

    population = (box_pattern*) malloc(sizeof(box_pattern)*subpopulation_size); //allocate memory
    for(i=0;i<subpopulation_size;i++)
        population[i].particle = malloc(num_particles*sizeof(position)); //allocate memory
    if(rank == 0)
        printf("Population size: %d, Subpop size: %d, Maxrank: %d\n", population_size, subpopulation_size, size);
    for(k=0; k<iter; k++)
    {   //k is number of times whole simulation is run
        //populate with initial population for each process
        if(rank == 0)
            printf("=========%d\n", k);
        printf("Initializing population for island %d\n", rank);
        initPopulation(population, subpopulation_size, x_max, y_max, num_particles);
        printf("Here island %d/n", rank);
        double max_fitness = 0;
        // main loop
        int stop = 0;
        int gen = 0, highest = 0;
        int current_stagnant = 0;
        int max_stagnant = MAX_GEN/10;
        int exchange_count = 0;

        while(gen < MAX_GEN)
        {
            if(gen != 0 && gen%exchange_freq == 0) //check if it is time to migrate/exchange
            {
                int send_to = (rank + 1 + exchange_count)%size;
                int receive_from = (rank - 1 - exchange_count)%size;
                printf("Island %d is ready to send individuals to island %d\n", rank, send_to);
                send_boxes(exchange_count, exchange_amount, num_particles, population, send_to, 0, MPI_COMM_WORLD);

                printf("Island %d is ready to receive individuals from island %d\n", rank, receive_from);
                recv_boxes(exchange_count, exchange_amount, num_particles, population, receive_from, 0, MPI_COMM_WORLD);
                printf("Island %d received individuals!\n", rank);

                exchange_count += 1;
            }

            int current_best = breeding(population, subpopulation_size, x_max, y_max, num_particles);
            if(current_best > highest)
            {
                highest = current_best;
                current_stagnant = 0;
            }
            else
                current_stagnant += 1;

            if(gen != 0 && gen%stagnantcheck_freq == 0) //check if it is time to report/check stagnancy
            {
                int total_stagnant = 0;
                MPI_Allreduce(&current_stagnant, &total_stagnant, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

                if((double)total_stagnant/(double)size > max_stagnant) //break if average greater than max
                    break;
            }

            gen += 1;
        }

        int current = 0;
        while(current < size)
        {
            if(rank==current)
            {
                printf("GA for thread %d:\n", rank);
                printf("# generations = %d\n", gen);
                printf("Best solution:\n");
                printbox(population[highest], num_particles);
            }
            rank++;
            MPI_Barrier(MPI_COMM_WORLD);
        }

        //find the highest fitness across all processes

        int localmax[2];
        int globalmax[2];
        localmax[0] = population[highest].fitness;
        localmax[1] = rank;
        MPI_Allreduce(localmax, globalmax, 1, MPI_2INT, MPI_MAXLOC, MPI_COMM_WORLD);

        box_pattern * global_bestbox = (box_pattern*) malloc(sizeof(box_pattern)); //allocate memory
        global_bestbox[0].particle = malloc(num_particles*sizeof(position)); //allocate memory

        if(globalmax[1] == 0) //if highest fitness on root
        {
            copybox(&population[highest], &global_bestbox[0], num_particles);
        }
        else
        {
            if(globalmax[1] == rank)
            {
                send_boxes(highest, 1, num_particles, population, 0, 1, MPI_COMM_WORLD);
            }
            else if(rank == 0)
            {
                recv_boxes(0, 1, num_particles, global_bestbox, globalmax[1], 1, MPI_COMM_WORLD);
            } 
        }

        if(rank == 0)
        {
            if (f == NULL)
            {
                printf("Error opening file!\n");
                exit(1);
            }
            
            printboxFile(global_bestbox[0], f, num_particles);
            printf("---------\n");
        }
        gen_count += gen;
    }
    fclose(f);

    for(i = 0; i < subpopulation_size; i++)
        free(population[i].particle); //release memory
    free(population); //release memory

    printf("Average generations for thread %d: %f\n", rank, (double)gen_count/(double)k);
    MPI_Finalize();
    return 0;
}

