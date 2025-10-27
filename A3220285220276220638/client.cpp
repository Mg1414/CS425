// ---------------------------------------------------------------------------
// client.cpp — Raw TCP handshake “initiator” (ELI5 version)
// ---------------------------------------------------------------------------
// Sends a crafted SYN with seq=200 to the server, expects a SYN‑ACK back, then
// replies with an ACK to complete the 3‑way handshake. All headers are built by
// us to demonstrate low‑level networking.
// ---------------------------------------------------------------------------
#include <iostream>               // for std::cout, std::cerr
#include <cstring>                // for memset, memcpy
#include <cstdlib>                // for exit codes and stdlib functions
#include <sys/socket.h>           // for socket(), sendto(), recvfrom(), setsockopt()
#include <netinet/ip.h>           // for iphdr structure
#include <netinet/tcp.h>          // for tcphdr structure
#include <arpa/inet.h>            // for inet_pton(), inet_addr(), htons(), htonl(), ntohl()
#include <unistd.h>               // for close()

#define SERVER_IP "127.0.0.1"      // IP address of the server
#define SERVER_PORT 12345         // Port number the server is listening on

// Custom pseudo header used for computing TCP checksum
struct header_2 {
    u_int32_t src;                // Source IP address
    u_int32_t dst;                // Destination IP address
    u_int8_t blank;              // Always zero
    u_int8_t ptcl;               // Protocol number (TCP = 6)
    u_int16_t len;               // TCP segment length
};

// Computes Internet checksum over given data
unsigned short checksum(unsigned short *ptr, int bts) {
    long sum = 0;                 // Accumulator for the sum
    unsigned short odd;          // Temporary storage for leftover byte
    short ans;                   // Final result (inverted sum)

    while (bts > 1) {            // Sum up 16-bit words
        sum += *ptr++;
        bts -= 2;
    }

    if (bts == 1) {              // If there is a leftover byte
        odd = 0;
        *((u_char *)&odd) = *(u_char *)ptr;
        sum += odd;
    }

    uint32_t high = sum >> 16;   // Carry bits
    uint32_t low = sum & 0xFFFF; // Lower 16 bits
    sum = high + low;            // Add carry
    sum += (sum >> 16);          // Final carry, if any

    ans = static_cast<unsigned short>(~sum); // One's complement of sum
    return ans;
}

// Constructs and sends a TCP packet with specified flags and sequence numbers
void send_tcp_packet(int sock, struct sockaddr_in *dst_add, uint32_t seq, uint32_t ack_seq, int synflag, int ackflag) {
    char packet[4096];
    memset(packet, 0, 4096);     // Zero out the packet buffer

    struct iphdr *ip = (struct iphdr *)packet;                    // IP header
    struct tcphdr *tcp = (struct tcphdr *)(packet + sizeof(struct iphdr)); // TCP header

    // Fill in IP header fields
    ip->ihl = 5;                 // IP header length
    ip->version = 4;            // IPv4
    ip->tos = 0;                // Type of Service
    ip->tot_len = htons(sizeof(struct iphdr) + sizeof(struct tcphdr)); // Total length
    ip->id = htons(54321);      // Identification field
    ip->frag_off = 0;           // No fragmentation
    ip->ttl = 64;               // Time to live
    ip->protocol = IPPROTO_TCP; // Protocol type
    ip->saddr = inet_addr("127.0.0.1"); // Source IP
    ip->daddr = dst_add->sin_addr.s_addr; // Destination IP
    ip->check = checksum((unsigned short *)packet, ip->ihl << 2); // IP checksum

    // Fill in TCP header fields
    tcp->source = htons(1234);  // Arbitrary source port
    tcp->dest = htons(SERVER_PORT); // Destination port
    tcp->seq = htonl(seq);      // Sequence number
    tcp->ack_seq = htonl(ack_seq); // Acknowledgment number
    tcp->doff = 5;              // Data offset (TCP header length)
    tcp->syn = synflag;         // SYN flag
    tcp->ack = ackflag;         // ACK flag
    tcp->window = htons(8192);  // Window size
    tcp->check = 0;             // Initial checksum value

    // Create pseudo header for checksum calculation
    struct header_2 psh;
    psh.src = inet_addr("127.0.0.1");
    psh.dst = dst_add->sin_addr.s_addr;
    psh.blank = 0;
    psh.ptcl = IPPROTO_TCP;
    psh.len = htons(sizeof(struct tcphdr));

    // Combine pseudo header and TCP header for checksum computation
    char msg[4096];
    memcpy(msg, &psh, sizeof(struct header_2));
    memcpy(msg + sizeof(struct header_2), tcp, sizeof(struct tcphdr));

    tcp->check = checksum((unsigned short *)msg, sizeof(struct header_2) + sizeof(struct tcphdr));

    // Send packet to destination
    if (sendto(sock, packet, sizeof(struct iphdr) + sizeof(struct tcphdr), 0, (struct sockaddr *)dst_add, sizeof(*dst_add)) < 0) {
        perror("could not perform sendto"); // Error sending packet
    }
}

// Listens for a TCP packet with expected SYN-ACK values
bool receive_tcp_packet(int sock, struct sockaddr_in *addr, socklen_t *addrlen, struct tcphdr *out_tcp) {
    char buffer[4096];
    while (true) {
        ssize_t len = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)addr, addrlen);
        if (len < 0) {
            perror("recvfrom"); // Error receiving packet
            return false;
        }

        struct iphdr *ip = (struct iphdr *)buffer;                         // Extract IP header
        struct tcphdr *tcp = (struct tcphdr *)(buffer + ip->ihl * 4);      // Extract TCP header

        // Check if the packet is the expected SYN-ACK
        if (tcp->syn == 1 && tcp->ack == 1 && ntohl(tcp->seq) == 400 && ntohl(tcp->ack_seq) == 201) {
            *out_tcp = *tcp;                                               // Save TCP header
            std::cout << "SYN-ACK sent from server to client" << std::endl;
            return true;
        }
    }
}

int main() {
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);        // Create raw socket for TCP
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    int one = 1;
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) { // Allow custom IP headers
        perror("setsockopt");
        return 1;
    }

    struct sockaddr_in dst_add;                // Destination address struct
    dst_add.sin_family = AF_INET;              // IPv4
    dst_add.sin_port = htons(SERVER_PORT);     // Destination port
    inet_pton(AF_INET, SERVER_IP, &dst_add.sin_addr); // Convert IP address

    std::cout << "SYN response with sequence number 200 being sent" << std::endl;
    send_tcp_packet(sock, &dst_add, 200, 0, 1, 0);   // Send SYN packet

    struct tcphdr syn_ack_tcp;                      // Struct to hold received TCP header
    socklen_t addr_len = sizeof(dst_add);
    if (!receive_tcp_packet(sock, &dst_add, &addr_len, &syn_ack_tcp)) {
        std::cerr << "SYN-ACK could not be received from the server " << std::endl;
        close(sock);
        return 1;
    }

    std::cout << "ACK response with sequence number 600 and ack_seq, 401 being sent" << std::endl;
    send_tcp_packet(sock, &dst_add, 600, 401, 0, 1); // Send final ACK

    std::cout << "handshake was succesfull" << std::endl;
    close(sock); // Close the socket
    return 0;
}
