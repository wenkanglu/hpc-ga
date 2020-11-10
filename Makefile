particle: particle.c
	 /usr/bin/gcc -I/usr/include -L/usr/lib particle.c -o particle -lm

particle_omp: particle_omp.c
	/usr/bin/gcc -I/usr/include -L/usr/lib particle_omp.c -fopenmp -o particle_omp -lm

particle_ompi: particle_ompi.c
	mpicc -I/usr/include -L/usr/lib particle_ompi.c -o particle_ompi -lm

run: particle
	./particle 2000 100 100 10 5
	cat solution.txt
	python3 plot_solution.py solution.txt

run_omp: particle_omp
	./particle_omp 2000 100 100 10 5 4
	cat solution_omp.txt
	python3 plot_solution.py solution_omp.txt

run_ompi: particle_ompi
	mpirun -np 4 particle_ompi 2000 100 100 10 5
	cat solution_ompi.txt
	python3 plot_solution.py solution_ompi.txt

clean:
	rm -f particle
	rm -f particle_omp
	rm -f particle_ompi
	rm -f solution*.*

