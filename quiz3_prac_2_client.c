#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define FIFO_NAME "myfifo"

int main() 
{
    pid_t client_pid, server_pid;

    // Get and send client's PID to the server
    client_pid = getpid();
    printf("My PID is %d\n", client_pid);
    int fifo_fd = open(FIFO_NAME, O_WRONLY);
    if (fifo_fd == -1) 
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (write(fifo_fd, &client_pid, sizeof(pid_t)) != 
        sizeof(pid_t)) 
    {
        perror("write");
        close(fifo_fd);
        exit(EXIT_FAILURE);
    }
    
    // Close the named pipes
    close(fifo_fd);
    

    // Open the named pipe for reading
    fifo_fd = open(FIFO_NAME, O_RDONLY);
    if (fifo_fd == -1) 
    {
        perror("open");
        close(fifo_fd);
        exit(EXIT_FAILURE);
    }

    // Read server's PID from the named pipe
    if (read(fifo_fd, &server_pid, sizeof(pid_t)) != 
        sizeof(pid_t)) 
    {
        perror("read");
        close(fifo_fd);
        exit(EXIT_FAILURE);
    }

    printf("Client received server PID: %d\n", server_pid);

    // Close the named pipes
    close(fifo_fd);

    return 0;
}

