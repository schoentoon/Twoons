#ifndef FLICKR_H
#define FLICKR_H

#include "framework.h"
#include "CString.h"
#include "jsonxx.h"
#include <sstream>

using namespace jsonxx;

class pFlickr : public PictureFramework
{
  Q_OBJECT
public:
  pFlickr();
  ~pFlickr();
  bool checkLink(QUrl url);
  void openImage(QUrl url);
private slots:
  void finished(QNetworkReply*);
private:
  QNetworkAccessManager *network;
  bool decode(const char* encoded, unsigned long* output);
  int decode_char(unsigned char ch);
};

#endif
