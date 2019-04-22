#include "framework.h"
#include "moc_framework.h"

/* All the picture services */
#include "picdotwitter.h"
#include "instagram.h"
#include "lockerz.h"
#include "twitpic.h"
#include "flickr.h"
#include "picplz.h"
#include "mobyto.h"
#include "yfrog.h"
#include "imgur.h"
#include "imgly.h"

vector<PictureFramework*> PictureFramework::init_pictures(Ui_ImageView *image)
{
  vector<PictureFramework*> tmp;
  tmp.push_back(new pTwitpic);
  tmp.push_back(new pYfrog);
  tmp.push_back(new pImgur);
  tmp.push_back(new pInstagram);
  tmp.push_back(new pPictwitter);
  tmp.push_back(new pLockerz);
  tmp.push_back(new pPicPlz);
  tmp.push_back(new pImgLy);
  tmp.push_back(new pFlickr);
  tmp.push_back(new pMobyTo);
  for (unsigned int i = 0; i < tmp.size(); i++) {
    QObject::connect(tmp[i], SIGNAL(openPicture(QUrl))
                    ,image, SLOT(openImage(QUrl)));
    QObject::connect(tmp[i], SIGNAL(showError(QString))
                    ,image, SLOT(showError(QString)));
  }
  return tmp;
}

bool PictureFramework::checkLink(QUrl)
{
  return false;
}

void PictureFramework::openImage(QUrl)
{
}
