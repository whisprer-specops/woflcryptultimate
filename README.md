# [README.md]

AES-CBC + Laplace Noise Encryption/Decryption Project
This project demonstrates a hybrid cryptographic system that combines AES-256-CBC encryption with Laplace noise masking to simulate enhanced privacy via differential privacy techniques. The system includes both encryption and decryption flows implemented in modern C++ with OpenSSL.

## Project Structure
github_desktop/
├── `encrypt.md`                  # Design notes on encryption strategy
├── `decrypt.md`                  # Design notes on decryption strategy
├── `what's_CBC_used_for.md`      # Background on CBC vulnerabilities and applications
├── `wcu.cpp`                     # Working Encrypt + Noise Masking (CBC + Laplace)
├── `wdcu.cpp`                    # Main decryption wrapper (depends on parts)
├── `wdcu_parts_for_edu.cpp`      # Modular functions for noise removal and AES decryption

## Encryption Pipeline
Implemented in: `wcu.cpp`
Uses AES-256-CBC mode for secure symmetric encryption.
Post-encryption, Laplace-distributed noise is added to each byte to simulate privacy masking.
Key and IV are randomly generated using OpenSSL’s RAND_bytes.
Laplace Noise Generation: Uses inverse transform sampling with a configurable scale parameter.

## Decryption Pipeline
Main files:
`wdcu.cpp` – Orchestrates decryption flow
`wdcu_parts_for_edu.cpp` – Contains helper functions:

Noise Removal:
Regenerates Laplace noise using known scale and seed.
Subtracts the noise from the received ciphertext.
AES Decryption:
Performs AES-256-CBC decryption on cleaned data.
Outputs the original plaintext.

## Build Requirements
C++17+
OpenSSL (tested with 1.1.1 and 3.x)

## Build Example (Linux/WSL):
bash
`g++ wcu.cpp -o encrypt_exec -lssl -lcrypto`
`g++ wdcu.cpp wdcu_parts_for_edu.cpp -o decrypt_exec -lssl -lcrypto`

## Example Flow
Run encrypt_exec to encrypt and mask your plaintext.
Capture the noised ciphertext output.
Use decrypt_exec with the same key, IV, noise scale, and seed to recover the plaintext.

## Notes
All noise generation is deterministic with a given seed.
AES key/IV must be securely shared between sender/receiver.
For real-world use, Laplace masking alone is not sufficient for cryptographic secrecy—it's intended here as an educational enhancement inspired by differential privacy.

---

Encryption Pathway::
[Plaintext]
     |
     v
[AES-256-CBC Encrypt] --> [Ciphertext]
     |
     v
[Add Laplace Noise]
     |
     v
[Noised Ciphertext] → (Send/store)

---

Decryption Pathway::
[Noised Ciphertext]
     |
     v
[Remove Laplace Noise]
     |
     v
[Clean Ciphertext]
     |
     v
[AES-256-CBC Decrypt]
     |
     v
[Plaintext]

---
