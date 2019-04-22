#include "yfrog.h"

bool pYfrog::checkLink(QUrl url) {
  if (!url.host().startsWith("yfrog."))
    return false;
  if (url.path().endsWith("j",Qt::CaseInsensitive)
   or url.path().endsWith("p",Qt::CaseInsensitive)
   or url.path().endsWith("b",Qt::CaseInsensitive)
   or url.path().endsWith("t",Qt::CaseInsensitive)
   or url.path().endsWith("g",Qt::CaseInsensitive))
    return true;
  return false;
}

void pYfrog::openImage(QUrl url)
{
  emit openPicture(QUrl(url.toString() + ":medium"));
}
