#ifndef gurgui_url_encoder_decoder
#define gurgui_url_encoder_decoder
#pragma once

#include <cstdint>
#include <cstring>
#include <sstream>

namespace url::charset{
  inline static constexpr const char *ascii_reserved = "!#$&'()*+,/:;=?@[]";
  inline static constexpr const int ascii_reserved_count = 18;
  
  inline static constexpr const char *ascii_special = "\"%-.<>\\^_`{|}~";
  inline static constexpr const int ascii_special_count = 14;
}

inline static const auto hex2byte = [](const unsigned char *byte, std::basic_ostringstream<char> &buff) {
  uint8_t digit = 0;
  for (int i = 0; i < 2; ++i) {
    char c = byte[i];
    if (c >= '0' && c <= '9') {
      digit += (c - '0') << (i == 0 ? 4 : 0);
    } else if (c >= 'a' && c <= 'f') {
      digit += (c - 'a' + 10) << (i == 0 ? 4 : 0);
    } else if (c >= 'A' && c <= 'F') {
      digit += (c - 'A' + 10) << (i == 0 ? 4 : 0);
    }
  }
  buff << static_cast<char>(digit);
};

namespace {
  static constexpr const char *hex_vals = "0123456789ABCDEF";
  static inline std::basic_ostringstream<char> __encode_to_stream(const unsigned char* data, size_t const dataSize){
    std::basic_ostringstream<char> result{};
    for(size_t i = 0; i < dataSize; ++i){
      result << '%' << hex_vals[data[i]/16] << hex_vals[data[i]%16] || result << data[i];
    }
    return result;
  }
  static inline std::basic_ostringstream<char> __encode_to_stream(const unsigned char* data, size_t const dataSize, const char *charset, size_t const charsetSize){
    std::basic_ostringstream<char> result{};
    for(size_t i = 0; i < dataSize; ++i){
      memchr(charset, data[i], charsetSize) != nullptr && result << '%' << hex_vals[data[i]/16] << hex_vals[data[i]%16] || result << data[i];
    }
    return result;
  }
  static inline std::basic_ostringstream<char> __decode_to_stream(const unsigned char* data, size_t const dataSize){
    std::basic_ostringstream<char> result{};
    for(int i = 0; i < dataSize; ++i){
      if(data[i] == '%'){
        hex2byte(data+i+1,result);
        i+=2;
        continue;
      }
      result << data[i];
    }
    return result;
  }
}

namespace url::encoding{
  static inline void encode(const char *data, size_t const dataSize, FILE* const outPath){
    auto result{__encode_to_stream(reinterpret_cast<const unsigned char*>(data), dataSize)};  
    fwrite(&result.str()[0], result.str().size(), 1, outPath);
  }
  
  static inline void encode(const char *data, size_t dataSize, const char *charset, size_t const charsetSize, FILE *const outPath){
    auto result{__encode_to_stream(reinterpret_cast<const unsigned char*>(data), dataSize, charset, charsetSize)};
    fwrite(&result.str()[0], result.str().size(), 1, outPath);
  }

  [[nodiscard("Ignoring returned value")]]
  static inline std::basic_ostringstream<char> encode(const char *const data, size_t const dataSize){
    return __encode_to_stream(reinterpret_cast<const unsigned char*>(data), dataSize);
  }

  [[nodiscard("Ignoring returned value")]]
  static inline std::basic_ostringstream<char> encode(const char *const data, size_t const dataSize, const char *const charset, size_t const charsetSize){
    return __encode_to_stream(reinterpret_cast<const unsigned char*>(data), dataSize, charset, charsetSize);
  }
}

namespace url::decoding{
  static inline void decode(const char *const data, size_t const dataSize, FILE *const outFile){
    auto result{__decode_to_stream(reinterpret_cast<const unsigned char*>(data), dataSize)};
    fwrite(&result.str()[0], result.str().size(), 1, outFile);
  }

  [[nodiscard("Ignoring returned value")]] 
  static inline std::basic_ostringstream<char> decode(const char *const data, size_t const dataSize){
    return __decode_to_stream(reinterpret_cast<const unsigned char*>(data), dataSize);
  }
}

#endif