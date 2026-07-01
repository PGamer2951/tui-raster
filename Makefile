build: src/main.c
	gcc src/main.c -o tui-raster.o -lm

clean: tui-raster.o
	rm tui-raster.o
	gcc src/main.c -o tui-raster.o -lm