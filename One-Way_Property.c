#include <openssl/sha.h>
#include <openssl/rand.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MESSAGE_LENGTH 16 // 15 characters + null terminator

uint32_t get_first_24_bits(unsigned char *hash) {
    return (uint32_t)(hash[0] << 16 | hash[1] << 8 | hash[2]);
}

int main() {
    unsigned char initial_message[MESSAGE_LENGTH] = "InitialMessage";
    unsigned char initial_hash[SHA256_DIGEST_LENGTH];
    uint32_t initial_hash_24;

    // Compute the hash of the initial message
    SHA256(initial_message, strlen((char *)initial_message), initial_hash);
    initial_hash_24 = get_first_24_bits(initial_hash);

    printf("Initial Message: %s\n", initial_message);
    printf("Initial Hash (24 bits): %06x\n\n", initial_hash_24);

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
            return 1;
        }
        trial_message[MESSAGE_LENGTH - 1] = '\0'; // Ensure null-terminated

        // Compute the hash
        SHA256(trial_message, MESSAGE_LENGTH - 1, trial_hash);
        trial_hash_24 = get_first_24_bits(trial_hash);

        attempts++;

        // Check if hash matches the initial message's hash
        if (trial_hash_24 == initial_hash_24) {
            printf("One-way property broken after %lu attempts.\n", attempts);
            printf("Found Message: %s\n", trial_message);
            printf("Matching Hash (24 bits): %06x\n", trial_hash_24);
            break;
        }
    }

    return 0;
}
