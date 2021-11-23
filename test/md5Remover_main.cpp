
/*!
 * \file      md5REmoverer_main.cpp
 * \brief     md5REmoverer Test Application.
 * \copyright Copyright 2021. All Rights Reserved.
 */

// C++ Header
#include <stdexcept>

// User Header
#include "md5Remover.h"

using namespace std;

int main(int argc, char *argv[]) {
  if (argc == 2) {
    try {
      cbm::md5remover checker(argv[1]);
      checker.StartRemove();

      printf("\tFINISH MD5 REMOVER!!!\n");
    }
    catch (invalid_argument& e) {
      printf("%s\n", e.what());
      printf("USAGE: md5remover [root directory path to compare md5] [result file path to save]\n");
    }
  }
  else {
    printf("USAGE: md5remover [root directory path to compare md5] [result file path to save]\n");
  }

  return 0;
}
