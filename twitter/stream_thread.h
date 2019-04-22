#ifndef STREAM_THREAD_H
#define STREAM_THREAD_H

#include "CString.h"
#include "OAuth.h"
#include "jsonxx.h"
#include <time.h>
/* Qt includes */
#include <QObject>
#include <QString>
#include <QApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QByteArray>
#include <QTimer>
#include <QSslError>

using namespace jsonxx;

class StreamThread: public QObject
{
  Q_OBJECT
public:
  StreamThread(bool replies = false);
  void start();
  void stop();
  bool ifReplies() { return replies; };
signals:
  void sendData(CString);
  void needBackfill();
private slots:
  void newData();
  void checkConnection();
  void closeConnection();
public slots:
  void setReplies(bool);
  void onRequestCompleted(QNetworkReply*);
private:
  QNetworkReply *stream;
  QNetworkAccessManager *network;
  QTimer *timer;
  unsigned int counter;
  bool replies;
};

#endif // STREAM_THREAD_H
