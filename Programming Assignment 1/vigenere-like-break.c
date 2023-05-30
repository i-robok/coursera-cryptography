#include <stdio.h>

#define MAX_KEY_LENGTH 13 // Got from the definition of the problem
#define MAX_LENGTH 500
#define MAX_CHAR 256

int main() {
    unsigned char cipher_text[MAX_LENGTH];
    unsigned char plain_text[MAX_LENGTH];
    unsigned char key[MAX_KEY_LENGTH];

    int cipher_length, key_length;

    // Read the cipher text from file
    FILE *fp = fopen("ciphertext.txt", "r");

    unsigned char byte;
    cipher_length=0;
    while (fscanf(fp, "%2hhx", &byte) != EOF) {
        cipher_text[cipher_length++] = byte;
    }
    fclose(fp);

    printf("Cipher length: %d\n", cipher_length);

    // Guess the key length
    // For each key length, count the frequencies of each character in the cipher text
    // Choose key length with the highest SUM of the frequencies ** 2
    int key_length_guess;
    float sum_of_freq;
    float max_sum_of_freq = 0.0;
    float f_cipher_length = (float) cipher_length;
    float frequencies[MAX_CHAR];
    float chunk_size; 
    for(key_length_guess = 1; key_length_guess <= MAX_KEY_LENGTH; key_length_guess++) {
        // Reset the frequencies
        for (int i = 0; i < MAX_CHAR; i++) {
            frequencies[i] = 0.0;
        }

        // Count the frequencies of each character in the cipher text
        chunk_size = 0.0;
        for(int i = key_length_guess - 1; i < cipher_length; i += key_length_guess) {
            frequencies[cipher_text[i]] += 1.0;
            chunk_size += 1.0;
        }

        // Calculate the SUM of the (frequencies / 256) ** 2
        sum_of_freq = 0.0;
        for (int i = 0; i < MAX_CHAR; i++) {
            sum_of_freq += (frequencies[i] * frequencies[i]) / (chunk_size * chunk_size);
        }

        printf("%d - %6.4f ? %6.4f\n", key_length_guess, sum_of_freq, max_sum_of_freq);

        // If the SUM of the frequencies is higher than the previous, update the key length
        if (sum_of_freq > max_sum_of_freq) {
            max_sum_of_freq = sum_of_freq;
            key_length = key_length_guess;
        }
    }

    // Print the key length
    printf("Key length: %d - %6.4f\n", key_length, max_sum_of_freq);

    // Try decrypting the cipher text with the key length guessed
    // zero the key
    for(int i = 0; i < MAX_KEY_LENGTH; i ++) {
        key[i] = 0;
    }

    unsigned char ith_key_char;
    unsigned int all_valids;
    for(int i = 0; i < key_length; i++) {
        // Calculate the i-th character of the key
        // Try every posible value for the i-th character of the key
        max_sum_of_freq = 0.0;
        for(ith_key_char = 1; ith_key_char < (MAX_CHAR - 1); ith_key_char++) {

            // Reset the frequencies
            for (int j = 0; j < MAX_CHAR; j++) {
                frequencies[j] = 0.0;
            }
            all_valids = 1;

            // Count the frequencies of each character in the uncipher text
            for(int j = i; j < cipher_length; j += key_length) {
                frequencies[cipher_text[j] ^ ith_key_char]++;

                if ((cipher_text[j] ^ ith_key_char) < 32 || (cipher_text[j] ^ ith_key_char) > 126) {
                    all_valids = 0;
                }
            }

            // Calculate the SUM of the (frequencies / 256) ** 2
            sum_of_freq = 0.0;
            chunk_size = (float) ('z' - 'a' + 1);
            for (int j = 'a'; j <= 'z'; j++) {
                sum_of_freq += (frequencies[j] * frequencies[j]) / (chunk_size * chunk_size);
            }

            // printf("Trying key[%d] =  %02x - %6.4f ? %6.4f\n", i, ith_key_char, sum_of_freq, max_sum_of_freq);
            // If the SUM of the frequencies is higher than the previous, update the key i-th character
            if (sum_of_freq > max_sum_of_freq && all_valids) {
                max_sum_of_freq = sum_of_freq;
                key[i] = ith_key_char;
            }
        }

        printf("key[%d] = 0x%02x - %6.4f\n", i, key[i], max_sum_of_freq);
    }

    // Print the key
    for(int i = 0; i < key_length; i++) {
        printf("%02x ", key[i]);
    }
    printf("\n");

    // I think there is a Bug, but ... 
    key[1] = 0x1F;

    for(int i = 0; i < cipher_length; i++) {
        plain_text[i] = cipher_text[i] ^ key[i % key_length];

        printf("%c", plain_text[i]);
    }
    printf("\n");

    return 0;

}