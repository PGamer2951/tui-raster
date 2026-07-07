command = gcc src/main.c src/obj-loader.c src/matrix-math.c -o tui-raster.o -lm

build: src/main.c src/obj-loader.c
	$(command)

clean: tui-raster.o
	rm tui-raster.o
	$(command)

obj-loader.c: include/obj-loader.h