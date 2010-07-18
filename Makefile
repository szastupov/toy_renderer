SDL_CFLAGS := $(shell pkg-config sdl --cflags)
SDL_LDFLAGS := $(shell pkg-config sdl --libs)

CFLAGS += $(SDL_CFLAGS) -Wall -MD -ggdb
LDLIBS += $(SDL_LDFLAGS)

CXXFLAGS += $(CFLAGS) -std=c++98

game: main.o transform.o canvas.o
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@

clean:
	rm -f *.o *.d game

-include *.d
