CC=gcc
CFLAGS=-Wall -Werror -fpic
LIBFOLDER="$(shell pwd)"
LIBNAME=loop

# Archivos objeto
LIB_OBJECTS=mainloop.o
EXECUTABLE=mysh

# Regla por defecto
all: lib$(LIBNAME).so $(EXECUTABLE)

# Reglas de compilación
%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

lib$(LIBNAME).so: $(LIB_OBJECTS)
	$(CC) $(CFLAGS) -shared -o $@ $^

$(EXECUTABLE): main.c lib$(LIBNAME).so
	$(CC) $(CFLAGS) -L$(LIBFOLDER) -o $@ main.c -l$(LIBNAME)

# Limpiar archivos compilados
.PHONY: clean
clean:
	rm -f *.o lib$(LIBNAME).so $(EXECUTABLE)

