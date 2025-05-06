CUR_DIR := .
BIN_DIR := $(CUR_DIR)/bin
INC_DIR := $(CUR_DIR)/inc
LIB_DIR := $(CUR_DIR)/lib
OBJ_DIR := $(CUR_DIR)/obj
SRC_DIR := $(CUR_DIR)/src
LIB_STATIC_DIR := $(LIB_DIR)/static

create_obj:
	gcc -c $(SRC_DIR)/main.c -o $(OBJ_DIR)/main.o -I $(INC_DIR)
	gcc -c $(SRC_DIR)/helloWorld.c -o $(OBJ_DIR)/helloWorld.o -I $(INC_DIR)

create_static:
	ar rcs $(LIB_STATIC_DIR)/libhelloworld.a $(OBJ_DIR)/helloWorld.o

linking:
	gcc $(OBJ_DIR)/main.o  -L$(LIB_STATIC_DIR) -lhelloworld -o $(BIN_DIR)/statically-linked

all: create_obj create_static linking
	@echo "create static library"

run:
	$(BIN_DIR)/statically-linked

clean:
	rm -rf bin/statically-linked $(OBJ_DIR)/*.o $(LIB_STATIC_DIR)/libhelloworld.a