CXX = g++
CC = gcc

CPPFLAGS = -Iinclude -I../common/include -Ivendor/bosch
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
CFLAGS   = -std=c11 -Wall -Wextra -O2
LIBS     = -lmosquitto

BUILD = build
TARGET = sensor-publisher

CXX_SOURCES = src/main.cpp \
              src/config.cpp \
              ../common/src/mqtt_shared.cpp \
              src/linux_i2c.cpp \
              src/bme280_sensor.cpp

C_SOURCES = vendor/bosch/bme280.c

CXX_OBJECTS = $(patsubst %.cpp, $(BUILD)/%.o, $(CXX_SOURCES))
C_OBJECTS = $(patsubst %.c, $(BUILD)/%.o, $(C_SOURCES))

all: $(TARGET)

$(TARGET): $(CXX_OBJECTS) $(C_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

$(BUILD)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD) $(TARGET)

.PHONY: all clean