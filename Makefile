BINARY_NAME := sha256

SRC_DIR := ./src
OBJ_DIR := ./build

SRC_FILES = $(shell find $(SRC_DIR)/ -type f -name '*.cpp')
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

COMPILER = g++
LDFLAGS := -std=c++17
CPPFLAGS := -std=c++17

all: clean $(OBJ_DIR)/$(BINARY_NAME) run

.PHONY: clean
clean:
	@rm -rf build

.PHONY: run
run: $(OBJ_DIR)/$(BINARY_NAME)
	./$(OBJ_DIR)/$(BINARY_NAME)

.PHONY: format
format:
	clang-format -i **/*.cpp **/*.h

$(OBJ_DIR):
	mkdir -p ./build

$(OBJ_DIR)/$(BINARY_NAME): $(OBJ_FILES) | $(SHADERS_OUT)
	$(COMPILER) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
  # HACK: Create the containing directory for each output file ( $(@D) gives the directory of $@ )
	mkdir -p $(@D)
	$(COMPILER) $(CPPFLAGS) -c -o $@ $<
