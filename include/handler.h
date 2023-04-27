#ifndef _HANDLER_H
#define _HANDLER_H

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <poll.h>

#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <linux/if_packet.h>

#define HEADER_SIZE (sizeof(struct ether_header))
#define FRAME_SIZE 1500
#define MAX_PAYLOAD_SIZE (FRAME_SIZE - HEADER_SIZE)

#define ETH_P_CTRL 0x4e44

struct raw_frame {
    struct ether_header eth;
    uint32_t value;
} __attribute__((packed));

int init_sock(char *if_name, struct ifreq *if_mac, struct sockaddr_ll *socket_address);
int send_value(int sockfd, struct ifreq *if_mac, struct sockaddr_ll *socket_address, uint32_t value);
int receive_result(int sockfd);

int handle(char *if_name, uint32_t value);

#endif // _HANDLER_H
