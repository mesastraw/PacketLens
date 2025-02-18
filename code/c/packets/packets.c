#include <pcap.h>
#include <stdlib.h>
#include "packets.h"

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


