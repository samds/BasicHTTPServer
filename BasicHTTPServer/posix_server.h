//
//  posix_server.h
//  BasicHTTPServer
//
//  Created by samuel.
//  Copyright (c) 2014 SDS. All rights reserved.
//

#ifndef BasicHTTPServer_posix_server_h
#define BasicHTTPServer_posix_server_h

#include <arpa/inet.h>      // inet_ntop()
#include <errno.h>          // errno
#include <netdb.h>          // addrinfo
#include <netinet/in.h>     // sockaddr_in
#include <stdbool.h>        // bool, true, false
#include <stdio.h>          // perror(), printf()
#include <stdlib.h>         // exit, EXIT_FAILURE
#include <string.h>         // memset(), strerror()
#include <sys/socket.h>     // AF_INET, SOCK_STREAM,
#include <unistd.h>         // close()

#define INVALID_SOCKET      (-1)
#define SOCKET_ERROR        (-1)
#define SEND_ERROR          (-1)
#define HOST                "localhost"
#define PORT                "8080"
#define GETADDRINFO_SUCCEED (0)
#define MSG_NOFLAG          (0x00)
#define BACKLOG             (10) // maximum length for the queue of pending connections

// socket type
typedef int         socket_t;

#define SIZE_BUF    1024 // maximum size of a message

bool POSIX_init_http_server()
{
    int status;
    struct addrinfo hints, *res, *it;
    socket_t sockfd, csockfd; // socket file descriptor, client socket file descriptor
    struct sockaddr_storage their_addr;
    char ipstr[INET6_ADDRSTRLEN]; // ip string
    
    // first, load up address structs with getaddrinfo():
    
    // Initialises the struct with 0
    memset(&hints, 0, sizeof hints);
    
    // AF_UNSPEC make it IP version-agnostic.
    // Can use IPv4(AF_INET) or IPv6(AF_INET6), whichever.
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
    
    const char *hostname = HOST;
    const char *servname = PORT;
    status = getaddrinfo(hostname, servname, &hints, &res);
    if(status != GETADDRINFO_SUCCEED) {
        fprintf(stderr, "getaddrinfo failed, reason: %s\n",gai_strerror(status));
        exit(EXIT_FAILURE);
    }
    
    printf("--\n");
    printf("Addr info associated to host [%s] and service name [%s]\n",hostname,servname);
    
    // browses all addrinfo associated to the given host and service name
    for (it = res; it != NULL; it = it->ai_next) {
        void *addr;
        const char *ip_version;
        
        if (it->ai_family == AF_INET) { //IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)it->ai_addr;
            addr = &(ipv4->sin_addr);
            ip_version = "IPv4";
        }
        else if (it->ai_family == AF_INET6) { //IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)it->ai_addr;
            addr = &(ipv6->sin6_addr);
            ip_version = "IPv6";
        }
        else {
            continue;
        }
        
        // "ntop" stands for network to presentation (printable)
        inet_ntop(it->ai_family, addr, ipstr, sizeof(ipstr));
        // Print address
        printf("Address %s = %s\n", ip_version, ipstr);
    }
    
    printf("--\n");
    
    // browses all addrinfo associated to the given host and service name
    // and bind to the first one working.
    for (it = res; it != NULL; it = it->ai_next) {
        
        // Create socket
        
        sockfd = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        
        if (sockfd == SOCKET_ERROR) {
            fprintf(stderr, "socket failed, reason: %s\n",strerror(errno));
            continue;
        }
        
        fprintf(stdout, "socket creation succeed\n");
        printf("--\n");

        // Bind socket
        
        // Bind it to the port we passed in to getaddrinfo():
        bind(sockfd, it->ai_addr, it->ai_addrlen);
        
        // Prevent "address already in use" problem
        int yes = 1;
        if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
        
        while (true) {
            // Listen
            
            if(listen(sockfd, BACKLOG) == SOCKET_ERROR)
            {
                perror("listen()");
                break;
            }
            
            // now accept an incoming connection:
            
            socklen_t addr_size = sizeof(their_addr);
            csockfd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
            
            printf("New connection\n");

            char buffer[SIZE_BUF];
            int flags = 0; // no flags
            size_t length = sizeof(buffer);
            ssize_t bytes_recv = recv(csockfd, (void *)buffer, length, flags);
            if (bytes_recv > 0) {
                //fprintf(stdout, "\t%s \n[%ld chars]\n\n", buffer, bytes_recv);
            }
            else if (bytes_recv == 0) {
                printf("the remote side has closed the connection on you!");
            }
            else {
                perror("recv()");
            }
            
            const char header[] =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=UTF-8"
            "\r\n\r\n"
            "<!DOCTYPE html><html><body><h1>Hello, World!</h1></body></html>"
            "\r\n";
            
            length = strlen(header); // We want to transmit the null character
            flags = MSG_NOFLAG;
            ssize_t bytes_sent = send(csockfd, header, length, flags);
            
            if (bytes_sent == SEND_ERROR) {
                perror("send()");
            }
            else {
                printf("Sent %ld characters\n",bytes_sent);
            }
            printf("--\n");
            
            // closes client socket
            close(csockfd);

        }
        
        // closes server socket
        close(sockfd);
        break;
    }
    
    freeaddrinfo(res);
    return true;
}

#endif
