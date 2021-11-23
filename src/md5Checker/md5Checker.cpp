
/*!
 * \file      md5Checker.cpp
 * \brief     md5Checker implement
 * \copyright Copyright 2021. All Rights Reserved.
 */

// C header
#include <stdint.h>
#include <termios.h>

// C++ header
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <thread>
#include <map>
#include <vector>

// User header
#include "md5Checker.h"

namespace fs = std::filesystem;

namespace cbm {

struct md5checker::Impl {
  string rootpath;
  string resultpath;
  bool interrupt;
  termios current;
  termios old;
  map<string, vector<string>> md5_saver;

  Impl() : interrupt(false) {
    rootpath.clear();
    tcgetattr(0, &old);
    current = old;
    current.c_lflag |= ICANON;
    current.c_lflag &= ~ECHO;
    tcsetattr(0, TCSANOW, &current);
  }

  ~Impl() {
    tcsetattr(0, TCSANOW, &old);
  }
  
  void GetMD5(string filepath) {
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

    printf("\33[u\n\33[J\r\tProcess.... %s\n", filepath.c_str());

    FILE *stream = popen(header.c_str(), "r");

    char md5[33] = {0, };
    fread(md5, 1, 32, stream);

    vector<string> vtemp;
    vtemp.push_back(filepath);

    auto ret = md5_saver.insert(make_pair(md5, vtemp));

    if (ret.second == false) {
      auto &temp = md5_saver[md5];
      temp.push_back(filepath);
    }

    pclose(stream);
  }

  void ExplorDirectory(string path = "") {
    fs::path p;

    if (path.empty())
      p = this->rootpath;
    else
      p = path;

    fs::directory_iterator start(p);
    fs::directory_iterator end;

    int count = 0;
    for (fs::directory_iterator itr = start ; itr != end ; itr++, count++) {
      if (itr->is_directory())
        ExplorDirectory(itr->path());
      else if(itr->is_regular_file())
        GetMD5(itr->path());

      if (this->interrupt)
        break;
    }
  }

  static void KeyInterrupt(void *instance) {
    auto self = reinterpret_cast<md5checker::Impl *>(instance);

    do {
      int input = getchar();

      if (input == 'q' || input == 'Q') {
        self->interrupt = true;
        break;
      }
    } while(1);
  }
};

md5checker::md5checker() : impl_(new md5checker::Impl()) {}

md5checker::md5checker(const string& path) : impl_(new md5checker::Impl()) {
  if (!path.empty()) {
    if (path[0] == '.')
      throw invalid_argument("SYNTAX ERROR: Wrong input path format. Please type full path");

    impl_->rootpath = path;
  }
}

md5checker::~md5checker() {
  impl_->rootpath.clear();
}

void md5checker::SetPath(const string& path) {
  if (!path.empty()) {
    if (path[0] == '.')
      throw invalid_argument("SYNTAX ERROR: Wrong input path format. Please type full path");

    impl_->rootpath = path;
  }
}

void md5checker::StartCheck() {
  system("clear");

  if (!impl_->rootpath.empty()) {
    printf("\t[[[ MD5 Checker ]]]\n ");
    printf("\tIf you want to stop working, type (q) and press enter... \n\33[s");

    thread press(md5checker::Impl::KeyInterrupt, impl_.get());
    press.detach();

    impl_->ExplorDirectory();
  }
  else {
    throw invalid_argument("ERROR: Not set path.");
  }
}

void md5checker::SaveResult(const string& path) {
  if (path.empty())
    throw invalid_argument("ERROR: The path to save is invalid");
  else {
    impl_->resultpath = path;
    FILE *fp = fopen(path.c_str(), "wt");

    if (fp == NULL)
      throw invalid_argument("ERROR: The path to save is invalid");
    else {
      char spin[] = {'-', '/', '|', '\\'};

      uint64_t count = 1;
      for (auto itr = impl_->md5_saver.begin(); itr != impl_->md5_saver.end() ; itr++) {
        printf("\33[u\n\33[J\r\tPlease wait for saving.... %c", spin[count%4]);

        if (itr->second.size() > 1) {
          fprintf(fp, "%llu. [%s]\n", count++, itr->first.c_str());

          for (auto seconditr  = itr->second.begin(); seconditr != itr->second.end() ; seconditr++)
            fprintf(fp, "%s\n", seconditr->c_str());
        }
      }

      printf("\n");
    }

    fclose(fp);
  }
}

void md5checker::DeleteDuplicateFiles(bool execute) {
  if (execute) {
    char spin[] = {'-', '/', '|', '\\'};
    string removeresult = impl_->resultpath;
    removeresult.append("_remove");
    FILE *fp_r = fopen(removeresult.c_str(), "wb");
    removeresult.append("_command");
    FILE *fp_rc = fopen(removeresult.c_str(), "wb");

    for (auto itr = impl_->md5_saver.begin(); itr != impl_->md5_saver.end() ; itr++) {
      if ((!itr->first.empty()) && (itr->second.size() > 1)) {
        auto seconditr  = itr->second.begin();

        uint64_t spincnt = 0;
        for (seconditr++ ; seconditr != itr->second.end() ; seconditr++) {
          printf("\33[u\n\33[J\r\tPlease wait for removing.... %c", spin[spincnt++%4]);
          string header = "rm -rf ";
          
          header.append("\"");
          header.append(seconditr->c_str());
          header.append("\"");
          fprintf(fp_r, "%s\n", seconditr->c_str());
          fprintf(fp_rc, "%s\n", header.c_str());
        }
      }
    }

    fclose(fp_r);
    fclose(fp_rc);
  }
}

} // namespace cbm