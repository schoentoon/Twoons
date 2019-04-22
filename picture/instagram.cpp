#include "instagram.h"

bool pInstagram::checkLink(QUrl url) {
  return ((url.host() == "instagram.com" or url.host() == "instagr.am") and url.path().startsWith("/p"));
}

void pInstagram::openImage(QUrl url)
{
  QString id = url.path().remove("/p/").remove("/");
  emit openPicture(QUrl("http://instagram.com/p/" + id + "/media?size=l"));
}
