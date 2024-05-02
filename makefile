CC = gcc
CFLAGS = -ansi -Wall -pedantic -g
GLOBAL_DEPS = utils.h macro.h data_structur.h preprocessor.h first_run.h data_table.h second_run.h # Add all header files as global dependencies

# List of source files
SRCS = preprocessor.c utils.c macro.c data_structur.c compiler.c first_run.c data_table.c second_run.c # Add data_structur.c and preprosessor.c to the list of source files

# List of object files corresponding to source files
OBJS = $(SRCS:.c=.o)

# The target executable
TARGET = compiler

# Rule to build the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Rules to compile each source file into an object file
%.o: %.c $(GLOBAL_DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

# Phony target to clean up generated files
.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJS) *.am *.ob *.ent *.ext

