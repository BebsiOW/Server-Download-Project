#include <stdio.h>
#include <libsocket/libinetsocket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

int main()
{
    char myServer[30];

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

    int fd = create_inet_stream_socket(myServer, "2346", LIBSOCKET_IPv4, 0);
    if (fd < 0)
    {
        printf("Can't make connection\n");
        exit(1);
    }
    else printf("Conected to %s\n", myServer);

    
}