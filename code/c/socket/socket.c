#include "socket.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

int getClient() {
  // Sockets and stuff
  struct sockaddr_un serverAddr;
  struct sockaddr_storage clientAddr;
  int serverSocket = socket(AF_UNIX, SOCK_STREAM, 0);

  serverAddr.sun_family = AF_UNIX;
  strcpy(serverAddr.sun_path, "/tmp/packetlens.sock");
  int serverLen = sizeof(serverAddr);

  if (bind(serverSocket, (struct sockaddr *)&serverAddr, serverLen) == -1) {
    perror("Error binding to socket: ");
    exit(EXIT_FAILURE);
  }

  if (listen(serverSocket, 5) == -1) {
    perror("Listening Error");
    exit(EXIT_FAILURE);
  }

  socklen_t clientLen = sizeof(clientAddr);
  int clientSocket = accept(serverSocket, (struct sockaddr *)&clientSocket, &clientLen);
  if (clientSocket == -1) {
    fprintf(stderr, "Error Accepting Client");
    exit(EXIT_FAILURE);
  }

  return clientSocket;
}
