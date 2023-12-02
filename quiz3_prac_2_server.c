#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define FIFO_NAME "myfifo"

int main() 
{
    pid_t client_pid, server_pid;

    // Create the named pipe (FIFO)
    if (mkfifo(FIFO_NAME, 0666) == -1)
    {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    // Open the named pipe for reading
    int fifo_fd = open(FIFO_NAME, O_RDONLY);
    if (fifo_fd == -1) 
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Read client's PID from the named pipe
    if (read(fifo_fd, &client_pid, sizeof(pid_t)) != 
        sizeof(pid_t)) 
    {
        perror("read");
        close(fifo_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server received client PID: %d\n", client_pid);
    
    // Close the named pipes
    close(fifo_fd);

    
    // Get and send server's PID to the client
    server_pid = getpid();
    printf("My PID is %d\n", server_pid);
    fifo_fd = open(FIFO_NAME, O_WRONLY);
    if (fifo_fd == -1) 
    {
        perror("open");
        close(fifo_fd);
        exit(EXIT_FAILURE);
    }

    if (write(fifo_fd, &server_pid, sizeof(pid_t)) != 
        sizeof(pid_t)) 
    {
        perror("write");
        close(fifo_fd);
        exit(EXIT_FAILURE);
    }

    // Close the named pipes
    close(fifo_fd);

    // Remove the named pipe
    if (unlink(FIFO_NAME) == -1) 
    {
        perror("unlink");
        exit(EXIT_FAILURE);
    }

    return 0;
}

