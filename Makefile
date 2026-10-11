CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
CPPFLAGS = -Iinclude -Iexternal/minifb/include
LDFLAGS = -Lexternal/minifb/lib
LDLIBS = -lminifb -lX11 -lXrandr -lXcursor -lGL -lxkbcommon -lm
TARGET = build
SOURCES := $(wildcard src/*.cpp)
OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS) $(LDLIBS)
%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
clean:
	rm -f $(OBJECTS) $(TARGET)
run: $(TARGET)
	./$(TARGET)