CC := gcc

all:
	$(CC) -o exercise main.c

run:
	sh -c 'export ACTION=$${ACTION:-1}; ./exercise'

clean:
	rm -rf exercise