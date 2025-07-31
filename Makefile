CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11 -D_GNU_SOURCE
LDFLAGS = 
TARGET = init
SRC_DIR = .
OUT_DIR = out
SRCS = main.c init.c networking.c mount.c services.c
OBJS = $(addprefix $(OUT_DIR)/, $(SRCS:.c=.o))
HEADERS = init.h networking.h mount.h services.h

.PHONY: all clean directories

all: directories $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(OUT_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

directories:
	@mkdir -p $(OUT_DIR)

clean:
	rm -rf $(OUT_DIR) $(TARGET)