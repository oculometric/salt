SRC_DIR			:= src/
BIN_DIR			:= bin/
OBJ_DIR			:= bin/obj/

CC				:= g++
CC_INCLUDE		:= -Iinc -Iinc/stui/inc
CC_FLAGS		:= -std=c++20 -g -O0 $(CC_INCLUDE) -Wall

LD				:= g++
LD_FLAGS		:= -g
LD_INCLUDE		:= -lpthread

SC				:= glslc
SC_FLAGS		:=

DEP_FLAGS		:= -MMD -MP

CC_FILES_IN		:= $(wildcard $(SRC_DIR)*.cpp) $(wildcard $(SRC_DIR)*/*.cpp)
CC_FILES_OUT	:= $(patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR)%.o, $(CC_FILES_IN))
CC_FILES_DEP	:= $(patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR)%.d, $(CC_FILES_IN))

EXE_OUT			:= $(BIN_DIR)salt

.PHONY: clean $(BIN_DIR) $(OBJ_DIR)

all: execute

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling" $< to $@
	@$(CC) $(CC_FLAGS) $(CC_INCLUDE) $(DEP_FLAGS) -c $< -o $@

-include $(CC_FILES_DEP)

$(EXE_OUT): $(CC_FILES_OUT)
	@mkdir -p $(dir $@)
	@echo "Linking" $(EXE_OUT)
	@$(LD) $(LD_FLAGS) -o $@ $(CC_FILES_OUT) $(LD_INCLUDE)

build: $(EXE_OUT)

execute: $(EXE_OUT)
	@$(EXE_OUT)

clean:
	@rm -r $(BIN_DIR)
