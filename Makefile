build: src/main.c
	gcc src/main.c -o tui-raster.o -lm

clean: main.o
	rm main.o
	gcc src/main.c -o tui-raster.o -lm