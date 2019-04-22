#ifndef IMGURUP_H
#define IMGURUP_H

#include "upframework.h"
#include "CString.h"
#include "global.h"
#include "jsonxx.h"

class uImgur : public PictureUpFramework
{
public:
  QNetworkReply* upload(QFile *file);
  unsigned int getUniqueId();
  QString getName();
  void postUpload(QNetworkReply *reply);
};

#endif // IMGUR_H
