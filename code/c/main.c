/*  The purpose of this c program is to capture packets from a network device
 *  Then send these packets to the go program that will process and display these packets
 *  It will send these packets using a unix socket
 */

#include "packets/packets.h"
#include "socket/socket.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

void cleanup(int socket);

pcap_t *handler;

int main() {

  // Sets up all pcap stuff
  setupPcap(&handler);

  // Gets client socket
  int clientSocket = getClient();
  int flags = fcntl(clientSocket, F_GETFL, 0);
  fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK);

  // Time to get sniffn
  pcap_loop(handler, -1, myCallback, (u_char *)&clientSocket);

  // Cleanup
  cleanup(clientSocket);
  return 0;
}

void cleanup(int socket) {
  unlink("/tmp/packetlens.sock");
  printf("Cleaning");
  shutdown(socket, 2);
}

void myCallback(u_char *clSocket, const struct pcap_pkthdr *header, const u_char *packet) {
  if (packet == NULL) {
    fprintf(stderr, "Error: NULL PACKET!");
    return;
  }
  if (clSocket == NULL) {
    fprintf(stderr, "Error socket is null");
    return;
  }

  int clientSocket = *(int *)clSocket;

  char msg[256];
  int bytesRead = recv(clientSocket, msg, sizeof(msg), 0);

  if (bytesRead > 0) {

    if (strcmp(msg, "quit") == 0) {
      cleanup(clientSocket);
      pcap_breakloop(handler);
    }
  }

  int bytesSent = send(clientSocket, packet, header->len, 0);
  if (bytesSent == -1) {
    perror("Sent Failed");
    return;
  }
}
