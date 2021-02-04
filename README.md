# hpc_ga
Repository containing a genetic algorithm (GA) to optimize the [Lennard-Jones potential](https://en.wikipedia.org/wiki/Lennard-Jones_potential).

---

_particle.c_ contains a basic, sequential GA.

_particle_omp.c_ makes use of OpenMP to parallelise the GA.

_particle_ompi.c_ makes use of, you guessed it, OpenMPI to parallelise the GA - best used on an HPC cluster.

_plot_solution.py_ visualises the optimised results using [Matplotlib](https://matplotlib.org/).

---

`make all` - compiles the C scripts.

`make particle`, `make particle_omp`, and `make particle_ompi` each compiles an individual script.

`run`, `run_omp`, and `run_ompi` each runs an individual script.

`make clean` - removes compiled C scripts.

`make clean_all` - removes compiled C scripts and run artifacts
