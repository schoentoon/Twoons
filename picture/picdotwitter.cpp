#include "picdotwitter.h"
#include "moc_picdotwitter.h"
#include "global.h"

pPictwitter::pPictwitter()
{
  network = new QNetworkAccessManager();
  QObject::connect(network, SIGNAL(finished(QNetworkReply*))
                  ,this, SLOT(finished(QNetworkReply*)));
}

bool pPictwitter::checkLink(QUrl url) {
  if (url.hasQueryItem("media_url"))
    return true;
  if (url.host().endsWith("twitter.com") and url.toString().contains("photo"))
    return true;
  return false;
}

void pPictwitter::openImage(QUrl url)
{
  if (url.hasQueryItem("media_url"))
    emit openPicture(url.queryItemValue("media_url"));
  else
    network->get(Global::dataStore()->getOAuth()->APICall(CString("http://api.twitter.com/1.1/statuses/show/"
                 + url.toString().remove(0,url.toString().indexOf("/status/")+8).split("/").at(0).toStdString()
                 + ".json?include_entities=t")));
}

void pPictwitter::finished(QNetworkReply *reply)
{
  reply->deleteLater();
  Object o;
  istringstream input(reply->readAll().data());
  if (Object::parse(input, o)) {
    if (o.has<Object>("entities")) {
      if (o.get<Object>("entities").has<Array>("media")) {
        if (o.get<Object>("entities").get<Array>("media").has<Object>(0)) {
          if (o.get<Object>("entities").get<Array>("media").get<Object>(0).has<string>("media_url"))
            emit openPicture(QUrl(o.get<Object>("entities").get<Array>("media").get<Object>(0).get<string>("media_url").c_str()));
        }
      }
    }
  } else
    emit showError("You are not allowed to view this picture.");
}
