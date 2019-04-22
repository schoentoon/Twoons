#ifndef IMGUR_H
#define IMGUR_H

#include "framework.h"

class pImgur : public PictureFramework
{
  bool checkLink(QUrl url);
  void openImage(QUrl url);
};

#endif
