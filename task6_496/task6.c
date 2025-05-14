#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "tea.h"

#define MAX_FILE_SIZE (5 * 1024 * 1024)
#define BLOCK_SIZE 8

void receive_file(const char *ip, int port, const char *output_file) {
    int sockfd;
    struct sockaddr_in server_addr;
    FILE *fp = fopen(output_file, "wb");

    if (!fp) {
        perror("Cannot create output file");
        exit(EXIT_FAILURE);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket error");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_aton(ip, &server_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connect error");
        exit(EXIT_FAILURE);
    }

    printf("Connected to %s:%d\n", ip, port);

    char buffer[4096];
    int bytes;
    while ((bytes = recv(sockfd, buffer, sizeof(buffer), 0)) > 0) {
        fwrite(buffer, 1, bytes, fp);
    }

    fclose(fp);
    close(sockfd);

    printf("Received encrypted file: %s\n", output_file);
}

int is_valid_ascii(unsigned char *data, int len) {
    for (int i = 0; i < len; i++) {
        if (!(data[i] >= 0x20 && data[i] <= 0x7E) &&
            data[i] != '\n' && data[i] != '\r' && data[i] != '\t') {
            return 0;
        }
    }
    return 1;
}

int remove_pkcs5_padding(unsigned char *data, int len) {
    if (len == 0) return 0;
    unsigned char padding = data[len - 1];
    if (padding < 1 || padding > BLOCK_SIZE) return len;

    for (int i = 0; i < padding; i++) {
        if (data[len - 1 - i] != padding) return len;
    }

    return len - padding;
}

int brute_force_decrypt(const char *encrypted_path, const char *decrypted_path) {
    FILE *fenc = fopen(encrypted_path, "rb");
    if (!fenc) {
        perror("Cannot open encrypted file");
        exit(EXIT_FAILURE);
    }

    fseek(fenc, 0, SEEK_END);
    int enc_size = ftell(fenc);
    rewind(fenc);

    if (enc_size > MAX_FILE_SIZE) {
        fprintf(stderr, "File too large\n");
        exit(EXIT_FAILURE);
    }

    if (enc_size % BLOCK_SIZE != 0) {
        printf("Warning: file size not aligned to 8-byte blocks (%d bytes)\n", enc_size);
    }

    unsigned char *enc_data = malloc(enc_size);
    unsigned char *dec_data = malloc(enc_size);
    fread(enc_data, 1, enc_size, fenc);
    fclose(fenc);

    unsigned int v[2], w[2], key[4];
    int dec_blocks = (enc_size / BLOCK_SIZE) * BLOCK_SIZE;

    for (int b = 0x00; b <= 0xFF; b++) {
        unsigned int pattern = b | (b << 8) | (b << 16) | (b << 24);
        for (int i = 0; i < 4; i++) key[i] = pattern;

        for (int i = 0; i + BLOCK_SIZE <= enc_size; i += BLOCK_SIZE) {
            memcpy(v, enc_data + i, BLOCK_SIZE);
            decipher(v, w, key);
            memcpy(dec_data + i, w, BLOCK_SIZE);
        }

        int plain_len = remove_pkcs5_padding(dec_data, dec_blocks);

        if (is_valid_ascii(dec_data, plain_len)) {
            printf("\nFound key: 0x%02X%02X%02X%02X repeated\n", b, b, b, b);
            printf("Saving decrypted file to: %s\n", decrypted_path);

            FILE *fout = fopen(decrypted_path, "w");
            fwrite(dec_data, 1, plain_len, fout);
            fclose(fout);

            free(enc_data);
            free(dec_data);
            return 0;
        }
    }

    fprintf(stderr, "No valid key found\n");
    free(enc_data);
    free(dec_data);
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 5 || strcmp(argv[1], "-server") != 0 || strcmp(argv[3], "-port") != 0) {
        fprintf(stderr, "Usage: %s -server <ip> -port <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *ip = argv[2];
    int port = atoi(argv[4]);

    const char *enc_file = "task6_encrypted.bin";
    const char *dec_file = "task6_decrypted.txt";

    receive_file(ip, port, enc_file);

    if (brute_force_decrypt(enc_file, dec_file) == 0) {
        printf("Decryption successful!\n");
    } else {
        printf("Decryption failed\n");
    }

    return 0;
}
