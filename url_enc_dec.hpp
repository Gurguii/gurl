#pragma once

#include <cstdint>
#include <string>
#include <cstring>
#include <sstream>

inline static constexpr const char *ASCII_RESERVED = "!#$&'()*+,/:;=?@[]";
inline static constexpr const int ASCII_RESERVED_COUNT = 18;

inline static constexpr const char *ASCII_SPECIAL = "\"%-.<>\\^_`{|}~";
inline static constexpr const int ASCII_SPECIAL_COUNT = 14;

inline static const auto int_to_hex = [](const uint8_t &byte, std::ostringstream &buff){
  static constexpr const char *hex_vals = "0123456789abcdef";
  buff << '%';
  buff << hex_vals[byte/16];
  buff << hex_vals[byte%16];
  return true;
};

inline static const auto hex_to_int = [](const char *byte, std::ostringstream &buff){
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

  buff << static_cast<char>(digit);
};

namespace url::encode{
  enum class charset : uint8_t{
    special  =   1 << 0 & 0xff,
    reserved =   1 << 1 & 0xff,
    all      =  (1 << 2 & 0xff) - 1,
    none     =   0
  };
  static inline bool operator&(charset lo, charset ro){
    return static_cast<bool>(static_cast<uint8_t>(lo) & static_cast<uint8_t>(ro));
  }
  static inline void operator|=(charset &lo, charset ro){
    lo = static_cast<charset>(static_cast<uint8_t>(lo) | static_cast<uint8_t>(ro));
  }

  static inline std::string full(const char *data, size_t const data_size){
    std::ostringstream result{};
    
    for(size_t i = 0; i < data_size; ++i){
      int_to_hex(data[i],result);
    }
  
    return result.str();
  }
  

  static inline std::string partial(const char *data, size_t data_size, charset desired){
    std::ostringstream result{};

    for(size_t i = 0; i < data_size; ++i){
      if(desired & charset::special && memchr(ASCII_SPECIAL, data[i], ASCII_SPECIAL_COUNT) != nullptr){
        int_to_hex(data[i], result);
        continue;
      }
      if(desired & charset::reserved && memchr(ASCII_RESERVED, data[i], ASCII_RESERVED_COUNT) != nullptr){
        int_to_hex(data[i], result);
        continue;
      }
      result << data[i];
    }

    return result.str();
  }
}

namespace url::decode{
  static inline std::string full(const char *data, size_t const data_size){
    std::ostringstream result{};
    for(int i = 0; i < data_size; ++i){
      if(data[i] == '%'){
        hex_to_int(data+i+1,result);
        i+=2;
      }else{
        result << data[i];
      }
    }
    return result.str();
  }
}