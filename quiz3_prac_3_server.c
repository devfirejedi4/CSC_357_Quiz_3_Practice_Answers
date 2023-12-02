#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 12345

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Create a socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set up the server address structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the address and port
    if (bind(server_fd, (struct sockaddr*)&address, 
        sizeof(address)) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 1) == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Accept a connection from a client
    if ((new_socket = accept(server_fd, 
                      (struct sockaddr*)&address, 
                      (socklen_t*)&addrlen)) == -1)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // Read client's PID from the socket
    pid_t client_pid;
    if (recv(new_socket, &client_pid, sizeof(pid_t), 0) != 
        sizeof(pid_t))
    {
        perror("recv");
        close(new_socket);
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server received client PID: %d\n", client_pid);

    // Send server's PID to the client
    pid_t server_pid = getpid();
    printf("My PID is %d\n", server_pid);
    if (send(new_socket, &server_pid, sizeof(pid_t), 0) != 
        sizeof(pid_t))
    {
        perror("send");
        close(new_socket);
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Close the socket
    close(new_socket);
    close(server_fd);

    return 0;
}

