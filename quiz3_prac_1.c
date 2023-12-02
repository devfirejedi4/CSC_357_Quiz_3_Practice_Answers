#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main() 
{
    pid_t pid, grandchild_pid;

    // Create a file for communication.
    const char *filename = "communication_file.txt";
    
    // Open it in
    // read/write mode,
    // create if doesn't exist, 
    // truncate it if it exists, and
    // set S_IRUSR | S_IWUSR permissions: user read, user write.
    int file_fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 
                       0600);
    
    if (file_fd == -1) 
    {
    	printf("hello");
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Fork the first child process.
    if ((pid = fork()) < 0) 
    {
        perror("fork");
        exit(EXIT_FAILURE);
    } 
    else if (pid == 0) 
    { 
        // Child process
        // Fork the grandchild process.
        if ((pid = fork()) < 0) 
        {
            perror("fork");
            exit(EXIT_FAILURE);
        } 
        else if (pid == 0) 
        { 
            // Grandchild process
            // Get the PID of the grandchild.
            pid_t my_pid = getpid();
            printf("I am grandchild and my PID is %d\n", 
            	    my_pid);

            // Write the grandchild's PID to the file.
            if (write(file_fd, &my_pid, sizeof(pid_t)) != 
                sizeof(pid_t)) 
            {
                perror("write");
                exit(EXIT_FAILURE);
            }

            close(file_fd);
            exit(EXIT_SUCCESS);
        } 
        else 
        {   
            // Child process
            // Wait for the grandchild to finish.
            wait(NULL);
            exit(EXIT_SUCCESS);
        }
    } 
    else 
    {   
    	// Parent process
        // Wait for the child to finish.
        wait(NULL);

	// Close the file and reopen it 
	// in order to ensure offset is at 0.
	// Alternative: lseek(file_fd, 0, SEEK_SET);
	close(file_fd);
	file_fd = open(filename, O_RDONLY);
	
        // Read the grandchild's PID from the file.
        if (read(file_fd, &grandchild_pid, sizeof(pid_t)) != 
            sizeof(pid_t)) 
        {
            perror("read");
            close(file_fd);
            exit(EXIT_FAILURE);
        }

        // Print the grandchild's PID.
        printf("I am parent and my grandchild's PID is %d\n", 
               grandchild_pid);

        // Close and remove the file.
        // Unlink deletes a name from the filesystem.
        // If that name was the last link to a file and 
        // no processes have the file open, 
        // the file is deleted and 
        // the space it was using is made available for reuse.
        close(file_fd);
        if (unlink(filename) == -1) 
        {
            perror("unlink");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}

