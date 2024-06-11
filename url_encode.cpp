#include "url_enc_dec.hpp"
#include <cstdint>

#ifdef _WIN32
#include <sys/stat.h>
#else
#include <sys/ioctl.h>
#endif

/* Variables */
static bool ENCODE = true;
static bool FULL = false;
static bool SPECIAL = false;

static inline int __available_on_stdin(){
  int available = 0;
  #ifdef _WIN32
    static HANDLE handle = GetStdHandle(STD_INPUT_HANDLE);
    PeekNamedPipe(handle, nullptr, 0, nullptr, &available, nullptr);
  #else
    ioctl(0, FIONREAD, &available);
  #endif
  return available;
}

static inline void usage(){
  printf("%s\n", R"(
** url - encode or decode data on stdin **

** Usage **

url [OPTIONS]

** Options **

-d, --decode      : Encodes all characters that may cause issues in URLs.
-f, --full        : Encodes characters reserved in the URL syntax (e.g., '&', '+').
-s, --special     : Encodes characters that might be misinterpreted in URLs (e.g., spaces, '$').

** Examples **
url <<< https://github.com/Gurguii/gpkih

)");
}


int main(int argc, const char **args) {

  for(int i = 1; i < argc; ++i){
    std::string_view opt = args[i];
    if("-d" == opt || "--decode" == opt){
      ENCODE = false;
    }else if("-f" == opt || "--full" == opt){
      FULL = true;
    }else if("-s" == opt || "--special" == opt){
      SPECIAL = true;
    }else if("-h" == opt | "--help" == opt){
      usage(); return 0;
    }else{
      fprintf(stderr, "-- Unknown option '%s'\n", args[i]);
    }
  }

  int available = __available_on_stdin();
  
  if(available <= 0){
    fprintf(stderr, "[!] Got nothing in stdin to %s\n", ENCODE ? "encode" : "decode");
    return 1;
  }

  uint8_t* data = (uint8_t*)malloc(available);

  if(data == nullptr){
    fprintf(stderr, "[!] Couldn't allocate '%i' bytes\n", available);
    return 1;
  }

  if(fread(data, available, 1, stdin) != 1){
    fprintf(stderr, "[!] Copied less bytes to buffer than available in stdin\n");
    return 1;
  };

  std::string result;
  
  if(ENCODE){
    result = FULL ? url::encode::full(reinterpret_cast<const char*>(data), available) : url::encode::reserved(reinterpret_cast<const char*>(data), available);
  }else{
    result = url::decode::full(reinterpret_cast<const char*>(data), available);
  }

  printf("%s\n",result.c_str());
  return 0;
}