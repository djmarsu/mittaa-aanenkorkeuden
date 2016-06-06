SRC_FILES=minunit_example.c ../pitchdetect.c

all: test

test: $(SRC_FILES)
	gcc -std=c99 -o test $(SRC_FILES) -lm
	./test
