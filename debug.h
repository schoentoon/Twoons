#ifndef DEBUG_H
#define DEBUG_H

#include "CString.h"
#include <QDateTime>

#include <iostream>

using namespace std;

#define DEBUG(f) do { \
  if (Debug::isDebug()) { \
    cout << "[" << Debug::Timestamp() << "] - [ " << f << " ]" << endl; \
  } \
} while (0)

class Debug {
public:
  static void SetDebug(bool b) { debug = b; }
  static bool isDebug() { return debug; }
  static std::string Timestamp() {
     return QDateTime::currentDateTime().toString("dd-MM-yy hh:mm:ss:zzz").toStdString();
  }
protected:
  static bool debug;
};

#endif //DEBUG_H