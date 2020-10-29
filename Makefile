particle: particle.c
	 /usr/bin/gcc -I/usr/include -L/usr/lib particle.c -o particle -lm
particle_omp: particle.c
	/usr/bin/gcc -I/usr/include -L/usr/lib particle.c -fopenmp -o particle_omp -lm

run: particle
	./particle 2000 100 100 10 5
	cat solution.txt
	python3 plot_solution.py solution.txt

run_omp: particle_omp
	./particle_omp 2000 100 100 10 5
	cat solution.txt
	python3 plot_solution.py solution.txt

clean:
	rm -f particle
	rm -f particle_omp
	rm -f solution.*
