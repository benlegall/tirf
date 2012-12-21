include ../Makefile.rules

OBJS=$(SRC:.c=.o)

all: PROJ

%.o:%.c
	$(CC) $(CFLAGS) -c $<  `pkg-config --cflags opencv` `pkg-config --libs opencv -g` -o $@ 

PROJ: $(OBJS)
	$(CC) $(CFLAGS) $(SRC) `pkg-config --cflags opencv` `pkg-config --libs opencv` -o ../$(PROJ)
