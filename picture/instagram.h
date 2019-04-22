#ifndef INSTAGRAM_H
#define INSTAGRAM_H

#include "framework.h"

class pInstagram : public PictureFramework
{
  bool checkLink(QUrl url);
  void openImage(QUrl url);
};

#endif
