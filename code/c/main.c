/*  The purpose of this c program is to capture packets from a network device
 *  Then send these packets to the go program that will process and display these packets
 *  It will send these packets using a unix socket
 */

#include "packets/packets.h"
#include "socket/socket.h"

void cleanup(int socket);

int main() {

  // Sets up all pcap stuff
  pcap_t *handler;
  setupPcap(&handler);

  // Gets client socket
  int clientSocket = getClient();

  // Time to get sniffn
  pcap_loop(handler, -1, myCallback, (u_char *)&clientSocket);

  // Cleanup
  cleanup(clientSocket);
  return 0;
}

void cleanup(int socket) {

  shutdown(socket, 2);
  system("sudo rm /tmp/packetlens.sock");
}
