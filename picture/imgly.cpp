#include "imgly.h"

bool pImgLy::checkLink(QUrl url) {
  return (url.host() == "img.ly" and url.path().count("/") < 2);
}

void pImgLy::openImage(QUrl url)
{
  emit openPicture(QUrl("http://img.ly/show/full" + url.path()));
}
