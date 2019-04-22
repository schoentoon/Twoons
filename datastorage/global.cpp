#include "global.h"

Ui_MainWindow* Global::Get()
{
  static Ui_MainWindow* CMain = new Ui_MainWindow();
  return CMain;
}

dataStorage* Global::dataStore()
{
  static dataStorage* storage = new dataStorage();
  return storage;
}
