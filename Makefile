CFILES = libleddisplay.c
HFILES = libleddisplay.h
OFILES = $(CFILES:.c=.o)

# For creating object code for a library:
# CFLAGS = -fpic
# For creating a library:
# CFLAGS = -Wall -shared -W1,-soname,libleddisplay.so.1 -o libleddisplay.so.1.0.1 $(OFILES)
CFLAGS = -Wall -fpic

.PHONY: all
all: libleddisplay

.PHONY: clean
clean:
	rm -f *.o

.PHONY: test
test: displaytest

displaytest: libleddisplay.o displaytest.o
	$(CC) displaytest.o -L. -lleddisplay -lusb -o displaytest

libleddisplay: $(OFILES)
	ar rcs libleddisplay.a libleddisplay.o

#
#displaytest: displaytest.o
#	$(CC) displaytest.o -L. -lleddisplay -lusb -lc -o displaytest
#
#libleddisplay: $(OFILES)
#	$(CC) $(OFILES) -shared -W1,-soname,libleddisplay.so.1 -o libleddisplay.so.1.0.1
#	/sbin/ldconfig -n .
#	ln -sf libleddisplay.so.1.0.1 libleddisplay.so
