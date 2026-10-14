CC = gcc

CFLAGS = -Wall -Wextra -Iinclude

TARGET = msgbus

SRC = src/main.c \
      src/broker.c \
      src/publisher.c \
      src/subscriber.c \
      src/ipc.c \
      src/signal_handler.c \
      src/memory.c \
      src/logger.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
