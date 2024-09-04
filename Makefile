CC = gcc
CFLAGS = -Wall -g -Wextra -MMD -MP
PARSER_CFLAGS = $(CFLAGS) -Wno-unused-parameter

TARGET = bin

SRCS = main.c $(wildcard compiler/*.c)
BUILD_DIR = build
OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)/compiler

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/compiler/parser.o: compiler/parser.c | $(BUILD_DIR)
	$(CC) $(PARSER_CFLAGS) -c $< -o $@

$(BUILD_DIR)/compiler/%.o: compiler/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	rm -f $(TARGET) $(OBJS) $(DEPS)
	rm -rf $(BUILD_DIR)

.PHONY: clean
