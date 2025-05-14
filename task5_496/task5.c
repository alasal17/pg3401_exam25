#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "include/ewpdef.h"

#define MAX_BUFFER 1024

int is_valid_filename(const char *filename) {
    if (!filename || strlen(filename) == 0) return 0;
    if (strstr(filename, "..") || strchr(filename, '/') || strchr(filename, '\\')) return 0;
    for (int i = 0; filename[i]; i++) {
        if (!isalnum(filename[i]) && filename[i] != '.' && filename[i] != '_' && filename[i] != '-') return 0;
    }
    return 1;
}

void send_server_accept(int client_fd, const char *server_id) {
    struct EWA_EXAM25_TASK5_PROTOCOL_SERVERACCEPT response;
    memset(&response, 0, sizeof(response));
    memcpy(response.stHead.acMagicNumber, "EWP", 3);
    memcpy(response.stHead.acDataSize, "0064", 4);
    memcpy(response.stHead.acDelimeter, "|", 1);
    memcpy(response.acStatusCode, "220", 3);
    response.acHardSpace[0] = ' ';
    snprintf(response.acFormattedString, sizeof(response.acFormattedString), "127.0.0.1 SMTP %s", server_id);
    response.acHardZero[0] = '\0';

    send(client_fd, &response, sizeof(response), 0);
}

void send_server_reply(int client_fd, const char* status_code, const char* message) {
    struct EWA_EXAM25_TASK5_PROTOCOL_SERVERREPLY reply;
    memset(&reply, 0, sizeof(reply));

    memcpy(reply.stHead.acMagicNumber, "EWP", 3);
    memcpy(reply.stHead.acDataSize, "0064", 4);
    memcpy(reply.stHead.acDelimeter, "|", 1);
    memcpy(reply.acStatusCode, status_code, 3);
    reply.acHardSpace[0] = ' ';
    strncpy(reply.acFormattedString, message, sizeof(reply.acFormattedString) - 1);
    reply.acHardZero[0] = '\0';

    send(client_fd, &reply, sizeof(reply), 0);
}

int main(int argc, char *argv[]) {
    if (argc != 5 || strcmp(argv[1], "-port") != 0 || strcmp(argv[3], "-id") != 0) {
        fprintf(stderr, "Usage: %s -port <port> -id <identifier>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int port = atoi(argv[2]);
    const char *server_id = argv[4];

    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket error");
        return EXIT_FAILURE;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind error");
        return EXIT_FAILURE;
    }

    listen(server_fd, 1);
    printf("Server listening on port %d...\n", port);

    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
        perror("Accept error");
        return EXIT_FAILURE;
    }

    send_server_accept(client_fd, server_id);

    char filename[256] = "";
    FILE *outfile = NULL;
    int receiving_data = 0;

    while (1) {
        char buffer[10000] = {0};
        int bytes = recv(client_fd, buffer, sizeof(buffer), 0);
        if (bytes <= 0) break;

        struct EWA_EXAM25_TASK5_PROTOCOL_SIZEHEADER *hdr = (void*)buffer;

        if (memcmp(hdr->acMagicNumber, "EWP", 3) != 0) {
            send_server_reply(client_fd, "500", "Invalid protocol");
            continue;
        }

        if (receiving_data) {
            struct EWA_EXAM25_TASK5_PROTOCOL_CLIENTDATAFILE *datafile = (void*)buffer;
            size_t content_len = atoi(hdr->acDataSize) - 1;
            if (outfile) fwrite(datafile->acFileContent, 1, content_len, outfile);

            if (outfile) fclose(outfile);
            outfile = NULL;
            receiving_data = 0;
            send_server_reply(client_fd, "250", "File received OK");
            continue;
        }

        if (memcmp(buffer + sizeof(*hdr), "HELO", 4) == 0) {
            send_server_reply(client_fd, "250", "127.0.0.1 Hello client");
        } else if (memcmp(buffer + sizeof(*hdr), "MAIL", 4) == 0) {
            send_server_reply(client_fd, "250", "Sender OK");
        } else if (memcmp(buffer + sizeof(*hdr), "RCPT", 4) == 0) {
            send_server_reply(client_fd, "250", "Recipient OK");
        } else if (memcmp(buffer + sizeof(*hdr), "DATA", 4) == 0) {
            struct EWA_EXAM25_TASK5_PROTOCOL_CLIENTDATACMD *datacmd = (void*)buffer;

            snprintf(filename, sizeof(filename), "%.*s",
                     (int)sizeof(datacmd->acFormattedString), datacmd->acFormattedString);

            if (!is_valid_filename(filename)) {
                send_server_reply(client_fd, "501", "Invalid filename");
                continue;
            }

            outfile = fopen(filename, "w");
            if (!outfile) {
                send_server_reply(client_fd, "451", "Failed to open file");
                continue;
            }

            receiving_data = 1;
            send_server_reply(client_fd, "354", "Start mail input; end with <CRLF>.<CRLF>");
        } else if (memcmp(buffer + sizeof(*hdr), "QUIT", 4) == 0) {
            send_server_reply(client_fd, "221", "Connection closing");
            break;
        } else {
            send_server_reply(client_fd, "500", "Unknown command");
        }
    }

    if (outfile) fclose(outfile);
    close(client_fd);
    close(server_fd);
    return 0;
}
