CC=gcc
CFLAGS=-g -pedantic -std=gnu17 -Wall -Wextra -pthread
LIBS=-lm

# the build target executable:
TARGET=nyuenc

# Source files
SRCS=nyuenc.c result.c task.c result.h task.h

# Object files
OBJS=$(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# Pattern rule for object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(TARGET) $(OBJS)