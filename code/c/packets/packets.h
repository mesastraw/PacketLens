#ifndef PACKETS_H
#define PACKETS_H
#include <pcap.h>

void myCallback(u_char *Socket, const struct pcap_pkthdr *header, const u_char *packet);
void setupPcap(pcap_t **handler);

#endif
