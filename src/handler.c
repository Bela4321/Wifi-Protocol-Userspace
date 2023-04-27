#include "helpers.h"
#include "handler.h"

int init_sock(char *if_name, struct ifreq *if_mac, struct sockaddr_ll *socket_address) {
    int res;
	int sockfd;
	struct ifreq if_idx;

	if ((sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_CTRL))) == -1) {
	    perror(">>> Could not create socket.");
        return sockfd;
	}

    memcpy(if_idx.ifr_name, if_name, IFNAMSIZ - 1);
    if ((res = ioctl(sockfd, SIOCGIFINDEX, &if_idx)) == -1) {
        perror(">>> Could not get the interface's index.");
        return res;
    }

    memcpy(if_mac->ifr_name, if_name, IFNAMSIZ - 1);
    if ((res = ioctl(sockfd, SIOCGIFHWADDR, if_mac)) == -1) {
        perror(">>> Could not get MAC address.");
        return res;
    }

    socket_address->sll_ifindex = if_idx.ifr_ifindex;
    socket_address->sll_halen = ETH_ALEN;

    return sockfd;
}

int send_value(int sockfd, struct ifreq *if_mac, struct sockaddr_ll *socket_address, uint32_t value) {
    struct raw_frame frame = {
        .eth = {
            .ether_shost = { 0x0 },
            .ether_dhost = { 0x0 },
            .ether_type = htons(ETH_P_CTRL)
        },
        .value = value
    };

    memcpy(frame.eth.ether_shost, if_mac->ifr_hwaddr.sa_data, 6);

    int tx_len = sizeof(struct ether_header) + sizeof(uint32_t);

    printf(">> Sending value %d.\n", value);
    int sent_bytes;
	if ((sent_bytes = sendto(sockfd, &frame, tx_len, 0, (struct sockaddr*)socket_address, sizeof(struct sockaddr_ll))) == -1) {
	    perror(">>> Could not send data.");
    }

    return sent_bytes;
}

int receive_result(int sockfd) {
    int res;
    ssize_t read_bytes;
    uint8_t recv_frame[FRAME_SIZE];
    memset(recv_frame, 0, FRAME_SIZE * sizeof (uint8_t));

    time_t runtime = 2000;

    struct pollfd pfd = {
        .fd = sockfd,
        .events = POLLIN
    };

    res = poll(&pfd, 1, runtime);

    if (res == -1) {
        perror(">>> Poll returned an error.");
        return res;
    }

    if (res == 0) {
        continue;
    }


    if (pfd.revents & POLLIN) {
        if ((read_bytes = recv(sockfd, recv_frame, FRAME_SIZE, 0)) >= 0) {
            printf(">> Received SBP frame.\n");
                /**
                 * YOUR CODE HERE
                 * As soon as a frame is received, you have to parse it
                 * accoringly and print the received value to stdout.
                 */
                //print the read bytes
                printf(">> Received %d bytes.\n", read_bytes);
                /**
                 * YOUR CODE HERE
                 */
        } else {
            if (errno == EINTR) {
                memset(recv_frame, 0, FRAME_SIZE * sizeof (uint8_t));
                perror(">>> Received signal from recv.");
                continue;
            } else {
                perror(">>> Could not receive data.");
                return read_bytes;
            }
        }
    }

    return 0;
}

int handle(char *if_name, uint32_t value) {
    int res;
	int sockfd;
	struct ifreq if_mac;
	struct sockaddr_ll socket_address;

    printf(">> Starting scan using Wi-Fi chip.\n");
    if ((sockfd = init_sock(if_name, &if_mac, &socket_address)) == -1) {
        return sockfd;
    }

    printf(">> Constructing value frame.\n");
    if ((res = send_value(sockfd, &if_mac, &socket_address, value)) == -1) {
        return res;
    }

    printf(">> Waiting for SBP Response.\n");
    if ((res = receive_result(sockfd)) < 0) {
        return res;
    }

    return 0;
}
