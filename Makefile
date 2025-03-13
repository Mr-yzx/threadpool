CC = gcc
LDFLAGS = -lpthread
CFLAGS = -std=c11 -O1
TARGET = myApp
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRCS))

ALL: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

.PHONY: clean
clean:
	@rm -rf $(TARGET) *.o
