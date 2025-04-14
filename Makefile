# === Compiler and Flags ===
CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -Wall -Wextra

# === Project Structure ===
SRC_DIR = src
OBJ_DIR = obj
BIN = tankgame

# === Source Files ===
SRCS = \
	$(SRC_DIR)/Core/Game.cpp \
	$(SRC_DIR)/Core/Tank.cpp \
	$(SRC_DIR)/Core/Artillery.cpp \
	$(SRC_DIR)/Core/GameObject.cpp \
	$(SRC_DIR)/Core/Direction.cpp \
	$(SRC_DIR)/TankAlgorithm/TankChase.cpp \
	$(SRC_DIR)/TankAlgorithm/TankEvasion.cpp

# === Object Files (flattened to obj/)
OBJS = \
	$(OBJ_DIR)/Game.o \
	$(OBJ_DIR)/Tank.o \
	$(OBJ_DIR)/Artillery.o \
	$(OBJ_DIR)/GameObject.o \
	$(OBJ_DIR)/Direction.o \
	$(OBJ_DIR)/TankChase.o \
	$(OBJ_DIR)/TankEvasion.o

# === Default Target ===
all: prebuild $(BIN)

# === Ensure obj/ exists ===
prebuild:
	@if not exist "$(OBJ_DIR)" mkdir $(OBJ_DIR)

# === Link Executable ===
$(BIN): $(OBJS) main.o
	$(CXX) $(CXXFLAGS) $^ -o $@

# === Compile main.cpp ===
main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# === Compile Core Source Files ===
$(OBJ_DIR)/%.o: $(SRC_DIR)/Core/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# === Compile AI Files ===
$(OBJ_DIR)/TankChase.o: $(SRC_DIR)/TankAlgorithm/TankChase.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/TankEvasion.o: $(SRC_DIR)/TankAlgorithm/TankEvasion.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# === Clean Target ===
clean:
	-del /Q $(OBJ_DIR)\*.o main.o $(BIN).exe 2>NUL
	-rmdir /S /Q $(OBJ_DIR) 2>NUL

.PHONY: all clean prebuild
