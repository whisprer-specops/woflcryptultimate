  #include <openssl/evp.h>
  #include <openssl/aes.h>
  #include <vector>
  #include <random>
  #include <cmath>
  #include <iostream>
  #include <cstring>

  // Function to generate Laplace noise
double generateLaplaceNoise(double scale, std::mt19937& gen) {
  std::uniform_real_distribution<double> dis(-0.5, 0.5);
  double u = dis(gen);
  return scale * ((u > 0) - (u < 0)) * std::log(1 - 2 * std::abs(u));
}

// Function to remove Laplace noise from data
std::vector<unsigned char> removeLaplaceNoise(const std::vector<unsigned char>& noisedData, double scale, unsigned int seed) {
  std::vector<unsigned char> cleanedData = noisedData;
  std::mt19937 gen(seed); // Initialize random number generator with the same seed

  for (size_t i = 0; i < cleanedData.size(); ++i) {
      double noise = generateLaplaceNoise(scale, gen);
      int cleanedByte = static_cast<int>(cleanedData[i]) - static_cast<int>(std::round(noise));
      cleanedData[i] = static_cast<unsigned char>(std::clamp(cleanedByte, 0, 255));
  }
  return cleanedData;
}

// Function to handle OpenSSL errors
void handleErrors() {
  ERR_print_errors_fp(stderr);
  abort();
}

// Function to decrypt data using AES-256-CBC
std::vector<unsigned char> aesDecrypt(const std::vector<unsigned char>& ciphertext, const unsigned char* key, const unsigned char* iv) {
  EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
  if (!ctx) handleErrors();

  if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
      handleErrors();

  std::vector<unsigned char> plaintext(ciphertext.size() + AES_BLOCK_SIZE);
  int len;
  if (1 != EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), ciphertext.size()))
      handleErrors();
  int plaintext_len = len;

  if (1 != EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len))
      handleErrors();
  plaintext_len += len;

  EVP_CIPHER_CTX_free(ctx);
  plaintext.resize(plaintext_len);
  return plaintext;
}



