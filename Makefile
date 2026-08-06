out = -o tui-raster
flags = -lm -O3

build: src/main.c src/obj-loader.c
	gcc src/main.c src/obj-loader.c src/matrix-math.c src/term.c src/scene-loader.c $(out) $(flags)

clean: tui-raster.o
	rm tui-raster
	gcc src/main.c src/obj-loader.c src/matrix-math.c src/term.c src/scene-loader.c $(out) $(flags)

obj-loader.c: include/obj-loader.h
matrix-math.c: include/matrix-math.h
term.c: include/term.h
scene-loader.c: include/scene-loader.h
