#include <stdio.h>
#include <libsocket/libinetsocket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

int main()
{
    char response[100];
    char myServer[30];
    char input = 0;

    while(1)
    {
        printf("Which server (richmond/london): ");
        scanf("%s", myServer);

        if(strcmp(myServer, "richmond") == 0 || strcmp(myServer, "london") == 0)
        {
            break;
        }
        else
        printf("Please enter a valid server.\n");
    }

    strcat(myServer, ".cs.sierracollege.edu");

    // Open connection to richmond/london servers, port 3456

    int fd = create_inet_stream_socket(myServer, "3456", LIBSOCKET_IPv4, 0);
    if (fd < 0)
    {
        printf("Can't make connection\n");
        exit(1);
    }
    else printf("Connected to %s\n", myServer);

    FILE *s = fdopen(fd, "r+");
    if(!s)
    {
        printf("Error opening file\n");
        close(fd);
        exit(1);
    }

    fgets(response, sizeof(response), s);

    while(1)
    {
        printf("(L)ist Files\n(D)ownload\n(Q)uit\n\nWhat would you like to do: ");
        scanf(" %c", &input);
        
        // Option 1: List
        if(input == 'L' || input == 'l')
        {
            fprintf(s, "LIST\n");

            // Eats +OK
            if (fgets(response, sizeof(response), s) == NULL)
            break;

            while (1)
            {
                if (fgets(response, sizeof(response), s) == NULL)
                    break;

                if (strcmp(response, ".\n") == 0)
                    break;

                printf("%s", response);
            }
            printf("\n\n");
        }

        // Option 2: Download
        else if(input == 'D' || input == 'd')
        {
            int size;
            int refreshBar = 50000;
            int buffer_size = 1000;

            char downFile[30];
            char buffer[buffer_size];
            
            int transferred = 0;
            

            printf ("Which file would you like to download: ");
            scanf("%s", downFile);

            fprintf(s, "SIZE %s\n", downFile);
            fgets(response, sizeof(response), s);
            sscanf(response, "+OK %d", &size); // reads size of file

            fprintf(s, "GET %s\n", downFile);
            fgets(response, sizeof(response), s);

            // Check if file already exists
            FILE *exists = fopen(downFile, "r");
            if (exists != NULL)
            {
                fclose(exists);  

                char choice;
                printf("File '%s' already exists. Overwrite? (y/n): ", downFile);
                scanf(" %c", &choice);

                if (choice != 'y' && choice != 'Y')
                {
                    printf("Download canceled.\n\n");
                    continue; 
                }
            }

            FILE *output = fopen(downFile, "wb");

            // Length-Prefix Framing
            while (transferred < size)
            {
                int remaining = size - transferred;
                int bytes_wanted;

                    if (remaining < buffer_size)
                        bytes_wanted = remaining;
                    else
                        bytes_wanted = buffer_size;

                int bytes_received = fread(buffer, 1, bytes_wanted, s);
                fwrite(buffer, 1, bytes_received, output);
                
                transferred += bytes_received;

                // Progress Bar
                if (transferred >= refreshBar)
                { 
                    system("clear");
                    printf("There are %d bytes left!\n", remaining);
                    refreshBar += 750000;
                }
            }

            printf("Downloaded!\n\n");

            fclose(output);
        }

        // Option 3: Quit
        else  if(input == 'Q' || input == 'q')
        {
            fprintf(s, "QUIT\n");
            fgets(response, sizeof(response), s);
            break;
        }
        else

        // Option 4: Panic
        {
            printf("Please enter a valid option (Ex: 'd' or 'L')\n");
        }
    }
    
    fclose(s);
    exit(1);
}