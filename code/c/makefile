packetCatcher: main.o
	gcc -lpcap -o packetCatcher main.o
main.o:
	gcc -lpcap -c main.c
clean:
	rm -f packetCatcher main.o
