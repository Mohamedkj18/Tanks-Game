# === Compiler and Flags ===
CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -Wall -Wextra -Werror -pedantic 

# === Project Structure ===
CORE_SRC_DIR = src/Core
ALGO_SRC_DIR = src/TankAlgorithm
INCLUDE_DIR = include
OBJ_DIR = obj
BIN = tankgame

# === Source and Object Files ===
CORE_SRCS = $(wildcard $(CORE_SRC_DIR)/*.cpp)
ALGO_SRCS = $(wildcard $(ALGO_SRC_DIR)/*.cpp)
SRCS = $(CORE_SRCS) $(ALGO_SRCS)
OBJS = $(SRCS:src/%.cpp=$(OBJ_DIR)/%.o)

# === Default Target ===
all: $(BIN)

# === Linking ===
$(BIN): $(OBJS) main.o
	$(CXX) $(CXXFLAGS) $^ -o $@

# === Compile main.cpp ===
main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# === Compile Core .cpp files ===
$(OBJ_DIR)/Core/%.o: $(CORE_SRC_DIR)/%.cpp | $(OBJ_DIR)/Core
	$(CXX) $(CXXFLAGS) -c $< -o $@

# === Compile TankAlgorithm .cpp files ===
$(OBJ_DIR)/TankAlgorithm/%.o: $(ALGO_SRC_DIR)/%.cpp | $(OBJ_DIR)/TankAlgorithm
	$(CXX) $(CXXFLAGS) -c $< -o $@

# === Create obj directory structure ===
$(OBJ_DIR)/Core:
	mkdir -p $@

$(OBJ_DIR)/TankAlgorithm:
	mkdir -p $@

$(OBJ_DIR):
	mkdir -p $@

# === Clean build artifacts ===
clean:
	rm -rf $(OBJ_DIR) main.o $(BIN)

.PHONY: all clean