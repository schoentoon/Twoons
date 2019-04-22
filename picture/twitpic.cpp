#include "twitpic.h"

bool pTwitpic::checkLink(QUrl url) {
  return (url.host() == "twitpic.com" and !url.path().contains("photos"));
}

void pTwitpic::openImage(QUrl url)
{
  QString id = url.path().remove("/full").remove("/");
  emit openPicture(QUrl("http://twitpic.com/show/full/" + url.path()));
}
