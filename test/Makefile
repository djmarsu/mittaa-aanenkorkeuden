SRC_FILES = tests.c ../pitchdetect.c ../params.c

all: test

test: $(SRC_FILES)
	gcc -std=c99 -o test $(SRC_FILES) -lm
	./test
