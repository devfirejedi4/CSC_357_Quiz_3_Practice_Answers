#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345

int main()
{
    int client_fd;
    struct sockaddr_in server_address;

    // Create a socket
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set up the server address structure
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to the server
    if (connect(client_fd, (struct sockaddr*)&server_address, 
        sizeof(server_address)) == -1)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    // Send client's PID to the server
    pid_t client_pid = getpid();
    printf("My PID is %d\n", client_pid);
    if (send(client_fd, &client_pid, sizeof(pid_t), 0) != 
        sizeof(pid_t))
    {
        perror("send");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    // Receive server's PID from the socket
    pid_t server_pid;
    if (recv(client_fd, &server_pid, sizeof(pid_t), 0) != 
        sizeof(pid_t))
    {
        perror("recv");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Client received server PID: %d\n", server_pid);

    // Close the socket
    close(client_fd);

    return 0;
}

