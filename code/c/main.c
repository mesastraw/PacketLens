/*  The purpose of this c program is to capture packets from an interface
 *  Then send these packets to the go program
 *  using a unix socket
 */

#include <pcap.h>
#include <pcap/pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

void myCallback(u_char *Socket, const struct pcap_pkthdr *header, const u_char *packet);
void setupPcap(pcap_t **handler);
void cleanup();

int main() {
  pcap_t *handler; // Session handler

  // Sets up all pcap stuff
  setupPcap(&handler);

  // Sockets and stuff
  struct sockaddr_un serverAddr;                      // Our server
  struct sockaddr_storage clientAddr;                 // Our client who we are accepting a connection from
  int serverSocket = socket(AF_UNIX, SOCK_STREAM, 0); // Our servers unix socket

  // Makes sure unix socket is gone
  cleanup();

  serverAddr.sun_family = AF_UNIX;
  strcpy(serverAddr.sun_path, "/tmp/packetlens.sock");
  int serverLen = sizeof(serverAddr);

  // Bind the server to the unix socket
  if (bind(serverSocket, (struct sockaddr *)&serverAddr, serverLen) == -1) {
    perror("Error binding to socket: ");
    return 1;
  }

  // Listen for connections 5 max queus
  if (listen(serverSocket, 5) == -1) {
    perror("Couldnt Get connection: ");
    exit(EXIT_FAILURE);
  }

  // Accept connection from client
  socklen_t clientLen = sizeof(clientAddr);
  int clientSocket = accept(serverSocket, (struct sockaddr *)&clientSocket, &clientLen);
  if (clientSocket == -1) {
    fprintf(stderr, "BAD CLIENT SOCKET");
    exit(EXIT_FAILURE);
  }

  // Time to get sniffn
  pcap_loop(handler, -1, myCallback, (u_char *)&clientSocket);

  // Cleanup
  shutdown(clientSocket, 2);
  cleanup();
  return 0;
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

  int bytesSent = send(clientSocket, packet, header->len, 0);
  if (bytesSent == -1) {
    perror("Sent Failed");
    return;
  }
}

void setupPcap(pcap_t **handler) {
  char filters[] = "tcp or udp"; // This is our filters
  pcap_if_t *devs;               // list of devices
  char errbuf[PCAP_ERRBUF_SIZE]; // Error String
  struct bpf_program fp;         // This holds our compiled filters
  bpf_u_int32 subMask;           // mask of network device
  bpf_u_int32 net;               // Ip of network device

  // First device is our network device
  if (pcap_findalldevs(&devs, errbuf) == PCAP_ERROR) {
    fprintf(stderr, "Couldnt find device: %s\n", errbuf);
    exit(EXIT_FAILURE);
  }

  pcap_if_t *dev = devs;
  // Get ip and mask of our device
  if (pcap_lookupnet(dev->name, &net, &subMask, errbuf) == PCAP_ERROR) {
    fprintf(stderr, "Can't get netmask for device %s\n", errbuf);
    net = 0;
    subMask = 0;
  }

  *handler = pcap_open_live(dev->name, BUFSIZ, 1, 1000, errbuf);
  if (*handler == NULL) {
    fprintf(stderr, "Couln't open device: %s\n", errbuf);
    exit(EXIT_FAILURE);
  }

  pcap_freealldevs(devs);

  // Compile filters
  if (*handler == NULL) {
    fprintf(stderr, "HANDLER IS NULL");
    return;
  }

  if (pcap_compile(*handler, &fp, filters, 0, net) == PCAP_ERROR) {
    fprintf(stderr, "Couldnt't parse filters %s: %s\n", filters,
            pcap_geterr(*handler));
    exit(EXIT_FAILURE);
  }

  // Set our filters
  if (pcap_setfilter(*handler, &fp) == PCAP_ERROR) {
    fprintf(stderr, "Error Setting filter: %s: %s", filters,
            pcap_geterr(*handler));
    exit(EXIT_FAILURE);
  }
}

void cleanup() {
  system("sudo rm /tmp/packetlens.sock");
}
