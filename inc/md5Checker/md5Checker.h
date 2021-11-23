
/*!
 * \file      md5Checker.h
 * \brief     md5Checker class
 * \copyright Copyright 2021. All Rights Reserved.
 */

#ifndef __MD5CHECKER_H__
#define __MD5CHECKER_H__

#include <memory>
#include <string>

using namespace std;

namespace cbm {

class md5checker {
public:
  explicit md5checker();
  explicit md5checker(const string&);
  md5checker(const md5checker&) = delete;

  ~md5checker();

  void SetPath(const string& path);
  void StartCheck();
  void SaveResult(const string& path);
  void DeleteDuplicateFiles(bool);

private:
  struct Impl;
  unique_ptr<Impl> impl_;
};

} // namespace cbm

#endif // __MD5CHECKER_H__