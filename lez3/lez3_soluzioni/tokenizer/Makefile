# ---------------------------------------------------------------------------
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License version 2 as 
#  published by the Free Software Foundation.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
#  As a special exception, you may use this file as part of a free software
#  library without restriction.  Specifically, if other files instantiate
#  templates or use macros or inline functions from this file, or you compile
#  this file and link it with other files to produce an executable, this
#  file does not by itself cause the resulting executable to be covered by
#  the GNU General Public License.  This exception does not however
#  invalidate any other reasons why the executable file might be covered by
#  the GNU General Public License.
#
# ---------------------------------------------------------------------------

# compilatore da usare
CC		=  gcc
# aggiungo alcuni flags di compilazione
CCFLAGS	        += -std=c99 -Wall
# gli includes sono nella directory includes
INCDIR          = ./includes
# cerco gli include oltre che in INCDIR anche nella dir. corrente
INCLUDES	= -I. -I $(INCDIR)
# librerie da linkare a tutti i files (es. -lm per le librerie matematiche)
LDFLAGS 	=
# altre librerie  (es. -L<path> -lmylib)
LIBS1		= -L ./libdir  -ltokenizer
LIBS2           = -L ./libdir2 -ltokenizer
# flags di ottimizzazione 
OPTFLAGS	= # -O3
# flags di debugging
DBGFLAGS        = -g
FLAGS           = $(DBGFLAGS) $(OPTFLAGS)
ARFLAGS         = rvs

TARGETS		= bin/tokenizer_main \
		  bin/tokenizer_main2

.PHONY: all clean cleanall
.SUFFIXES: .c .h

libdir/%.o: %.c
	$(CC) $(CCFLAGS) $(INCLUDES) $(FLAGS) -c -o $@ $<
libdir2/%.o: %.c
	$(CC) $(CCFLAGS) $(INCLUDES) $(FLAGS) -c -fPIC -o $@ $<


# root target
all		: $(TARGETS)

# NOTA: $^ prende tutte le occorrenze della dependency list
#       $< prende solo la prima entry della dependency list
bin/tokenizer_main: libdir/tokenizer_main.o libdir/libtokenizer.a 
	$(CC)  $(CFLAGS) $(INCLUDES) $(FLAGS) -o $@ $< $(LDFLAGS) $(LIBS1) 

bin/tokenizer_main2: libdir2/tokenizer_main.o libdir2/libtokenizer.so
	$(CC) $(CFLAGS) $(INCLUDES) $(FLAGS) -o $@ $< -Wl,-rpath=${PWD}/libdir,-rpath=${PWD}/libdir2  $(LDFLAGS) $(LIBS2)

libdir/libtokenizer.a: libdir/tokenizer_lib.o $(INCDIR)/tokenizer.h
	$(AR) $(ARFLAGS) $@ $<

libdir2/libtokenizer.so: libdir2/tokenizer_lib.o $(INCDIR)/tokenizer.h
	$(CC) -shared -o $@ $<

clean		: 
	-rm -f $(TARGETS)
cleanall	: clean
	-rm -f *.o *~ libtokenizer.a libdir/* libdir2/*



