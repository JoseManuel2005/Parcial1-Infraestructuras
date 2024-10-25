all: compile exe

compile:
	# gcc -fopenmp -o main main.c
	gcc -fopenmp -o main main.c

exe:
	./all.sh

clean:
	rm -f main *.bin*