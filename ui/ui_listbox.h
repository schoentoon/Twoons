#ifndef UI_LISTBOX_H
#define UI_LISTBOX_H

#include "list.h"
/* Qt includes */
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QStatusBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QApplication>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <iostream>

using namespace std;
/**
 * @brief ListBox class
 * @section description This class is used to display a list
 */
class ListBox: public QGroupBox
{
  Q_OBJECT
public:
  /**
    * @brief Constructor
    * @param list The list to parse for this listbox
    */
  ListBox(TwList list);
  /**
   * @return The TwList object
   */
  TwList getList() { return pList; };
protected:
  QVBoxLayout *verticalLayout;
  QLabel *description, *extraText;
  TwList pList;
};

#endif // UI_LISTBOX_H