#ifndef IMGURUPD_H
#define IMGURUPD_H

#include "upframework.h"
#include "CString.h"
#include "global.h"
#include "jsonxx.h"

class uImgurD : public PictureUpFramework
{
public:
  QNetworkReply* upload(QFile *file);
  unsigned int getUniqueId();
  QString getName();
  void postUpload(QNetworkReply *reply);
};

#endif // IMGURD_H
