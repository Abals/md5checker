**Table of Contents**  *generated with Abals*

- [MD5Checker (Message-Digest algorithm 5 Checker)](#md5checker-message-digest-algorithm-5-checker)
- [How to build](#how-to-build)
  - [Edit build.sh](#edit-buildsh)
- [Execute build.sh](#execute-buildsh)
- [Output path](#output-path)
- [USAGE](#usage)

# MD5Checker (Message-Digest algorithm 5 Checker)
MD5 (Message-Digest algorithm 5) is a 128-bit cryptographic hash function. It is designated as RFC 1321, and is mainly used for integrity checking to verify that a program or file is original.  
A program that extracts MD5 of a file is basically installed for each OS.  
MocOS : `md5 <name of file>`  
Linux : `md5sum <name of file>`  
**Without the above example programs, md5 and md5sum, this md5checker will not run.**  
_**md5checker** is an application that checks whether there are duplicate files through md5 comparison while searching inside setted folder._


# How to build
This project is built using cmake.  
Therefore, cmake (higher version 3) must be installed.

## Edit build.sh
Set the appropriate compiler in the compiler setting section.
```bash
# build.sh
line 3. PROJ_C_COMPILER=/usr/bin/gcc
line 4. PROJ_CPP_COMPILER=/usr/bin/g++
```

# Execute build.sh
```bash
$ bash ./build.sh
```

# Output path
`/<respository>/md5checker/build/bin/${OPERATING_SYSTEM}/${BUILD_TYPE}`

# USAGE
```bash
$ ./md5checker [the path to the directory to be check] [the file name to save the results]
```