CXX = g++
CXXFLAGS = -Wall -Werror

SRC_DIR = src
BUILD_DIR = build
TARGET  = $(BUILD_DIR)/Load-Balancer
OBJECTS = $(BUILD_DIR)/LoadBalancer.o \
       $(BUILD_DIR)/Request.o \
       $(BUILD_DIR)/Server.o \
       $(BUILD_DIR)/main.o

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

LoadBalancer.o: $(SRC_DIR)/LoadBalancer.cpp $(SRC_DIR)/LoadBalancer.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/LoadBalancer.cpp -o $(BUILD_DIR)/LoadBalancer.o
Request.o: $(SRC_DIR)/Request.cpp $(SRC_DIR)/Request.h
	$(CC) $(CFLAGS)  -c $(SRC_DIR)/Request.cpp -o $(BUILD_DIR)/Request.o

Server.o: $(SRC_DIR)/Server.cpp $(SRC_DIR)/Server.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/Server.cpp -o $(BUILD_DIR)/Server.o

main.o: $(SRC_DIR)/main.cpp
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main.cpp -o $(BUILD_DIR)/main.o

clean:
	rm -f $(BUILD_DIR)

