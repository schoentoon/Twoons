#ifndef UI_AUTOCOMPLETE_H
#define UI_AUTOCOMPLETE_H

#include "status.h"
#include "datastorage.h"
/* Qt includes */
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QWidget>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>
#include <QKeyEvent>
#include <iostream>

using namespace std;
/** @brief Auto Complete class
 * @section description This class is used for the autocompletion of user names */
class Ui_Autocomplete: public QGroupBox
{
  Q_OBJECT
public:
  /** @brief Constructor */
  Ui_Autocomplete();
  bool eventFilter(QObject *, QEvent *);
protected:
  void mousePressEvent(QMouseEvent *event) { dragStart = event->pos(); };
  void mouseMoveEvent(QMouseEvent *event) { setGeometry(QRect(geometry().topLeft() + event->pos() - dragStart, rect().size())); };
  QPoint dragStart;
  QGridLayout *layout;
  QLabel *header;
  QLineEdit *inputBox;
  QListWidget *listWidget;
  QPushButton *closeButton;
  dataStorage* store;
public slots:
  void appear();
protected slots:
  void query(QString);
  void activatedItem(QModelIndex);
signals:
  void sendNickname(CString);
  void focus();
};

#endif // UI_AUTOCOMPLETE_H
