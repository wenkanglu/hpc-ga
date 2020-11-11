particle: particle.c
	 /usr/bin/gcc -I/usr/include -L/usr/lib particle.c -o particle -lm

particle_omp: particle_omp.c
	/usr/bin/gcc -I/usr/include -L/usr/lib particle_omp.c -fopenmp -o particle_omp -lm

particle_ompi: particle_ompi.c
	mpicc -I/usr/include -L/usr/lib particle_ompi.c -o particle_ompi -lm

run: particle
	./particle 1000 100 100 10 10
	cat solution.txt
	./particle 1000 100 100 20 10
	cat solution2.txt
	./particle 1000 100 100 30 10
	cat solution3.txt

	./particle 1000 150 150 10 10
	cat solution4.txt
	./particle 1000 150 150 20 10
	cat solution5.txt
	./particle 1000 150 150 30 10
	cat solution6.txt

	./particle 1000 200 200 10 10
	cat solution7.txt
	./particle 1000 200 200 20 10
	cat solution8.txt
	./particle 1000 200 200 30 10
	cat solution9.txt

	./particle 2000 100 100 10 10
	cat solution10.txt
	./particle 2000 100 100 20 10
	cat solution11.txt
	./particle 2000 100 100 30 10
	cat solution12.txt

	./particle 2000 150 150 10 10
	cat solution13.txt
	./particle 2000 150 150 20 10
	cat solution14.txt
	./particle 2000 150 150 30 10
	cat solution15.txt

	./particle 2000 200 200 10 10
	cat solution16.txt
	./particle 2000 200 200 20 10
	cat solution17.txt
	./particle 2000 200 200 30 10
	cat solution18.txt

	./particle 5000 100 100 10 10
	cat solution19.txt
	./particle 5000 100 100 20 10
	cat solution20.txt
	./particle 5000 100 100 30 10
	cat solution21.txt

	./particle 5000 150 150 10 10
	cat solution22.txt
	./particle 5000 150 150 20 10
	cat solution23.txt
	./particle 5000 150 150 30 10
	cat solution24.txt

	./particle 5000 200 200 10 10
	cat solution25.txt
	./particle 5000 200 200 20 10
	cat solution26.txt
	./particle 5000 200 200 30 10
	cat solution27.txt
	# python3 plot_solution.py solution.txt

run_omp: particle_omp
	./particle_omp 2000 100 100 10 5 4
	cat solution_omp.txt
	# python3 plot_solution.py solution_omp.txt

run_ompi: particle_ompi
	mpirun -np 4 particle_ompi 2000 100 100 10 5
	cat solution_ompi.txt
	python3 plot_solution.py solution_ompi.txt

clean:
	rm -f particle
	rm -f particle_omp
	rm -f particle_ompi
	rm -f solution*.*

