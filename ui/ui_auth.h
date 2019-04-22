#ifndef UI_AUTH_H
#define UI_AUTH_H

#include "OAuth.h"
#include "OAuthUtils.h"
#include "CString.h"
#include "datastorage.h"
#include "ui_errorbox.h"
/* Qt includes */
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QWidget>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>
#include <QtGui/QDialog>
#include <QNetworkReply>
#include <QKeyEvent>
#include <iostream>

using namespace std;
/** @brief The gui used to authorize
 * @section description This class is a part of the autorization */
class Ui_Auth: public QGroupBox
{
  Q_OBJECT
public:
  Ui_Auth();
  bool eventFilter(QObject * object, QEvent * event);
protected:
  OAuth *user;
  CString sPin;
  QGridLayout *layout;
  QLabel *url, *errorBox;
  QTextEdit *pin;
  QPushButton *activate;
  QNetworkReply *networkHandle;
signals:
  void sendKeys(CString,CString);
public slots:
  void onRequestCompleted(QNetworkReply *reply);
};

#endif // UI_AUTOCOMPLETE_H
