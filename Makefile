# === Compiler and Flags ===
CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -Wall -Wextra

# === Project Structure ===
SRC_DIR = src/Core
INCLUDE_DIR = include/Core
OBJ_DIR = obj
BIN = game

# === Source and Object Files ===
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# === Default Target ===
all: $(BIN)

# === Linking ===
$(BIN): $(OBJS) main.o
	$(CXX) $(CXXFLAGS) $^ -o $@

# === Compile main.cpp ===
main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# === Compile each .cpp to .o ===
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# === Create obj directory if missing ===
$(OBJ_DIR):
	mkdir $(OBJ_DIR)

# === Clean build artifacts ===
clean:
	rm -f $(OBJ_DIR)/*.o main.o $(BIN)

.PHONY: all clean
