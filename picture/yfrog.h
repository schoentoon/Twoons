#ifndef YFROG_H
#define YFROG_H

#include "framework.h"

class pYfrog : public PictureFramework
{
  bool checkLink(QUrl url);
  void openImage(QUrl url);
};

#endif
