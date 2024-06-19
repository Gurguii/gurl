#include "url_enc_dec.hpp"

#ifdef _WIN32
#include <sys/stat.h>
#else
#include <sys/ioctl.h>
#endif

/* Variables */
static bool ENCODE = true;
static bool FULL = false;
static bool SPECIAL = false;
static url::encode::charset DESIRED_CHARSET = url::encode::charset::none;

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
-s, --special     : Encodes ascii special characters  - \"%-.<>\\^_`{|}~
-r, --reserved    : Encodes ascii reserved characters - !#$&'()*+,/:;=?@[]

** Examples **
url -f    <<< '/my_/$foo_example' - %2f%6d%79%5f%2f%24%66%6f%6f%5f%65%78%61%6d%70%6c%65%0a
url -r    <<< '/my_/$foo_example' - %2fmy_%2f%24foo_example
url -s    <<< '/my_/$foo_example' - /my%5f/$foo%5fexample
url -s -r <<< '/my_/$foo_example' - %2fmy%5f%2f%24foo%5fexample
)");
}

static int parse(int argc, const char **args){
  for(int i = 1; i < argc; ++i){
    std::string_view opt = args[i];
    if("-d" == opt || "--decode" == opt){
      ENCODE = false;
    }else if("-f" == opt || "--full" == opt){
      FULL = true;
    }else if("-s" == opt || "--special" == opt){
      DESIRED_CHARSET |= url::encode::charset::special;
    }else if("-r" == opt || "--reserved" == opt){
      DESIRED_CHARSET |= url::encode::charset::reserved;
    }else if("-h" == opt || "--help" == opt){
      usage(); return 1;
    }else{
      fprintf(stderr, "-- Unknown option '%s'\n", args[i]);
    }
  }
  return 0;
}

int main(int argc, const char **args) {
  if(parse(argc, args)){
    return -1;
  }

  int available = __available_on_stdin();
  
  if(available <= 0){
    fprintf(stderr, "[!] Got nothing in stdin to %s, use -h | --help to display help\n", ENCODE ? "encode" : "decode");
    return 1;
  }

  unsigned char* data = (unsigned char*)malloc(available);

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
    if(FULL == false && DESIRED_CHARSET == url::encode::charset::none){
      fprintf(stderr, "[!] No encoding option given, use -h | --help to see available options\n");
      return 1;
    }
    result = FULL ? url::encode::full(reinterpret_cast<const unsigned char*>(data), available, stdout)
                  : url::encode::partial(reinterpret_cast<const unsigned char*>(data), available, DESIRED_CHARSET, stdout);
  }else{
    result = url::decode::full(reinterpret_cast<const unsigned char*>(data), available, stdout);
  }

  //printf("%s\n",result.c_str());
  return 0;
}