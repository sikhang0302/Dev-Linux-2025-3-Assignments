CUR_DIR := .
BIN_DIR := $(CUR_DIR)/bin
INC_DIR := $(CUR_DIR)/inc
LIB_DIR := $(CUR_DIR)/lib
OBJ_DIR := $(CUR_DIR)/obj
SRC_DIR := $(CUR_DIR)/src
LIB_SHARED_DIR := $(LIB_DIR)/shared


create_obj:
	gcc -c $(SRC_DIR)/main.c -o $(OBJ_DIR)/main.o -I $(INC_DIR)
	gcc -c -fPIC $(SRC_DIR)/myMath.c -o $(OBJ_DIR)/myMath.o -I $(INC_DIR)

create_shared:
	gcc -shared $(OBJ_DIR)/myMath.o -o $(LIB_SHARED_DIR)/libmymath.so

linking:
	gcc  $(OBJ_DIR)/main.o -L$(LIB_SHARED_DIR) -lmymath -o $(BIN_DIR)/use-shared-library

all: create_obj create_shared linking
	@echo "create shared library"

run:
	LD_LIBRARY_PATH=$(LIB_SHARED_DIR) $(BIN_DIR)/use-shared-library

clean:
	rm -rf $(BIN_DIR)/use-shared-library $(OBJ_DIR)/*.o $(LIB_SHARED_DIR)/libmymath.so