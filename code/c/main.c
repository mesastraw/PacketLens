/*  The purpose of this c program is to capture packets from an interface
 *  Then send these packets to the go program
 *  Add only one instance using flock
 */

#include <pcap.h>
#include <pcap/pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

// This is my callback function passed to pcap_loop and is called each time a
// packet is recieved and it will send packets to the go program
void myCallback(u_char *args, const struct pcap_pkthdr *pkhdr, const u_char *packet) {}

// Packet Handling Functions
void setupFilters(pcap_t **handle, struct bpf_program *fp, char filters[], bpf_u_int32 *net);
pcap_t *setupDevice(bpf_u_int32 *net, bpf_u_int32 *mask);

int main() {
  pcap_t *handler;               // Session handler
  char filters[] = "tcp or udp"; // This is our filters
  struct bpf_program fp;         // This holds our compiled filters
  bpf_u_int32 submask;           // mask of sniffer device
  bpf_u_int32 net;               // Ip of sniffer device

  // Sockets and stuff
  struct sockaddr_un serverAddr;      // Our server
  struct sockaddr_storage clientAddr; // Our client who we are accepting a connection from

  int sck = socket(AF_UNIX, SOCK_STREAM, 0);

  serverAddr.sun_family = AF_UNIX;                     // Says this is a unix socket
  strcpy(serverAddr.sun_path, "/tmp/packetlens.sock"); // This sets our unix socket file
  if (bind(sck, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
    perror("Error binding to socket: ");
    exit(EXIT_FAILURE);
  }

  // Listen for connections 5 max queus
  if (listen(sck, 5) == -1) {
    perror("Couldnt Get connection: ");
    exit(EXIT_FAILURE);
  }

  // Accept connection from client
  socklen_t clientLen = sizeof(clientAddr);
  int clientSocket = accept(sck, (struct sockaddr *)&clientSocket, &clientLen);

  // Write data to client
  char *data = "Hello Go!!";
  // Put send in callback function to send packts
  send(clientSocket, data, strlen(data), 0);

  // Close connection dont need it right now
  shutdown(clientSocket, 2);

  /*
  // This part of the program is all for getting packets
  setupDevice(&net, &submask);
  // Compiles and sets filters
  setupFilters(&handler, &fp, filters, &net);
  // Time to get sniffn
  pcap_loop(handler, -1, myCallback, NULL);
  */
  return 0;
}

pcap_t *setupDevice(bpf_u_int32 *net, bpf_u_int32 *subMask) {
  pcap_if_t *dev;                // list of devices
  char errbuf[PCAP_ERRBUF_SIZE]; // Error String

  // First device is our network device
  if (pcap_findalldevs(&dev, errbuf) == PCAP_ERROR) {
    fprintf(stderr, "Couldnt find device: %s\n", errbuf);
    exit(EXIT_FAILURE);
  }

  // Get ip and mask of our device
  if (pcap_lookupnet(dev->name, net, subMask, errbuf) == PCAP_ERROR) {
    fprintf(stderr, "Can't get netmask for device %s\n", errbuf);
    net = 0;
    subMask = 0;
  }

  pcap_t *handler = pcap_open_live(dev->name, BUFSIZ, 1, 1000, errbuf);
  if (handler == NULL) {
    fprintf(stderr, "Couln't open device: %s\n", errbuf);
    exit(EXIT_FAILURE);
  }

  pcap_freealldevs(dev);
  return handler;
}

void setupFilters(pcap_t **handler, struct bpf_program *fp, char filters[],
                  bpf_u_int32 *net) {
  // Compile filters
  if (pcap_compile(*handler, fp, filters, 0, *net) == PCAP_ERROR) {
    fprintf(stderr, "Couldnt't parse filters %s: %s\n", filters,
            pcap_geterr(*handler));
    exit(EXIT_FAILURE);
  }

  // Set our filters
  if (pcap_setfilter(*handler, fp) == PCAP_ERROR) {
    fprintf(stderr, "Error Setting filter: %s: %s", filters,
            pcap_geterr(*handler));
    exit(EXIT_FAILURE);
  }
}
