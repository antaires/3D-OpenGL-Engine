build:
	g++ -w -std=c++14 -Wfatal-errors \
	./src/*.cpp \
	-o game \
	-lSDL2 \
	-lSDL2_image \
	-lSDL2_ttf \
	-lSDL2_mixer \
  -lGLEW \
  -framework OpenGL;

clean:
	rm ./game;

run:
	./game;
