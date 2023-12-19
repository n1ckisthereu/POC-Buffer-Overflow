# Nome do executável
TARGET = app

# Compilador
CC = gcc

# Flags de compilação
CFLAGS_COMMON = -Wall -Wextra
CFLAGS_DEBUG = -g -O0
CFLAGS_RELEASE = -O2 -mwindows

# Bibliotecas
LIBS = -static-libgcc -static-libstdc++ -lmingwex -lgdi32 -lws2_32

# Diretórios
SRC_DIR = src
BUILD_DIR = build
DEBUG_DIR = $(BUILD_DIR)/debug
RELEASE_DIR = $(BUILD_DIR)/release

# Arquivos fonte
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ_DEBUG = $(patsubst $(SRC_DIR)/%.c,$(DEBUG_DIR)/%.o,$(SRC))
OBJ_RELEASE = $(patsubst $(SRC_DIR)/%.c,$(RELEASE_DIR)/%.o,$(SRC))

# Regras de compilação
all: debug release

debug: $(DEBUG_DIR)/$(TARGET)

release: $(RELEASE_DIR)/$(TARGET)

$(DEBUG_DIR)/$(TARGET): $(OBJ_DEBUG)
	$(CC) $^ -o $@ $(LIBS) $(CFLAGS_DEBUG)

$(RELEASE_DIR)/$(TARGET): $(OBJ_RELEASE)
	$(CC) $^ -o $@ $(LIBS) $(CFLAGS_RELEASE)

$(DEBUG_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p "$(dir $@)"
	$(CC) $(CFLAGS_COMMON) $(CFLAGS_DEBUG) -c $< -o $@

$(RELEASE_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p "$(dir $@)"
	$(CC) $(CFLAGS_COMMON) $(CFLAGS_RELEASE) -c $< -o $@

clean:
	@rm -rf "$(BUILD_DIR)"

.PHONY: all debug release clean
