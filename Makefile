CC = gcc
TARGET = tui-raster
CFLAGS = -lm -O3

SRC = src/main.c src/obj-loader.c src/matrix-math.c src/term.c src/scene-loader.c

build: $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS)	

clean:
	rm -f $(TARGET)
