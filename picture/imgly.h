#ifndef IMGLY_H
#define IMGLY_H

#include "framework.h"

class pImgLy : public PictureFramework
{
  bool checkLink(QUrl url);
  void openImage(QUrl url);
};

#endif
