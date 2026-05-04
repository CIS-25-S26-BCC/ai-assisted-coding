CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra
TARGET   := platformer
SRCS     := main.cpp src/map.cpp src/player.cpp src/renderer.cpp

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)

.PHONY: clean
