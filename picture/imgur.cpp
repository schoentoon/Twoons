#include "imgur.h"

bool pImgur::checkLink(QUrl url) {
  return (url.host() == "imgur.com" and !url.path().startsWith("/a/"));
}

void pImgur::openImage(QUrl url)
{
  QString id = url.path().remove("gallery",Qt::CaseInsensitive).remove("/");
  emit openPicture(QUrl("http://i.imgur.com/" + id + ".png"));
}
