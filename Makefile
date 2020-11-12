particle: particle.c
	 /usr/bin/gcc -I/usr/include -L/usr/lib particle.c -o particle -lm

particle_omp: particle_omp.c
	/usr/bin/gcc -I/usr/include -L/usr/lib particle_omp.c -fopenmp -o particle_omp -lm

particle_ompi: particle_ompi.c
	mpicc -I/usr/include -L/usr/lib particle_ompi.c -o particle_ompi -lm

run: particle
	./particle 1000 100 100 10 10
	./particle 1000 100 100 20 10
	./particle 1000 100 100 30 10

	./particle 1000 150 150 10 10
	./particle 1000 150 150 20 10
	./particle 1000 150 150 30 10

	./particle 1000 200 200 10 10
	./particle 1000 200 200 20 10
	./particle 1000 200 200 30 10

	./particle 2000 100 100 10 10
	./particle 2000 100 100 20 10
	./particle 2000 100 100 30 10

	./particle 2000 150 150 10 10
	./particle 2000 150 150 20 10
	./particle 2000 150 150 30 10

	./particle 2000 200 200 10 10
	./particle 2000 200 200 20 10
	./particle 2000 200 200 30 10

	./particle 5000 100 100 10 10
	./particle 5000 100 100 20 10
	./particle 5000 100 100 30 10

	./particle 5000 150 150 10 10
	./particle 5000 150 150 20 10
	./particle 5000 150 150 30 10

	./particle 5000 200 200 10 10
	./particle 5000 200 200 20 10
	./particle 5000 200 200 30 10
	# python3 plot_solution.py solution.txt

run_omp: particle_omp
	./particle_omp 1000 100 100 10 10
	./particle_omp 1000 100 100 20 10
	./particle_omp 1000 100 100 30 10

	./particle_omp 1000 150 150 10 10
	./particle_omp 1000 150 150 20 10
	./particle_omp 1000 150 150 30 10

	./particle_omp 1000 200 200 10 10
	./particle_omp 1000 200 200 20 10
	./particle_omp 1000 200 200 30 10

	./particle_omp 2000 100 100 10 10
	./particle_omp 2000 100 100 20 10
	./particle_omp 2000 100 100 30 10

	./particle_omp 2000 150 150 10 10
	./particle_omp 2000 150 150 20 10
	./particle_omp 2000 150 150 30 10

	./particle_omp 2000 200 200 10 10
	./particle_omp 2000 200 200 20 10
	./particle_omp 2000 200 200 30 10

	./particle_omp 5000 100 100 10 10
	./particle_omp 5000 100 100 20 10
	./particle_omp 5000 100 100 30 10

	./particle_omp 5000 150 150 10 10
	./particle_omp 5000 150 150 20 10
	./particle_omp 5000 150 150 30 10

	./particle_omp 5000 200 200 10 10
	./particle_omp 5000 200 200 20 10
	./particle_omp 5000 200 200 30 10
	# python3 plot_solution.py solution_omp.txt

run_ompi: particle_ompi
	mpirun -np 2 particle_ompi 1000 100 100 10 10
	mpirun -np 2 particle_ompi 1000 100 100 20 10
	mpirun -np 2 particle_ompi 1000 100 100 30 10

	mpirun -np 2 particle_ompi 1000 150 150 10 10
	mpirun -np 2 particle_ompi 1000 150 150 20 10
	mpirun -np 2 particle_ompi 1000 150 150 30 10

	mpirun -np 2 particle_ompi 1000 200 200 10 10
	mpirun -np 2 particle_ompi 1000 200 200 20 10
	mpirun -np 2 particle_ompi 1000 200 200 30 10

	mpirun -np 2 particle_ompi 2000 100 100 10 10
	mpirun -np 2 particle_ompi 2000 100 100 20 10
	mpirun -np 2 particle_ompi 2000 100 100 30 10

	mpirun -np 2 particle_ompi 2000 150 150 10 10
	mpirun -np 2 particle_ompi 2000 150 150 20 10
	mpirun -np 2 particle_ompi 2000 150 150 30 10

	mpirun -np 2 particle_ompi 2000 200 200 10 10
	mpirun -np 2 particle_ompi 2000 200 200 20 10
	mpirun -np 2 particle_ompi 2000 200 200 30 10

	mpirun -np 2 particle_ompi 5000 100 100 10 10
	mpirun -np 2 particle_ompi 5000 100 100 20 10
	mpirun -np 2 particle_ompi 5000 100 100 30 10

	mpirun -np 2 particle_ompi 5000 150 150 10 10
	mpirun -np 2 particle_ompi 5000 150 150 20 10
	mpirun -np 2 particle_ompi 5000 150 150 30 10

	mpirun -np 2 particle_ompi 5000 200 200 10 10
	mpirun -np 2 particle_ompi 5000 200 200 20 10
	mpirun -np 2 particle_ompi 5000 200 200 30 10
	# mpirun -np 4 particle_ompi 2000 100 100 10 5
	# cat solution_ompi.txt
	# python3 plot_solution.py solution_ompi.txt

clean_all:
	rm -f particle
	rm -f particle_omp
	rm -f particle_ompi
	rm -f solution*.*
	rm -f results*.*

clean:
	rm -f particle
	rm -f particle_omp
	rm -f particle_ompi

