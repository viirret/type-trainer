CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -g -lSDL2 -lSDL2_ttf
SRC_DIR = src
BUILD_DIR = build

# Source files and object files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC_FILES))

# Target executable
TARGET = $(BUILD_DIR)/typing_trainer
all: $(TARGET)

# Rule to create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Rule to build the target
$(TARGET): $(BUILD_DIR) $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $(TARGET) $(CFLAGS)

# Rule for compiling .c files to .o files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -rf $(BUILD_DIR)

# Run the program
run: $(TARGET)
	./$(TARGET)
