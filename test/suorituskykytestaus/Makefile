NAME = suorituskyky

SRC = ${NAME}.c ../pitchdetect.c ../params.c ../freqandpitch.c

OBJ = ${SRC:.c=.o}

INCS = -I. -I/usr/include
LIBS = -L/usr/lib -lasound -lm

CFLAGS   = -g -std=c99 -pedantic -Wall ${INCS}
LDFLAGS  = -g ${LIBS}

CC = cc

all: options ${NAME}

${NAME}: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

options:
	@echo ${NAME} build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"
