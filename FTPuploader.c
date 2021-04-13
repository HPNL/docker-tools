#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <libgen.h>

#define BUFFLEN 1024

int ftp_send_command(int socket, char *cmd)
{
    int ret = send(socket, cmd, strlen(cmd), 0);
    if (ret < 0)
        fprintf(stderr, "Error in socket send.\n");
    return ret;
}

int ftp_recv_response(int socket)
{
    char buf[BUFFLEN];
    int ret = recv(socket, buf, BUFFLEN - 1, 0);
    if (ret == -1)
    {
        fprintf(stderr, "Error in socket recv.\n");
        return 0;
    }
    buf[ret] = '\0';
    sscanf(buf, "%d", &ret);

    return ret;
}

int ftp_enter_pasv(int socket)
{
    char buf[BUFFLEN];

    if (ftp_send_command(socket, "PASV\r\n") < 0)
        return 0;

    int ret = recv(socket, buf, BUFFLEN - 1, 0);
    if (ret == -1)
    {
        fprintf(stderr, "Error in socket recv.\n");
        return 0;
    }
    buf[ret] = '\0';
    int code, a, b, c, d, pa, pb;
    sscanf(buf, "%d Entering Passive Mode (%d,%d,%d,%d,%d,%d).\n", &code, &a, &b, &c, &d, &pa, &pb);
    if (code != 227)
    {
        fprintf(stderr, "Error in entering PASV mode.\n");
        return 0;
    }
    return pa * 256 + pb;
}

void cleanup(int socket, int errorCode, const char *msg)
{
    if (msg != NULL)
        fprintf(stderr, "%s\n", msg);
    if (socket)
        close(socket);
    if (errorCode)
        exit(errorCode);
}

int main(int argc, char *argv[])
{
    int sockserver, ftp;
    struct hostent *hent;
    struct sockaddr_in server_addr;
    int ret;
    char buf[BUFFLEN];

    if (argc != 5)
    {
        printf("usage : %s <host address> <username> <password> <file to upload>\n", argv[0]);
        return -1;
    }

    // Get the TCP server's IP address
    if ((hent = gethostbyname(argv[1])) == NULL)
    {
        fprintf(stderr, "Error in gethostbyname.\n");
        return 1;
    }
    // Create the TCP client socket
    if ((sockserver = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "Error in creating the socket.\n");
        return 1;
    }
    // Set the server socket address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(21); //Default FTP port
    server_addr.sin_addr = *((struct in_addr *)hent->h_addr);
    memset(&(server_addr.sin_zero), '\0', 8);
    // Connect to the above server socket
    if (connect(sockserver, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        fprintf(stderr, "Error in connect.\n");
        return 1;
    }

    ret = ftp_recv_response(sockserver);
    if (ret != 220)
        cleanup(sockserver, -2, "Error in FTP connect.");

    strcpy(buf, "USER ");
    strcat(buf, argv[2]);
    strcat(buf, "\r\n");
    ret = ftp_send_command(sockserver, buf);
    if (ret < 0)
        cleanup(sockserver, 1, NULL);
    ret = ftp_recv_response(sockserver);
    if (ret != 331)
        cleanup(sockserver, -2, "Error in FTP login.");

    strcpy(buf, "PASS ");
    strcat(buf, argv[3]);
    strcat(buf, "\r\n");
    ret = ftp_send_command(sockserver, buf);
    if (ret < 0)
        cleanup(sockserver, 1, NULL);
    ret = ftp_recv_response(sockserver);
    if (ret != 230)
        cleanup(sockserver, -2, "Login failed.");

    FILE *f = fopen(argv[4], "r");
    if (f == NULL)
        cleanup(sockserver, -2, "File does not exists.");

    int port = ftp_enter_pasv(sockserver);
    if (port == 0)
    {
        fclose(f);
        cleanup(sockserver, 1, "Error in PASV command.\n");
    }

    if ((ftp = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        fclose(f);
        cleanup(sockserver, 1, "Error in creating the data socket.");
    }

    // Set the server socket address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr *)hent->h_addr);
    memset(&(server_addr.sin_zero), '\0', 8);
    // Connect to the above server socket
    if (connect(ftp, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        fclose(f);
        cleanup(sockserver, 1, "Error in data connect.\n");
    }

    strcpy(buf, "STOR ");
    strcat(buf, basename(argv[4]));
    strcat(buf, "\r\n");
    ret = ftp_send_command(sockserver, buf);
    if (ret < 0)
    {
        fclose(f);
        close(ftp);
        cleanup(sockserver, 1, "Error in upload\n");
    }
    ret = ftp_recv_response(sockserver);
    if (ret != 150)
    {
        fclose(f);
        close(ftp);
        cleanup(sockserver, 1, "Error in upload\n");
    }

    while (!feof(f))
    {
        int size = fread(buf, sizeof(char), BUFFLEN, f);
        ret = send(ftp, buf, size, 0);
        if (ret < 0)
        {
            fclose(f);
            close(ftp);
            cleanup(sockserver, 1, "Error in data socket send.\n");
        }
    }
    fclose(f);
    close(ftp);

    ret = ftp_recv_response(sockserver);
    if (ret != 226)
        cleanup(sockserver, 1, "File upload failed\n");

    printf("File uploaded\n");

    close(sockserver);
    return 0;
}
