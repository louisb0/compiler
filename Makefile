# Compiler
CC = gcc
CFLAGS = -Wall -Wextra -MMD -MP

# Target
TARGET = bin

# Source files and object files
SRCS = main.c compiler/lexer.c
OBJS = $(SRCS:.c=.o)
DEPS = $(OBJS:.o=.d)

# Build target
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

compiler/%.o: compiler/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Include dependencies
-include $(DEPS)

# Clean build files
clean:
	rm -f $(TARGET) $(OBJS) $(DEPS)

# Phony targets
.PHONY: clean
