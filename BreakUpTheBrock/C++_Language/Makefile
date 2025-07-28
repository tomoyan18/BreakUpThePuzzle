TARGET = sdlWindow
CXX = g++

PKG_CONFIG_PATH := /usr/local/Cellar/sdl2_ttf/2.24.0/lib/pkgconfig:/usr/local/lib/pkgconfig
CXXFLAGS += -O3 $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags sdl2 SDL2_ttf)
LDFLAGS += $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs sdl2 SDL2_ttf)

all: $(TARGET)

$(TARGET): main.o
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) *.o
