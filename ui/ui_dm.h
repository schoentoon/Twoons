#ifndef UI_DM_H
#define UI_DM_H

#include "CString.h"
#include "datastorage.h"
#include "jsonxx.h"
#include "ui_dmuser.h"
#include "ui_dmview.h"
/* Qt includes */
#include <QNetworkAccessManager>
#include <QDesktopServices>
#include <QPlainTextEdit>
#include <QTextBrowser>
#include <QPixmapCache>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDateTime>
#include <QGroupBox>
#include <QLabel>
#include <QTimer>
#include <vector>

using namespace std;
using namespace jsonxx;
/** @brief The gui for direct messages */
class Ui_DirectMessage: public QWidget
{
  Q_OBJECT
public:
  /** @brief Constructor */
  Ui_DirectMessage();
  bool eventFilter(QObject*,QEvent*);
  void receiveData(Object&);
  void needBackfill();
  void sendDM(CString msg);
  void openDM(CString screen_name, unsigned int user_id);
protected:
  void mousePressEvent(QMouseEvent *event) { dragStart = event->pos(); raise(); };
  void mouseMoveEvent(QMouseEvent *event) { setGeometry(QRect(geometry().topLeft() + event->pos() - dragStart, rect().size())); };
  QPoint dragStart;
  QScrollArea *scrollArea;
  QWidget     *scrollAreaWidgetContents;
  QVBoxLayout *scrollLayout;
  QGridLayout *mainLayout;
  QScrollArea *scrollDm;
  QWidget     *scrollDmWidget;
  QVBoxLayout *dmLayout;
  QNetworkReply *sent, *received;
  QTimer *timer;
  QNetworkAccessManager *network;
  QNetworkReply *sending;
  QPlainTextEdit *dmField;
  QPushButton *closeButton;
  QLabel *characterCounter;
  vector<DmUser*> people;
  unsigned int user;
  double last_id;
public slots:
  void getDM(DM);
  void clearDMs(unsigned int);
private slots:
  void makeRequests();
  void onRequestCompleted(QNetworkReply*);
  void textChanged();
  void deleteDM(double);
signals:
  void notifyDM();
};

#endif // UI_SETTINGS_H
