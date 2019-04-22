#ifndef TWITPICUP_H
#define TWITPICUP_H

#include "upframework.h"
#include "CString.h"
#include "global.h"
#include "jsonxx.h"

class uTwitPic : public PictureUpFramework
{
public:
  QNetworkReply* upload(QFile *file);
  unsigned int getUniqueId();
  QString getName();
  void postUpload(QNetworkReply *reply);
};

#endif // TWITPICUP_H
