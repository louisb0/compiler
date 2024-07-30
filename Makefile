CC = gcc
CFLAGS = -Wall -Wextra -MMD -MP

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

$(BUILD_DIR)/compiler/%.o: compiler/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	rm -f $(TARGET) $(OBJS) $(DEPS)
	rm -rf $(BUILD_DIR)

.PHONY: clean
