IDIR = ../core/include
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=obj
LDIR =../lib

LIBS=-lm

_DEPS = core_fsm.h queue_manager.h 
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = my_NFC_UserSpace_App.o queue_manager.o application_handler.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

app: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
callback_tester:
	gcc callback_tester.c queue_manager.c -o callback_tester -I ../core/include/ 
