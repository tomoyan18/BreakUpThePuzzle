TARGET = sdlWindow
CXX = g++

PKG_CONFIG_PATH := /usr/local/Cellar/sdl2_ttf/2.24.0/lib/pkgconfig:/usr/local/lib/pkgconfig
CXXFLAGS += -std=c++11 -O3 \
    -I./ \
    -I./Games \
    -I./Balls \
    -I./Paddles \
    -I./Blocks \
    -I./Items \
    -I./Stages \
    $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags sdl2 SDL2_ttf)

LDFLAGS += $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs sdl2 SDL2_ttf)

SRCS = main.cpp \
    Games/Game.cpp \
    Balls/Ball.cpp \
    Paddles/Paddle.cpp \
    Blocks/Block.cpp \
    Items/Item.cpp \
    Stages/Stage.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
