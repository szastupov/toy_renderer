SDL_CFLAGS := $(shell sdl-config --cflags)
SDL_LDFLAGS := $(shell sdl-config --libs)

CFLAGS += $(SDL_CFLAGS) -Wall -MD -ggdb 
LDFLAGS += $(SDL_LDFLAGS)

CXXFLAGS += $(CFLAGS)

game: main.o transform.o canvas.o
	$(LINK.cc) $^ -o $@

clean:
	rm -f *.o *.d game

-include *.d
