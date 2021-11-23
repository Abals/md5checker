
/*!
 * \file      md5Checker_main.cpp
 * \brief     md5Checker Test Application.
 * \copyright Copyright 2021. All Rights Reserved.
 */

// C++ Header
#include <stdexcept>

// User Header
#include "md5Checker.h"

void printUsage(void) {
  printf("USAGE: md5checker [--option] [root directory path to compare md5] [result file path to save]\n");
  printf("\t Option List\n\n");
  printf("\t --delete, -d\t\t Delete duplicate files after md5 check\n");
}

using namespace std;

int main(int argc, char *argv[]) {
  bool deleteFlag = false;
  cbm::md5checker checker;

  if (argc > 1) {
    try {
      for (int idx = 1 ; idx < argc ; idx++) {
        switch (argv[idx][0]) {
        case '-':
          if (argv[idx][1] == '-') {
            if (argv[idx][2] == 'd')
              deleteFlag = true;
          }
          else if (argv[idx][1] == 'd')
            deleteFlag = true;

          break;
        case '/':
          checker.SetPath(argv[idx]);

          checker.StartCheck();
          checker.SaveResult(argv[idx+1]);
          idx++;

          break;
        default:
          printUsage();
          break;
        }
      }

      checker.DeleteDuplicateFiles(deleteFlag);

      printf("\n\n\tFINISH MD5 CHECKER!!!\n");
    }
    catch (invalid_argument& e) {
      printf("%s\n", e.what());
      printUsage();
    }
  }
  else
    printUsage();

  return 0;
}
