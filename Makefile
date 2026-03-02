CXX = g++ -std=c++17
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

$(BUILD_DIR)/config.txt: $(SRC_DIR)/config.txt | $(BUILD_DIR)
	cp $(SRC_DIR)/config.txt $(BUILD_DIR)/config.txt

$(TARGET): $(OBJECTS) $(BUILD_DIR)/config.txt
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

$(BUILD_DIR)/LoadBalancer.o: $(SRC_DIR)/LoadBalancer.cpp $(SRC_DIR)/LoadBalancer.h | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/LoadBalancer.cpp -o $(BUILD_DIR)/LoadBalancer.o
$(BUILD_DIR)/Request.o: $(SRC_DIR)/Request.cpp $(SRC_DIR)/Request.h | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS)  -c $(SRC_DIR)/Request.cpp -o $(BUILD_DIR)/Request.o
$(BUILD_DIR)/Server.o: $(SRC_DIR)/Server.cpp $(SRC_DIR)/Server.h | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/Server.cpp -o $(BUILD_DIR)/Server.o
$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/main.cpp -o $(BUILD_DIR)/main.o

clean:
	rm -r $(BUILD_DIR)

