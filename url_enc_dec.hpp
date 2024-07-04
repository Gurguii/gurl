#pragma once

#include <cstdint>
#include <cstring>
#include <sstream>

static constexpr const char *hex_vals = "0123456789ABCDEF";

inline static const auto hex2byte = [](const char *byte, std::basic_ostringstream<char> &buff){
  const char &first_half = *byte;
  const char &second_half = *(byte+1);
  uint8_t digit;

  if(first_half >= '0' && first_half <= '9'){
    digit = (first_half - '0') * 16;
  }else if(first_half >= 'a' && first_half <= 'f'){
    digit = (first_half - 'a' + 10) * 16;
  }else if(first_half >= 'A' && first_half <= 'F'){
    digit = (first_half - 'A' + 10) * 16;
  }

  if(second_half >= '0' && second_half <= '9'){
    digit += second_half - '0';
  }else if(second_half >= 'a' && second_half <= 'f'){
    digit += second_half - 'a' + 10;
  }else if(second_half >= 'A' && second_half <= 'F'){
    digit += second_half - 'A' + 10;
  }

  buff << digit;
};

namespace url::encode{
  static inline void full(unsigned char *data, size_t const dataSize, FILE* outPath){
    std::basic_ostringstream<char> result{};
    
    for(size_t i = 0; i < dataSize; ++i){
      result << '%' << hex_vals[data[i]/16] << hex_vals[data[i]%16];
    }
  
    fwrite(&result.str()[0], result.str().size(), 1, outPath);
  }

  static inline std::basic_ostringstream<char> full(unsigned char *data, size_t const dataSize){
    std::basic_ostringstream<char> result{};
    
    for(size_t i = 0; i < dataSize; ++i){
      result << '%' << hex_vals[data[i]/16] << hex_vals[data[i]%16];
    }
  
    return result;
  }

  static inline void partial(unsigned char *data, size_t dataSize, const char *charset, size_t charsetSize, FILE *outPath){
    std::basic_ostringstream<char> result{};

    for(size_t i = 0; i < dataSize; ++i){
      memchr(charset, data[i], charsetSize) != nullptr && result << '%' << hex_vals[data[i]/16] << hex_vals[data[i]%16] || result << data[i];
    }

    fwrite(&result.str()[0], result.str().size(), 1, outPath);
  }

  static inline std::basic_ostringstream<char> partial(unsigned char *data, size_t dataSize, const char *charset, size_t charsetSize){
    std::basic_ostringstream<char> result{};

    for(size_t i = 0; i < dataSize; ++i){
      memchr(charset, data[i], charsetSize) != nullptr && result << '%' << hex_vals[data[i]/16] << hex_vals[data[i]%16] || result << data[i];
    }

    return result;
  }
}

namespace url::decode{
  static inline void full(const char *data, size_t const dataSize, FILE* outFile){
    std::basic_ostringstream<char> result{};
    for(int i = 0; i < dataSize; ++i){
      if(data[i] == '%'){
        hex2byte(data+i+1,result);
        i+=2;
      }else{
        result << data[i];
      }
    }
    fwrite(&result.str()[0], result.str().size(), 1, outFile);
  }

  static inline std::basic_ostringstream<char> full(const char *data, size_t const dataSize){
    std::basic_ostringstream<char> result{};
    for(int i = 0; i < dataSize; ++i){
      if(data[i] == '%'){
        hex2byte(data+i+1,result);
        i+=2;
      }else{
        result << data[i];
      }
    }
    return result;
  }
}