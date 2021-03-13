CC			=  gcc
CFLAGS		= -Wall -pedantic
TARGETS		= es1 es2_create_mat es2_read_mat es3 #usa \ per andare a capo

.PHONY: all clean cleanall 
.SUFFIXES: .c .h

#CHIAMATA LANCIANDO make
all	: $(TARGETS)

#REGOLA PER LA GENERAZIONE DI UN ESEGUIBILE PARTENDO DA UN .c
% : %.c 
	$(CC) $(FLAGS) $< -o $@

#EQUIVALENTE ALLA REGOLA!!!
#es1 : es1.c 
#	$(CC) $(FLAGS) $< -o $@

#es2_create_mat : es2_create_mat.c
#	$(CC) $(FLAGS) $< -o $@
	
#es2_read_mat : es2_read_mat.c
#	$(CC) $(FLAGS) $< -o $@

#es3 : es3.c
#	$(CC) $(FLAGS) $< -o $@

#ELIMINA I TARGETS AKA GLI ESEGUIBILI CREATI
clean : 
	-rm -f $(TARGETS)

#ELIMINA I FILE ESEGUIBILI, OGGETTO E TEMPORANEI
cleanall :
	-rm -f $(TARGETS) *.o *~ 