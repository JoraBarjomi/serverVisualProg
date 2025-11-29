#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>
#define PORT 12345

int main(int argc, char const* argv[])
{
    int status, valread, client_fd;
    struct sockaddr_in serv_addr;
    char hello [1024] = { 0 };
    char buffer[1024] = { 0 };
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "2.59.161.68", &serv_addr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }

    if ((status = connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
  
    while (true){
        printf("Enter message to server: ");
        hello [0]= '\0';
        fgets(hello, sizeof(hello), stdin);
        hello[strcspn(hello, "\n")] = '\0';
        if(strcmp(hello, "quit") == 0){
            close(client_fd);
            break;
        }
        send(client_fd, hello, strlen(hello), 0);;
        valread = read(client_fd, buffer, 1024 - 1); 
        printf("%s\n", buffer);
    }
    
    close(client_fd);
    return 0;
}