OBJ = main.o  http.o hum_temp.o basededatos.o libhttp.so libsensor.so  libbasededatos.so 
LDFLAGS = -fPIC -c -o
CC = gcc 
CFLAGS = -shared -fPIC -o 
LIB = -lhttp -lsensor -lbasededatos -lsqlite3
URL = /home/pi/Documents/Proyect_kernel_check

all: captura	
	export LD_LIBARY_PATH=.
	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(URL) ./captura -a 192.168.11.250 -b config.txt -c $(URL)/basededatos.db
captura: $(OBJ)
	$(CC) -o captura main.o -L$(URL) $(LIB)
http.o: http.c
	 $(CC) $(LDFLAGS) http.o http.c 

hum_temp.o: hum_temp.c
	 $(CC) $(LDFLAGS) hum_temp.o hum_temp.c

basededatos.o: basededatos.o
	$(CC) $(LDFLAGS) basededatos.o basededatos.c

libhttp.so: http.o
	$(CC) $(CFLAGS) libhttp.so http.o

libsensor.so: hum_temp.o
	$(CC) $(CFLAGS) libsensor.so hum_temp.o
libbasededatos.so: basededatos.o
	$(CC) $(CFLAGS) libbasededatos.so basededatos.o

main.o: main.c
	$(CC) -c -o main.o main.c -I$(URL)

clean:
	rm -f $(OBJ) *.db captura
