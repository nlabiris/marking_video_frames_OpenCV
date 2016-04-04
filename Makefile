# This Makefile compiles the source code for all the algorithms used

CFLAGS=`pkg-config --cflags opencv` -Wall -I include
LIBS=`pkg-config --libs opencv`
ABSDIFF_TARGET=src/absdiff/absdiff_export_images

$(ABSDIFF_TARGET): src/absdiff/absdiff_export_images.o src/video_algorithms.o
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@
absdiff_export_images.o: src/absdiff/absdiff_export_images.c src/video_algorithms.c include/video_algorithms.h
	$(CC) $(CFLAGS) $(LIBS) -c $<
video_algorithms.o: src/video_algorithms.c include/video_algorithms.h
	$(CC) $(CFLAGS) $(LIBS) -c $<

.PHONY: clean
clean:
	rm src/*.o
	rm src/absdiff/*.o