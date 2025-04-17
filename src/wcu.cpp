#include <openssl/evp.h>
#include <openssl/rand.h>
#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <cstring>

// Function to handle OpenSSL errors
void handleErrors() {
    ERR_print_errors_fp(stderr);
    abort();
}

// Function to generate a random key and IV
void generateKeyIV(unsigned char* key, unsigned char* iv) {
    if (!RAND_bytes(key, 32) || !RAND_bytes(iv, 16)) {
        handleErrors();
    }
}

// Function to encrypt plaintext using AES-256-CBC
std::vector<unsigned char> aesEncrypt(const std::vector<unsigned char>& plaintext,
                                      const unsigned char* key,
                                      const unsigned char* iv) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) handleErrors();

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    std::vector<unsigned char> ciphertext(plaintext.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
    int len;
    if (1 != EVP_EncryptUpdate(ctx, ciphertext.data(), &len, plaintext.data(), plaintext.size()))
        handleErrors();
    int ciphertext_len = len;

    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len))
        handleErrors();
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    ciphertext.resize(ciphertext_len);
    return ciphertext;
}

// Function to generate Laplace noise
double generateLaplaceNoise(double scale) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(-0.5, 0.5);
    double u = dis(gen);
    return scale * std::sgn(u) * std::log(1 - 2 * std::abs(u));
}

// Function to add Laplace noise to encrypted data
std::vector<unsigned char> addLaplaceNoise(const std::vector<unsigned char>& data, double scale) {
    std::vector<unsigned char> noisedData = data;
    for (auto& byte : noisedData) {
        double noise = generateLaplaceNoise(scale);
        int noisedByte = static_cast<int>(byte) + static_cast<int>(std::round(noise));
        byte = static_cast<unsigned char>(std::clamp(noisedByte, 0, 255));
    }
    return noisedData;
}

int main() {
    // Sample plaintext
    std::string plaintext = "This is a test message.";

    // Generate key and IV
    unsigned char key[32];
    unsigned char iv[16];
    generateKeyIV(key, iv);

    // Encrypt the plaintext
    std::vector<unsigned char> encryptedData = aesEncrypt(
        std::vector<unsigned char>(plaintext.begin(), plaintext.end()), key, iv);

    // Add Laplace noise
    double noiseScale = 1.0; // Adjust the scale as needed
    std::vector<unsigned char> noisedData = addLaplaceNoise(encryptedData, noiseScale);

    // Output the results
    std::cout << "Original Plaintext: " << plaintext << std::endl;
    std::cout << "Encrypted Data: ";
    for (auto byte : encryptedData) {
        std::cout << std::hex << static_cast<int>(byte);
    }
    std::cout << std::endl;

    std::cout << "Noised Encrypted Data: ";
    for (auto byte : noisedData) {
        std::cout << std::hex << static_cast<int>(byte);
    }
    std::cout << std::endl;

    return 0;
}
