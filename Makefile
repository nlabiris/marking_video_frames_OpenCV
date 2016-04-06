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

.PHONY: all clean help absdiff comprdiff ecrdiff_v1 ecrdiff_v2

all:
	@echo "\033[1;32mStarting compilation...\033[0m"
	@echo ""
	@echo "\033[1;32mCompiling absdiff...\033[0m"
	$(MAKE) -f Makefile_absdiff
	@echo "\033[1;32mCompiling comprdiff...\033[0m"
	$(MAKE) -f Makefile_comprdiff
	@echo "\033[1;32mCompiling ecrdiff_v1...\033[0m"
	$(MAKE) -f Makefile_ecrdiff_v1
	@echo "\033[1;32mCompiling ecrdiff_v2...\033[0m"
	$(MAKE) -f Makefile_ecrdiff_v2
	@echo "\033[1;32mCompiling histdiff...\033[0m"
	$(MAKE) -f Makefile_histdiff
	@echo ""
	@echo "\033[1;32mDone.\033[0m"

absdiff:
	@echo "\033[1;32mCompiling absdiff...\033[0m"
	$(MAKE) -f Makefile_absdiff
	@echo "\033[1;32mDone.\033[0m"

comprdiff:
	@echo "\033[1;32mCompiling comprdiff...\033[0m"
	$(MAKE) -f Makefile_comprdiff
	@echo "\033[1;32mDone.\033[0m"

ecrdiff_v1:
	@echo "\033[1;32mCompiling ecrdiff_v1...\033[0m"
	$(MAKE) -f Makefile_ecrdiff_v1
	@echo "\033[1;32mDone.\033[0m"

ecrdiff_v2:
	@echo "\033[1;32mCompiling ecrdiff_v2...\033[0m"
	$(MAKE) -f Makefile_ecrdiff_v2
	@echo "\033[1;32mDone.\033[0m"
	
histdiff:
	@echo "\033[1;32mCompiling histdiff...\033[0m"
	$(MAKE) -f Makefile_histdiff
	@echo "\033[1;32mDone.\033[0m"
	
clean:
	@echo "\033[1;32mDeleting object files...\033[0m"
	rm -f src/*.o
	rm -f src/absdiff/*.o
	rm -f src/comprdiff/*.o
	rm -f src/ecrdiff_v1/*.o
	rm -f src/ecrdiff_v2/*.o
	rm -f src/histdiff/*.o
	@echo "\033[1;32mDone.\033[0m"
	
help:
	@echo "Usage: make all                 # compile all algorithms"
	@echo "       make <algorithm_name>    # compile the source code"
	@echo "                                # for a specific algorithm"
	@echo "       make clean               # delete any object files"