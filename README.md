# Gurl: A Command-Line URL Encoder/Decoder

`Gurl` is a lightweight command-line utility for URL encoding and decoding data. It processes data from standard input (`stdin`) or a specified file and outputs the result to standard output (`stdout`) or a file.

---

## Key Features

* **Flexible Modes**: Easily switch between URL encoding and decoding.
* **Targeted Encoding**: Encode only specific character sets (reserved, special, or custom).
* **File I/O**: Read input from and write output to files, making it suitable for scripting.
* **Piping Support**: Seamlessly integrates with Unix-like pipes for command chaining.

---

## Building

```bash
git clone https://github.com/Gurguii/gurl
mkdir gurl/build
cd !$
cmake ..
make
``` 

----
## Options  
| Option | Description |
| :--- | :--- |
| -d, --decode | Decode |
| -r, --reserved | Encodes ASCII reserved characters %s |
| -s, --special | Encodes ASCII special characters %s - https://datatracker.ietf.org/doc/rfc3986/ |
| -f, --full | Encodes every character |
| -o, --out \<file> | Output data to file instead of stdout |
| -i, --in \<file> | Read data from file instead of stdin |
| -c, --charset \<str> | Custom charset to encode |  

----
## EXAMPLES
Encodes only the reserved characters, leaving the unreserved characters as they are
- `echo '/my_/$foo_example' | gurl` => `%%2fmy_%%2f%%24foo_example`  
Encodes only the special characters.
- `echo '/my_/$foo_example' | gurl -s` => `/my%%5f/$foo%%5fexample`  
Encodes every character in the string.  
- `echo '/my_/$foo_example' | gurl -f` => `%%2f%%6d%%79%%5f%%2f%%24%%66%%6f%%6f%%5f%%65%%78%%61%%6d%%70%%6c%%65%%0a`  
Encodes the custom charset "fo$".
- `echo '/my_/$foo_example' | gurl -c fo$` => `/my_/%%24%%66%%6f%%6f_example`  
Encodes both reserved and special characters.
- `echo '/my_/$foo_example' | gurl -r -s` => `%%2fmy%%5f%%2f%%24foo%%5fexample`  
Reads the file `/etc/passwd`, fully encodes its contents, and writes the output to a new file named `encoded`.
- `gurl -f -i /etc/passwd -o encoded`
Reads the file `encoded`, decodes its contents, and writes the output to a new file named `decoded`.
- `gurl -d -i encoded -o decoded`
