CC=gcc -g

CFLAGS=
LIBS=

BIN_DIR=bin
SRC_DIR=src
OBJ_DIR=.obj

# MODULES=

TARGET_EXEC = $(BIN_DIR)/a.out

ALL_SRC := $(shell find $(SRC_DIR) -name "*.c")
ALL_OBJ := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(ALL_SRC))

# PKG_CFLAGS := $(shell pkg-config --cflags $(MODULES))
# PKG_LIBS := $(shell pkg-config --libs $(MODULES))

CFLAGS += $(PKG_CFLAGS)
LIBS += $(PKG_LIBS)

all: $(TARGET_EXEC)

$(TARGET_EXEC): $(ALL_OBJ) | $(BIN_DIR)
	$(CC) -o $@ $^ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR) $(BIN_DIR):
	@mkdir -p $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean
