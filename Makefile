build:
	gcc ./src/*.c `sdl2-config --libs --cflags` -lm -o voxelspace

run:
	./voxelspace

clean:
	rm voxelspace