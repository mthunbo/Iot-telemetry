CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude -I../common/include
LIBS = -lmosquitto

BUILD = build
TARGET = telemetry-processor

LOCAL_SRCS = $(wildcard src/*.cpp)
COMMON_SRCS = ../common/src/mqtt_shared.cpp

LOCAL_OBJS = $(patsubst src/%.cpp, $(BUILD)/%.o, $(LOCAL_SRCS))
COMMON_OBJS = $(patsubst ../common/src/%.cpp, $(BUILD)/common/%.o, $(COMMON_SRCS))

OBJS = $(LOCAL_OBJS) $(COMMON_OBJS)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

$(BUILD)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/common/%.o: ../common/src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD) $(TARGET)

.PHONY: all clean