particle: particle.c
	 /usr/bin/gcc -I/usr/include -L/usr/lib particle.c -o particle -lm

particle_omp: particle_omp.c
	/usr/bin/gcc -I/usr/include -L/usr/lib particle_omp.c -fopenmp -o particle_omp -lm

particle_ompi: particle_ompi.c
	mpicc -I/usr/include -L/usr/lib particle_ompi.c -o particle_ompi -lm

run: particle
	./particle 1000 100 100 10 10
	cat solution_1000_100_100_10_10.txt
	cat results.txt
	# don't forget to input the correct file name (not just solutions.txt)
	python3 plot_solution.py solution_1000_100_100_10_10.txt

run_omp: particle_omp
	./particle_omp 1000 100 100 10 10
	cat solution_omp_1000_100_100_10_10.txt
	cat results_omp.txt
	# don't forget to input the correct file name (not just solutions.txt)
	python3 plot_solution.py solution_omp_1000_100_100_10_10.txt

run_ompi: particle_ompi
	# population size must be divisible by the number of processors (islands) where each has an EVEN amount of solutions
	mpirun -np 4 particle_ompi 1000 100 100 10 10
	cat solution_ompi_1000_100_100_10_10.txt
	cat results_ompi.txt
	# don't forget to input the correct file name (not just solutions.txt)
	python3 plot_solution.py solution_ompi_1000_100_100_10_10.txt

# remove all outputs
clean_all:
	rm -f particle
	rm -f particle_omp
	rm -f particle_ompi
	rm -f solution*.*
	rm -f results*.*

# retain solutions and results
clean:
	rm -f particle
	rm -f particle_omp
	rm -f particle_ompi
