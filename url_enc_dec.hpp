#pragma once

#include <cstring>
#include <sstream>

static constexpr const char *ASCII_RESERVED = "!#$&'()*+,/:;=?@[]";
static constexpr const int   ASCII_RESERVED_COUNT = 18;

static constexpr const char *ASCII_SPECIAL = "\"%-.<>\\^_`{|}~";
static constexpr const int   ASCII_SPECIAL_COUNT = 14;
static constexpr const char *hex_vals = "0123456789abcdef";

static void int_to_hex(const unsigned char &byte, std::ostringstream &buff){
  buff << '%';
  buff << hex_vals[byte/16];
  buff << hex_vals[byte%16];
};

static void hex_to_int(const unsigned char *byte, std::ostringstream &buff){
  const unsigned char &first_half = *byte;
  const unsigned char &second_half = *(byte+1);
  unsigned char digit = 0;
  
  first_half >= '0' && first_half <= '9' && (digit = (first_half - '0') * 16)
  || first_half >= 'a' && first_half <= 'f' && (digit = (first_half - 'a' + 10) * 16)
  || first_half >= 'A' && first_half <= 'F' && (digit = (first_half - 'A' + 10) * 16);

  second_half >= '0' && second_half <= '9' && (digit += second_half - '0')
  || second_half >= 'a' && second_half <= 'f' && (digit += second_half - 'a' + 10)
  || second_half >= 'A' && second_half <= 'F' && (digit += second_half - 'A' + 10);

  buff << digit;
};

namespace url::encode{
  enum class charset : unsigned char{
    special  =   1 << 0 & 0xff,
    reserved =   1 << 1 & 0xff,
    all      =  (1 << 2 & 0xff) - 1,
    none     =   0
  };
  static inline bool operator&(charset lo, charset ro){
    return static_cast<bool>(static_cast<unsigned char>(lo) & static_cast<unsigned char>(ro));
  }
  static inline void operator|=(charset &lo, charset ro){
    lo = static_cast<charset>(static_cast<unsigned char>(lo) | static_cast<unsigned char>(ro));
  }

  static inline std::string full(const unsigned char *data, size_t const data_size, FILE *outFile = nullptr){
    std::ostringstream result{};
    
    for(size_t i = 0; i < data_size; ++i){
      int_to_hex(data[i],result);
    }
    
    if(outFile != nullptr){
      fwrite(&result.str()[0], result.str().size(), 1, outFile);  
    }
    
    return result.str();
  }
  
  static inline std::string partial(const unsigned char *data, size_t data_size, charset desired, FILE *outFile = nullptr){
    std::ostringstream result{};

    for(size_t i = 0; i < data_size; ++i){
      if(desired & charset::special  && memchr(ASCII_SPECIAL, data[i], ASCII_SPECIAL_COUNT)!= nullptr && result << '%' << hex_vals[data[i]/16] << hex_vals[data[i]%16]){
        continue;
      }
      if(desired & charset::reserved  && memchr(ASCII_RESERVED, data[i], ASCII_RESERVED_COUNT)!= nullptr && result << '%' << hex_vals[data[i]/16] << hex_vals[data[i]%16]){
        continue;
      }
      result << data[i];
    }

    if(outFile != nullptr){
      fwrite(&result.str()[0], result.str().size(), 1, outFile);  
    }

    return result.str();
  }
}

namespace url::decode{
  static inline std::string full(const unsigned char *data, size_t const data_size, FILE *outFile = nullptr){
    std::ostringstream result{};
    for(int i = 0; i < data_size; ++i){
      if(data[i] == '%'){
        hex_to_int(data+i+1,result);
        i+=2;
      }else{
        result << data[i];
      }
    }
    if(outFile != nullptr){
      fwrite(&result.str()[0], result.str().size(), 1, outFile);  
    }
    return result.str();
  }
}