# This Makefile compiles the source code for all the algorithms used

CFLAGS=`pkg-config --cflags opencv` -Wall -I include
LIBS=`pkg-config --libs opencv`
ABSDIFF_TARGET=bin/absdiff/absdiff_export_images
COMPRDIFF_TARGET=bin/comprdiff/comprdiff_export_images
.PHONY: all clean

# Compile all algorithms at once
all: $(ABSDIFF_TARGET) $(COMPRDIFF_TARGET)

# Absolute differences algorithm
$(ABSDIFF_TARGET): src/absdiff/absdiff_export_images.o src/video_algorithms.o
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@
absdiff_export_images.o: src/absdiff/absdiff_export_images.c src/video_algorithms.c include/video_algorithms.h
	$(CC) $(CFLAGS) $(LIBS) -c $<

# Compression differences algorithm
$(COMPRDIFF_TARGET): src/comprdiff/comprdiff_export_images.o src/video_algorithms.o
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@
comprdiff_export_images.o: src/comprdiff/comprdiff_export_images.c src/video_algorithms.c include/video_algorithms.h
	$(CC) $(CFLAGS) $(LIBS) -c $<

# Video algorithms
video_algorithms.o: src/video_algorithms.c include/video_algorithms.h
	$(CC) $(CFLAGS) $(LIBS) -c $<

# Clean object files
clean:
	rm src/*.o
	rm src/absdiff/*.o
	rm src/comprdiff/*.o