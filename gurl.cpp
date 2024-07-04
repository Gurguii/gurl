#include "url_enc_dec.hpp"
#include <cstdio>
#include <cstring>

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/ioctl.h>
#endif

/* Variables */
static bool ENCODE   = true;
static bool FULL     = false;
static bool SPECIAL  = false;

static FILE *INFILE  = stdin;
static FILE *OUTFILE = stdout;

inline static constexpr const char *ASCII_RESERVED = "!#$&'()*+,/:;=?@[]";
inline static constexpr const int ASCII_RESERVED_COUNT = 18;

inline static constexpr const char *ASCII_SPECIAL = "\"%-.<>\\^_`{|}~";
inline static constexpr const int ASCII_SPECIAL_COUNT = 14;

static char ENCODE_CHARSET[255]{};
static int ENCODE_CHARSET_SIZE = 0;

static inline int __available_on_stdin(){
  unsigned long available = 0;
  #ifdef _WIN32
    static HANDLE handle = GetStdHandle(STD_INPUT_HANDLE);
    PeekNamedPipe(handle, nullptr, 0, nullptr, &available, nullptr);
  #else
    ioctl(0, FIONREAD, &available);
  #endif
  return available;
}

static inline long __available_on_file(FILE *file){
  fseek(file, 0, SEEK_END);
  long av = ftell(file);
  fseek(file, 0, SEEK_SET);
  return av;
};

static inline void usage(){
  printf(R"(** Gurl **
URL encode or decode data

** Usage **
./gurl [OPTIONS]

** Options **
Note: by default the program will encode reserved characters, the -r option
only makes sense if -s | --special is given and you also want to encode reserved characters.
  
-d, --decode         : Decode
-r, --reserved       : Encodes ASCII reserved characters %s
-s, --special        : Encodes ASCII special characters %s - https://datatracker.ietf.org/doc/rfc3986/
-f, --full           : Encodes every character
-o, --out     <file> : Output data to file instead of stdout
-i, --in      <file> : Read data from file instead of stdin
-c, --charset <str>  : Custom charset to encode

** Examples **
echo '/my_/$foo_example' | gurl        => %%2fmy_%%2f%%24foo_example
echo '/my_/$foo_example' | gurl -s     => /my%%5f/$foo%%5fexample
echo '/my_/$foo_example' | gurl -f     => %%2f%%6d%%79%%5f%%2f%%24%%66%%6f%%6f%%5f%%65%%78%%61%%6d%%70%%6c%%65%%0a
echo '/my_/$foo_example' | gurl -c fo$ => /my_/%%24%%66%%6f%%6f_example
echo '/my_/$foo_example' | gurl -r -s  => %%2fmy%%5f%%2f%%24foo%%5fexample
gurl -f -i /etc/passwd -o encoded
gurl -d -i encoded -o decoded
)",ASCII_RESERVED, ASCII_SPECIAL);
}

static int parse(int argc, const char **args){
  for(int i = 1; i < argc; ++i){
    std::string_view opt = args[i];
    if("-d" == opt || "--decode" == opt){
      ENCODE = false;
    }else if("-f" == opt || "--full" == opt){
      FULL = true;
    }else if("-r" == opt || "--reserved" == opt){
      if(ENCODE_CHARSET_SIZE + ASCII_RESERVED_COUNT >= 255){
        fprintf(stderr, "-- Charset exceeds maximum size (255)\n");
        return -1;
      }
      memcpy(ENCODE_CHARSET+ENCODE_CHARSET_SIZE, ASCII_RESERVED, ASCII_RESERVED_COUNT);
      ENCODE_CHARSET_SIZE+=ASCII_RESERVED_COUNT;
    }else if("-s" == opt || "--special" == opt){
      if(ENCODE_CHARSET_SIZE + ASCII_SPECIAL_COUNT >= 255){
        fprintf(stderr, "-- Charset exceeds maximum size (255)\n");
        return -1;
      }
      memcpy(ENCODE_CHARSET+ENCODE_CHARSET_SIZE, ASCII_SPECIAL, ASCII_SPECIAL_COUNT);
      ENCODE_CHARSET_SIZE+=ASCII_SPECIAL_COUNT;
    }else if("-h" == opt || "--help" == opt){
      usage(); return 1;
    }else if("-o" == opt || "--out" == opt){
      if(i+1 == argc){
        fprintf(stderr, "-- Missing argument for '%s'\n",opt.data()); return 1;
      }
      OUTFILE = fopen(args[++i], "wb");
      if(OUTFILE == nullptr){
        perror("-- Couldn't open OUTPUT file\n"); return 1;
      }
    }else if("-i" == opt || "--infile" == opt){
      if(i+1 == argc){
        fprintf(stderr, "-- Missing argument for '%s'\n",opt.data()); return 1;
      }
      INFILE = fopen(args[++i], "rb");
      if(INFILE == nullptr){
        perror("-- Couldn't open INPUT file\n"); return 1;
      }
    }else if("-c" == opt || "--charset" == opt){
      if(i+1 == argc){
        fprintf(stderr, "-- Missing argument for '%s'\n", opt.data()); return 1;
      }
      size_t charsetSize = strlen(args[++i]);
      if((ENCODE_CHARSET_SIZE + charsetSize) >= 255){
        fprintf(stderr, "-- Charset exceeds maximum size (255)\n");
        return -1;
      }
      memcpy(ENCODE_CHARSET+ENCODE_CHARSET_SIZE,args[i],charsetSize);
      ENCODE_CHARSET_SIZE+=charsetSize;
    }else{
      fprintf(stderr, "-- Unknown option '%s'\n", args[i]);
    }
  }

  if(ENCODE_CHARSET_SIZE == 0){
    memcpy(ENCODE_CHARSET, ASCII_RESERVED, ASCII_RESERVED_COUNT);
  }
  
  return 0;
}

int main(int argc, const char **args) {
  if(parse(argc, args)){
    return -1;
  }

  long long available = 0;

  available = INFILE == stdin ? __available_on_stdin() : __available_on_file(INFILE);
  
  if(available <= 0){
    fprintf(stderr, "[!] Got no data to %s, use -h | --help to display help\n", ENCODE ? "encode" : "decode");
    return 1;
  }

  unsigned char* data = (unsigned char*)malloc(available);

  if(data == nullptr){
    fprintf(stderr, "[!] Couldn't allocate enough memory\n");
    return 1;
  }

  if(fread(data, available, 1, INFILE) != 1){
    if(ferror(INFILE)){
      perror("Reading data to buffer");
      return 1;
    }
    // This is probably an unexpected END OF FILE
    // which might happen if using stdin in Windows (Fuck Windows)
    // so no worries
  };

  std::string result;

  if(ENCODE){
    FULL ? url::encode::full(data, available, OUTFILE)
         : url::encode::partial(data, available, ENCODE_CHARSET,ENCODE_CHARSET_SIZE, OUTFILE);
  }else{
    url::decode::full(reinterpret_cast<const char*>(data), available, OUTFILE);
  }

  if(INFILE != stdin){
    fclose(INFILE);
  }

  if(OUTFILE != stdout){
    fclose(OUTFILE);
  }

  free(data);

  return 0;
}