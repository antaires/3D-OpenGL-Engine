build:
	g++ -w -std=c++14 -Wfatal-errors -L. -Lsrc/include/SDL2/SDL2 -Lsrc/include/SDL2 -Lsrc/include/SDL2/build/.libs -Isrc/include/GL -Isrc/include/SDL2 -Isrc/include ./src/*.cpp -o game -lGLEW -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo

clean:
	rm ./game

run:
	./game