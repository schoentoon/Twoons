#ifndef MOBYTO_H
#define MOBYTO_H

#include "framework.h"

class pMobyTo : public PictureFramework
{
  bool checkLink(QUrl url);
  void openImage(QUrl url);
};

#endif
