#ifndef LOCKERZ_H
#define LOCKERZ_H

#include "framework.h"

class pLockerz : public PictureFramework
{
  bool checkLink(QUrl url);
  void openImage(QUrl url);
};

#endif
