packetCatcher: main.o packet.o socket.o
	gcc -lpcap -o packetCatcher main.o packets.o socket.o
main.o:
	gcc -lpcap -c main.c 
packet.o:
	gcc -lpcap -c ./packets/packets.c
socket.o:
	gcc -c ./socket/socket.c
clean:
	rm -f packetCatcher main.o packets.o socket.o
