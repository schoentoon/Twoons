#include "mobyto.h"

bool pMobyTo::checkLink(QUrl url) {
  return (url.host() == "moby.to");
}

void pMobyTo::openImage(QUrl url)
{
  emit openPicture(QUrl(url.toString() + ":full"));
}
