#include <iostream>
#include <stdint.h>

#include <array>
#include <string>
#include <vector>

uint32_t leftRotate(uint32_t n, uint32_t d) {
  return (n << d) | (n >> (32 - d));
}

uint32_t rightRotate(uint32_t n, uint32_t d) {
  return (n >> d) | (n << (32 - d));
}

uint32_t endianSwap(uint32_t n) {
  uint32_t ret = 0;
  ret |= (n & 0xff000000) >> 24;
  ret |= (n & 0x00ff0000) >> 8;
  ret |= (n & 0x0000ff00) << 8;
  ret |= (n & 0x000000ff) << 24;
  return ret;
}

int main(int argc, char* argv[]) {
  std::string message = "";
  if (argc > 1) {
    message = argv[1];
  }
  std::cout << "Message: \"" << message << "\"" << std::endl;

  // Initial hash values
  // First 32 bits of fractional portions of square roots of first 8 primes
  // 2, 3, 5, 7, 11, 13, 17, 19
  uint32_t h0 = 0x6a09e667;
  uint32_t h1 = 0xbb67ae85;
  uint32_t h2 = 0x3c6ef372;
  uint32_t h3 = 0xa54ff53a;
  uint32_t h4 = 0x510e527f;
  uint32_t h5 = 0x9b05688c;
  uint32_t h6 = 0x1f83d9ab;
  uint32_t h7 = 0x5be0cd19;

  // Round constants
  // First 32 bits of fractional portions of cube roots of first 64 primes
  // (2..311)
  uint32_t k[64] = {
      // clang-format off
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
      // clang-format on
  };

  uint64_t L = message.size() * 8; // Initial message size in bits

  std::vector<bool> vBits;
  for (const auto chr : message) {
    for (int i = 7; i >= 0; i--)
      vBits.push_back(chr & (1 << i));
  }

  // Append a one bit after message
  vBits.push_back(1);

  // Pad bits to a multiple of 512 (leave room for length)
  while ((vBits.size() + 64) % 512 != 0)
    vBits.push_back(0);
  // Append length to bits
  for (int i = 63; i >= 0; i--)
    vBits.push_back(L & ((uint64_t)1 << i));

  int numChunks = vBits.size() / 512;

  std::cout << "bits/chunks: " << vBits.size() << "/" << numChunks << std::endl;
  std::cout << "bits:" << std::endl;
  for (int i = 0; i < vBits.size(); i++) {
    std::cout << vBits[i];
    if ((i + 1) % 8 == 0)
      std::cout << ' ';
    if ((i + 1) % 32 == 0)
      std::cout << std::endl;
    if ((i + 1) % 512 == 0)
      std::cout << std::endl;
  }

  // Convert into 512-bit chunks
  std::vector<std::array<uint32_t, 16>> vChunks;
  for (int chunkIndex = 0; chunkIndex < numChunks; chunkIndex++) {
    std::array<uint32_t, 16> chunk;
    std::fill(chunk.begin(), chunk.end(), 0);
    for (int i = 0; i < 16; i++) {
      for (int j = 0; j < 32; j++) {
        chunk[i] = chunk[i] | uint32_t(vBits[(chunkIndex * 512) + (i * 32) + j])
                                  << (31 - j);
      }
    }
    vChunks.push_back(chunk);
  }

  for (const auto chunk : vChunks) {
    uint32_t w[64] = {
        // clang-format off
      chunk[0], chunk[1], chunk[2], chunk[3], chunk[4], chunk[5], chunk[6], chunk[7],
      chunk[8], chunk[9], chunk[10], chunk[11], chunk[12], chunk[13], chunk[14], chunk[15],
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
        // clang-format on
    };

    for (int i = 16; i < 64; i++) {
      uint32_t s0 = rightRotate(w[i - 15], 7) ^ rightRotate(w[i - 15], 18) ^
                    (w[i - 15] >> 3);
      uint32_t s1 = rightRotate(w[i - 2], 17) ^ rightRotate(w[i - 2], 19) ^
                    (w[i - 2] >> 10);
      w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }

    uint32_t a = h0;
    uint32_t b = h1;
    uint32_t c = h2;
    uint32_t d = h3;
    uint32_t e = h4;
    uint32_t f = h5;
    uint32_t g = h6;
    uint32_t h = h7;

    for (int i = 0; i < 64; i++) {
      uint32_t s1 = rightRotate(e, 6) ^ rightRotate(e, 11) ^ rightRotate(e, 25);
      uint32_t ch = (e & f) ^ (~e & g);
      uint32_t temp1 = h + s1 + ch + k[i] + w[i];

      uint32_t s0 = rightRotate(a, 2) ^ rightRotate(a, 13) ^ rightRotate(a, 22);
      uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
      uint32_t temp2 = s0 + maj;

      h = g;
      g = f;
      f = e;
      e = d + temp1;
      d = c;
      c = b;
      b = a;
      a = temp1 + temp2;
    }

    h0 += a;
    h1 += b;
    h2 += c;
    h3 += d;
    h4 += e;
    h5 += f;
    h6 += g;
    h7 += h;
  }

  unsigned char digest[33];

  *reinterpret_cast<uint32_t*>(&digest[0]) = endianSwap(h0);
  *reinterpret_cast<uint32_t*>(&digest[4]) = endianSwap(h1);
  *reinterpret_cast<uint32_t*>(&digest[8]) = endianSwap(h2);
  *reinterpret_cast<uint32_t*>(&digest[12]) = endianSwap(h3);
  *reinterpret_cast<uint32_t*>(&digest[16]) = endianSwap(h4);
  *reinterpret_cast<uint32_t*>(&digest[20]) = endianSwap(h5);
  *reinterpret_cast<uint32_t*>(&digest[24]) = endianSwap(h6);
  *reinterpret_cast<uint32_t*>(&digest[28]) = endianSwap(h7);
  digest[32] = 0x00;

  std::cout << "digest (hex): ";
  for (int i = 0; i < 32; i++) {
    std::cout << std::hex << (int)digest[i];
  }
  std::cout << std::endl;

  return 0;
}
