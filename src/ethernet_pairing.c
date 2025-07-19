/*
* This file is used to demonstrate how to create a virtual ethernet pair in C.
* This pair can be used as a network tunnel between two namespaces.
* Packets on one end appear at the other as though they were received from another device.
 * */
#define _GNU_SOURCE

#include <linux/if.h>
#include <linux/rtnetlink.h> // socket based protocol used to communicate between kernel and userspace
#include <linux/if_link.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define IFLA_INFO_PEER 3 // unexpectedly doesnt seem to be in if_link.h, hence this macro

struct {
  struct nlmsghdr nlh;
  struct ifinfomsg ifm;
  char buffer[1024];
} req;

int main() {
    int nl_socket = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (nl_socket < 0) {
        perror("Failed to create Netlink socket");
        exit(EXIT_FAILURE);
    }

    memset(&req, 0, sizeof(req));

    req.nlh.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
    req.nlh.nlmsg_type = RTM_NEWLINK;
    req.nlh.nlmsg_flags = NLM_F_REQUEST | NLM_F_CREATE | NLM_F_EXCL;
    req.ifm.ifi_family = AF_UNSPEC;

    struct rtattr *linkinfo = (struct rtattr *)((char *)&req + NLMSG_ALIGN(req.nlh.nlmsg_len));
    linkinfo->rta_type = IFLA_LINKINFO;
    linkinfo->rta_len = RTA_LENGTH(0);

    struct rtattr *info_kind = (struct rtattr *)((char *)linkinfo + RTA_ALIGN(linkinfo->rta_len));
    info_kind->rta_type = IFLA_INFO_KIND;
    info_kind->rta_len = RTA_LENGTH(strlen("veth") + 1);
    strcpy((char *)RTA_DATA(info_kind), "veth");
    linkinfo->rta_len = RTA_ALIGN(linkinfo->rta_len) + RTA_ALIGN(info_kind->rta_len);

    // Add peer information
    struct rtattr *info_data = (struct rtattr *)((char *)linkinfo + RTA_ALIGN(linkinfo->rta_len));
    info_data->rta_type = IFLA_INFO_DATA;
    info_data->rta_len = RTA_LENGTH(0);

    struct rtattr *peer_info = (struct rtattr *)((char *)info_data + RTA_ALIGN(info_data->rta_len));
    peer_info->rta_type = IFLA_INFO_PEER;
    peer_info->rta_len = RTA_LENGTH(sizeof(struct ifinfomsg));

    // Peer ifinfomsg
    struct ifinfomsg *peer_ifm = (struct ifinfomsg *)RTA_DATA(peer_info);
    peer_ifm->ifi_family = AF_UNSPEC;

    // Add peer name
    struct rtattr *peer_name = (struct rtattr *)((char *)peer_info + RTA_ALIGN(peer_info->rta_len));
    peer_name->rta_type = IFLA_IFNAME;
    peer_name->rta_len = RTA_LENGTH(strlen("veth-peer") + 1);
    strcpy((char *)RTA_DATA(peer_name), "veth-peer");

    // Update lengths
    peer_info->rta_len += RTA_ALIGN(peer_name->rta_len);
    info_data->rta_len += RTA_ALIGN(peer_info->rta_len);
    linkinfo->rta_len += RTA_ALIGN(info_data->rta_len);
    req.nlh.nlmsg_len += RTA_ALIGN(linkinfo->rta_len);

    struct sockaddr_nl sa = {0};
    sa.nl_family = AF_NETLINK;

    if (sendto(nl_socket, &req, req.nlh.nlmsg_len, 0, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
        perror("Failed to send Netlink message");
        close(nl_socket);
        exit(EXIT_FAILURE);
    }
}