#ifndef PICDOTTWITTER_H
#define PICDOTTWITTER_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <sstream>
#include "framework.h"
#include "jsonxx.h"

using namespace jsonxx;
using namespace std;

class pPictwitter: public PictureFramework
{
  Q_OBJECT
public:
  pPictwitter();
  bool checkLink(QUrl url);
  void openImage(QUrl url);
private:
  QNetworkAccessManager *network;
private slots:
  void finished(QNetworkReply*);
};

#endif
