#ifndef GLOBAL_H
#define GLOBAL_H

#include "datastorage.h"
#include "ui_mainwindow.h"

class Global {
public:
  static Ui_MainWindow* Get();
  static dataStorage* dataStore();
};

#endif //GLOBAL_H
