
/*!
 * \file      md5Checker_main.cpp
 * \brief     md5Checker Test Application.
 * \copyright Copyright 2021. All Rights Reserved.
 */

#include "md5Checker.h"

using namespace std;

int main(int argc, char *argv[]) {
  if (argc == 3) {
    try {
      cbm::md5checker checker(argv[1]);

      checker.StartCheck();
      checker.SaveResult(argv[2]);

      printf("\tFINISH MD5 CHECKER!!!\n");
    }
    catch (invalid_argument& e) {
      printf("%s\n", e.what());
      printf("USAGE: md5checker [root directory path to compare md5] [result file path to save]\n");
    }
  }
  else {
    printf("USAGE: md5checker [root directory path to compare md5] [result file path to save]\n");
  }

  return 0;
}
