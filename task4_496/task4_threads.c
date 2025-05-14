#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>

#define BUFFER_SIZE 4096
#define MAX_FILE_SIZE (5 * 1024 * 1024)  // 5 MB
#define BLOCK_SIZE 8

int Task2_SimpleDjb2Hash(FILE* fFileDescriptor, int* piHash);
void encipher(unsigned int *const v, unsigned int *const w, const unsigned int *const k);

// Structure shared between threads
typedef struct {
    unsigned char buffer[BUFFER_SIZE];  
    int bytes_read;                     
    int done;                        
    sem_t sem_full, sem_empty;       
    pthread_mutex_t mutex;        
    char filename[256];             
} SharedData;

// Thread A: Reads from file into buffer
void* thread_A(void* arg) {
    SharedData* data = (SharedData*)arg;
    FILE* fp = fopen(data->filename, "rb");
    if (!fp) {
        perror("Failed to open input file");
        exit(EXIT_FAILURE);
    }

    while (1) {
        sem_wait(&data->sem_empty);
        pthread_mutex_lock(&data->mutex);

        data->bytes_read = fread(data->buffer, 1, BUFFER_SIZE, fp);
        data->done = (data->bytes_read == 0);

        pthread_mutex_unlock(&data->mutex);
        sem_post(&data->sem_full);

        if (data->done) break;
    }

    fclose(fp);
    pthread_exit(NULL);
}

// Function to apply PKCS5 padding
int pkcs5_pad(unsigned char* input, int input_len, unsigned char** output) {
    int pad_len = BLOCK_SIZE - (input_len % BLOCK_SIZE);
    int total_len = input_len + pad_len;

    *output = (unsigned char*)malloc(total_len);
    if (!*output) {
        perror("Failed to allocate memory for padded data");
        exit(EXIT_FAILURE);
    }

    memcpy(*output, input, input_len);
    memset(*output + input_len, pad_len, pad_len);

    return total_len;
}

// Thread B: Reads from buffer and performs hashing and encryption
void* thread_B(void* arg) {
    SharedData* data = (SharedData*)arg;
    unsigned char* total_data = malloc(MAX_FILE_SIZE);
    if (!total_data) {
        perror("Failed to allocate memory for full file data");
        exit(EXIT_FAILURE);
    }

    int total_len = 0;

    while (1) {
        sem_wait(&data->sem_full);
        pthread_mutex_lock(&data->mutex);

        if (data->bytes_read > 0) {
            if (total_len + data->bytes_read > MAX_FILE_SIZE) {
                fprintf(stderr, "File too large\n");
                exit(EXIT_FAILURE);
            }
            memcpy(total_data + total_len, data->buffer, data->bytes_read);
            total_len += data->bytes_read;
        }

        int is_done = data->done;
        pthread_mutex_unlock(&data->mutex);
        sem_post(&data->sem_empty);

        if (is_done) break;
    }

    // Part 1: Hashing
    FILE* f_in = fopen(data->filename, "rb");
    if (!f_in) {
        perror("Failed to reopen file for hashing");
        exit(EXIT_FAILURE);
    }

    int hash_value = 0;
    Task2_SimpleDjb2Hash(f_in, &hash_value);
    fclose(f_in);

    FILE* f_hash = fopen("task4_pg2265.hash", "w");
    if (!f_hash) {
        perror("Failed to open hash file");
        exit(EXIT_FAILURE);
    }
    fprintf(f_hash, "%08x\n", hash_value);
    fclose(f_hash);

    // Part 2: Padding and encryption
    unsigned char* padded_data = NULL;
    int padded_len = pkcs5_pad(total_data, total_len, &padded_data);

    unsigned int key[4] = {0x12345678, 0x87654321, 0xdeadbeef, 0xbeefdead};
    FILE* f_enc = fopen("task4_pg2265.enc", "wb");
    if (!f_enc) {
        perror("Failed to open encrypted output file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < padded_len; i += BLOCK_SIZE) {
        unsigned int v[2], w[2];
        memcpy(v, padded_data + i, BLOCK_SIZE);
        encipher(v, w, key);
        fwrite(w, sizeof(unsigned int), 2, f_enc);
    }

    fclose(f_enc);
    free(total_data);
    free(padded_data);

    pthread_exit(NULL);
}

// Main function
int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    SharedData data;
    memset(&data, 0, sizeof(data));
    strncpy(data.filename, argv[1], sizeof(data.filename) - 1);
    data.filename[sizeof(data.filename) - 1] = '\0';

    pthread_mutex_init(&data.mutex, NULL);
    sem_init(&data.sem_empty, 0, 1);
    sem_init(&data.sem_full, 0, 0);

    pthread_t tA, tB;
    pthread_create(&tA, NULL, thread_A, &data);
    pthread_create(&tB, NULL, thread_B, &data);

    pthread_join(tA, NULL);
    pthread_join(tB, NULL);

    pthread_mutex_destroy(&data.mutex);
    sem_destroy(&data.sem_empty);
    sem_destroy(&data.sem_full);

    return EXIT_SUCCESS;
}
