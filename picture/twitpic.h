#ifndef TWITPIC_H
#define TWITPIC_H

#include "framework.h"

class pTwitpic : public PictureFramework
{
  bool checkLink(QUrl url);
  void openImage(QUrl url);
};

#endif
