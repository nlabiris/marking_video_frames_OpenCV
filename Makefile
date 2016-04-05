# This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
# (http://creativecommons.org/licenses/by-nc-sa/4.0/)
#
# ------------------------------------------------------------------------
#
# This Makefile compiles the source code for all the algorithms used
#
# Usage: make all				# compile all algorithms
#        make <algorithm_name>  # compile the source code for a specific algorithm
#        make clean				# delete any object files
# 
# This generic makefile calls a specific makefile for each algorithm
# These makefiles are:
#
# Makefile_absdiff
# Makefile_comprdiff
# Makefile_ecrdiff_v1
# Makefile_ecrdiff_v2

.PHONY: all clean absdiff comprdiff ecrdiff_v1 ecrdiff_v2

all:
	$(MAKE) -f Makefile_absdiff
	$(MAKE) -f Makefile_comprdiff
	$(MAKE) -f Makefile_ecrdiff_v1
	$(MAKE) -f Makefile_ecrdiff_v2

absdiff:
	$(MAKE) -f Makefile_absdiff

comprdiff:
	$(MAKE) -f Makefile_comprdiff

ecrdiff_v1:
	$(MAKE) -f Makefile_ecrdiff_v1

ecrdiff_v2:
	$(MAKE) -f Makefile_ecrdiff_v2
	
clean:
	rm -f src/*.o
	rm -f src/absdiff/*.o
	rm -f src/comprdiff/*.o
	rm -f src/ecrdiff_v1/*.o
	rm -f src/ecrdiff_v2/*.o