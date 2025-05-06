.PHONY := clean all
TARGET ?= my_program
FILE ?= my_file.txt

all:
	gcc -o $(TARGET) main.c

run:
	./$(TARGET) $(FILE) 5 w Hello 

clean:
	rm -rf $(TARGET) ./*.txt