
/*!
 * \file      md5Remover.h
 * \brief     md5Remover class
 * \copyright Copyright 2021. All Rights Reserved.
 */

#ifndef __MD5REMOVER_H__
#define __MD5REMOVER_H__

#include <memory>
#include <string>

using namespace std;

namespace cbm {

class md5remover {
public:
  explicit md5remover();
  explicit md5remover(const string&);
  md5remover(const md5remover&) = delete;

  ~md5remover();

  void SetPath(const string& path);

  void StartRemove();

private:
  struct Impl;
  unique_ptr<Impl> impl_;
};

} // namespace cbm

#endif // __MD5REMOVER_H__