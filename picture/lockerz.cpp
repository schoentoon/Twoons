#include "lockerz.h"

bool pLockerz::checkLink(QUrl url) {
  if (url.host() == "lockerz.com" and url.path().startsWith("/s/"))
    return true;
  return false;
}

void pLockerz::openImage(QUrl url)
{
  QUrl tmp("http://api.plixi.com/api/tpapi.svc/imagefromurl");
  tmp.addQueryItem("size","big");
  tmp.addQueryItem("url",url.toString());
  emit openPicture(tmp);
}
