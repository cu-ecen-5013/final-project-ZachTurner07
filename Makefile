ifeq ($(CFLAGS),)
	CFLAGS := -Wall -Werror -Wextra -pedantic -pthread
endif

# https://stackoverflow.com/questions/30573481/path-include-and-src-directory-makefile/30602701
SRC_DIR := src
SRC := $(wildcard $(SRC_DIR)/*.c)

OBJ_DIR := $(SRC_DIR)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

EXEC = uartserver

ifeq ($(CC),)
	ifndef CROSS_COMPILE
		CC := gcc
	else
		CC := $(CROSS_COMPILE)gcc
	endif
endif

ifeq ($(LDFLAGS),)
	LDFLAGS := -lrt
endif

.PHONY: all $(EXEC) clean

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $^ $(CFLAGS) -o $@ $(LDFLAGS)

clean:
	rm -rf $(OBJ) $(EXEC)