
/*!
 * \file      md5Remover.cpp
 * \brief     md5Remover implement
 * \copyright Copyright 2021. All Rights Reserved.
 */

// C header
#include <stdint.h>
#include <termios.h>

// C++ header
#include <cstring>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <thread>
#include <map>
#include <vector>

// User header
#include "md5Remover.h"

namespace fs = std::filesystem;

namespace cbm {

struct md5remover::Impl {
  string filepath;
  bool interrupt;
  termios current;
  termios old;
  map<string, vector<string>> md5_saver;

  Impl() : interrupt(false) {
    filepath.clear();
    tcgetattr(0, &old);
    current = old;
    current.c_lflag |= ICANON;
    current.c_lflag &= ~ECHO;
    tcsetattr(0, TCSANOW, &current);
  }

  ~Impl() {
    tcsetattr(0, TCSANOW, &old);
  }
  
  int equalMD5(char *filepath, char *orimd5) {
    if ((filepath != nullptr) && (filepath[0] != '\0')) {
      #if _MD5_BUILD_SYSTEM==1
      string header = "md5sum "; // for UNIX
      #elif _MD5_BUILD_SYSTEM==2
      string header = "md5 "; // for MAC
      #endif
      
      header.append("\"");
      header.append(filepath);
      header.append("\"");

      #if _MD5_BUILD_SYSTEM==2
      header.append(" | awk '{print $4}'"); // for MAC
      #endif

      FILE *stream = popen(header.c_str(), "r");

      char md5[33] = {0, };
      fread(md5, 1, 32, stream);

      pclose(stream);

      return strcmp(md5, orimd5);
    }
    else
      return 1;
  }

  bool isMD5(const char *str, char *outmd5) {
    bool ismd5 = false;

    if ((str != nullptr) && (str[0] != '\0')) {
      char md5[10240] = {0, };
      bool hasdot = false;
      bool hasLSB = false; // Left Square Brackets
      bool hasRSB = false;

      int idx = 0, md5idx = 0;
      for (idx = 0 ; str[idx] != '\n' ; idx++) {
        if (hasLSB)
          md5[md5idx++] = str[idx];

        switch(str[idx]) {
        case '.':
          hasdot = true;
          break;
        case '[':
          if (hasdot)
            hasLSB = true;

          break;
        case ']':
          if (hasLSB)
            hasRSB = true;

          break;
        default:
          break;
        }

        if (hasRSB) {
          md5[md5idx-1] = '\0';
          strcpy(outmd5, md5);
          ismd5 = true;
          break;
        }
      }
    }

    return ismd5;
  }

  void AnalyzeFile() {
    FILE *fp = fopen(this->filepath.c_str(), "rb");

    if (fp != nullptr) {
      while (!feof(fp)) {
        char temp[10240] = {0, };
        char md5[10240] = {0, };
        long begin = 0, end = 0;

        do {
          if (isMD5(temp, md5))
            fgets(temp, sizeof(temp)/sizeof(char), fp); // Just passing
          else {
            //if (equalMD5(temp, md5) == 0) {
              printf("%s", temp);
            //}
          }

          fgets(temp, sizeof(temp)/sizeof(char), fp);
        } while(!isMD5(temp, md5) && !feof(fp));
      }

      fclose(fp);
    }
    else
      throw invalid_argument("SYSTEM ERROR: Can't file open");
  }

  static void KeyInterrupt(void *instance) {
    auto self = reinterpret_cast<md5remover::Impl *>(instance);

    do {
      int input = getchar();

      if (input == 'q' || input == 'Q') {
        self->interrupt = true;
        break;
      }
    } while(1);
  }
};

md5remover::md5remover() : impl_(new md5remover::Impl()) {}

md5remover::md5remover(const string& path) : impl_(new md5remover::Impl()) {
  if (!path.empty())
    impl_->filepath = path;
  else
    throw invalid_argument("SYNTAX ERROR: Wrong input path format. Please type full path");
}

md5remover::~md5remover() {
  impl_->filepath.clear();
}

void md5remover::SetPath(const string& path) {
  if (!path.empty()) {
    if (path[0] == '.')
      throw invalid_argument("SYNTAX ERROR: Wrong input path format. Please type full path");

    impl_->filepath = path;
  }
}

void md5remover::StartRemove() {
  system("clear");

  if (!impl_->filepath.empty()) {
    printf("\t[[[ MD5 Remover ]]]\n ");
    printf("\tIf you want to stop working, type (q) and press enter... \n\33[s");

    thread press(md5remover::Impl::KeyInterrupt, impl_.get());
    press.detach();

    impl_->AnalyzeFile();
  }
  else {
    throw invalid_argument("ERROR: Not set path.");
  }
}

} // namespace cbm