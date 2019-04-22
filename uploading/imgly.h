#ifndef IMGLYUP_H
#define IMGLYUP_H

#include "upframework.h"
#include "CString.h"
#include "global.h"
#include "jsonxx.h"

class uImgLy : public PictureUpFramework
{
public:
  QNetworkReply* upload(QFile *file);
  unsigned int getUniqueId();
  QString getName();
  void postUpload(QNetworkReply *reply);
};

#endif // IMGLYUP_H
