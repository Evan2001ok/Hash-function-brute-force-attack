#include <openssl/sha.h>
#include <openssl/rand.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MESSAGE_LENGTH 16 // 15 characters + null terminator
#define HASH_TABLE_SIZE (1 << 24) // 2^24

typedef struct MessageNode {
    unsigned char message[MESSAGE_LENGTH];
    struct MessageNode* next;
} MessageNode;

MessageNode* hash_table[HASH_TABLE_SIZE];

uint32_t get_first_24_bits(unsigned char *hash) {
    return (uint32_t)(hash[0] << 16 | hash[1] << 8 | hash[2]);
}

unsigned long hash_function(uint32_t hash_value) {
    return hash_value % HASH_TABLE_SIZE;
}

void free_hash_table() {
    for (unsigned long i = 0; i < HASH_TABLE_SIZE; i++) {
        MessageNode* node = hash_table[i];
        while (node) {
            MessageNode* temp = node;
            node = node->next;
            free(temp);
        }
    }
}

int main() {
    memset(hash_table, 0, sizeof(hash_table));

    unsigned char trial_message[MESSAGE_LENGTH];
    unsigned char trial_hash[SHA256_DIGEST_LENGTH];
    uint32_t trial_hash_24;
    unsigned long attempts = 0;

    // Initialize OpenSSL random number generator
    if (RAND_status() != 1) {
        fprintf(stderr, "OpenSSL PRNG not properly initialized.\n");
        return 1;
    }

    while (1) {
        // Generate a random message
        if (RAND_bytes(trial_message, MESSAGE_LENGTH - 1) != 1) {
            fprintf(stderr, "RAND_bytes failed.\n");
            free_hash_table();
            return 1;
        }
        trial_message[MESSAGE_LENGTH - 1] = '\0'; // Ensure null-terminated

        // Compute the hash
        SHA256(trial_message, MESSAGE_LENGTH - 1, trial_hash);
        trial_hash_24 = get_first_24_bits(trial_hash);
        attempts++;

        unsigned long index = hash_function(trial_hash_24);
        MessageNode* node = hash_table[index];
        while (node) {
            if (memcmp(node->message, trial_message, MESSAGE_LENGTH) != 0 &&
                get_first_24_bits(SHA256(node->message, MESSAGE_LENGTH - 1, trial_hash)) == trial_hash_24) {
                // Collision found
                printf("Collision found after %lu attempts.\n", attempts);
                printf("Message 1: %s\n", node->message);
                printf("Message 2: %s\n", trial_message);
                printf("Hash (24 bits): %06x\n", trial_hash_24);
                free_hash_table();
                return 0;
            }
            node = node->next;
        }

        // No collision found, insert message into hash table
        MessageNode* new_node = (MessageNode*)malloc(sizeof(MessageNode));
        if (!new_node) {
            fprintf(stderr, "Memory allocation failed.\n");
            free_hash_table();
            return 1;
        }
        memcpy(new_node->message, trial_message, MESSAGE_LENGTH);
        new_node->next = hash_table[index];
        hash_table[index] = new_node;
    }

    // Should not reach here
    free_hash_table();
    return 0;
}
