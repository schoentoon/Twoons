#ifndef PICPLZ_H
#define PICPLZ_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <sstream>
#include "framework.h"
#include "jsonxx.h"

using namespace jsonxx;
using namespace std;

class pPicPlz: public PictureFramework
{
  Q_OBJECT
public:
  pPicPlz();
  bool checkLink(QUrl url);
  void openImage(QUrl url);
private:
  QNetworkAccessManager *network;
private slots:
  void finished(QNetworkReply*);
};

#endif
