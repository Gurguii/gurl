#pragma once
#include <cstdint>
#include <string>
#include <cstring>
#include <sstream>

static constexpr const char *ASCII_RESERVED = "!#$&'()*+,/:;=?@[]";
static constexpr const int ASCII_RESERVED_COUNT = 18;

static constexpr const char *ASCII_SPECIAL = "\"%-.<>\\^_`{|}~";
static constexpr const int ASCII_SPECIAL_COUNT = 14;

static inline const auto int_to_hex = [](const uint8_t &byte, std::ostringstream &buff){
  static constexpr const char *hex_vals = "0123456789abcdef";
  buff << '%';
  buff << hex_vals[byte/16];
  buff << hex_vals[byte%16];
};

static inline const auto hex_to_int = [](const char *byte, std::ostringstream &buff){
  const char &first_half = *byte;
  const char &second_half = *(byte+1);
  uint8_t digit;

  if(first_half >= '0' && first_half <= '9'){
    digit = first_half * 16;
  }else if(first_half >= 'a' && first_half <= 'f'){
    digit = 'f' - first_half + 10 * 16; 
  }else if(first_half >= 'A' && first_half <= 'F'){
    digit = 'F' - first_half + 10 * 16;
  }

  if(second_half >= '0' && second_half <= '9'){
    digit += second_half;
  }else if(second_half >= 'a' && second_half <= 'f'){
    digit += 'f' - second_half + 15;
  }else if(second_half >= 'A' && second_half <= 'F'){
    digit += 'F' - second_half + 15;
  }

  buff << digit;
};

namespace url::encode{
  static inline std::string full(const char *data, size_t const data_size){
    std::ostringstream result{};
    
    for(size_t i = 0; i < data_size; ++i){
      int_to_hex(data[i],result);
    }
  
    return result.str();
  }
  
  static inline std::string special(const char *data, size_t data_size){
    std::ostringstream result{};

    for(size_t i = 0; i < data_size; ++i){
      if( memchr(ASCII_SPECIAL, data[i], ASCII_SPECIAL_COUNT) == nullptr){
        result <<  data[i];
      }else{
        int_to_hex(data[i],result);
      }
    }

    return result.str();
  }
  
  static inline std::string reserved(const char *data, size_t data_size){
    std::ostringstream result{};
  
    for(size_t i = 0; i < data_size; ++i){
      if( memchr(ASCII_RESERVED, data[i], ASCII_RESERVED_COUNT) == nullptr){
        result << data[i];
      }else{
        int_to_hex(data[i],result);
      }
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