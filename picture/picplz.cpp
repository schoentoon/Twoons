#include "picplz.h"
#include "moc_picplz.h"

pPicPlz::pPicPlz()
{
  network = new QNetworkAccessManager();
  QObject::connect(network, SIGNAL(finished(QNetworkReply*))
                  ,this, SLOT(finished(QNetworkReply*)));
}

bool pPicPlz::checkLink(QUrl url) {
  return (url.host().endsWith("picplz.com"));
}

void pPicPlz::openImage(QUrl url)
{
  network->get(QNetworkRequest(QUrl("http://api.picplz.com/api/v2/pic.json?shorturl_id=" + url.path().remove("/"))));
}

void pPicPlz::finished(QNetworkReply *reply)
{
  reply->deleteLater();
  Object o;
  istringstream input(reply->readAll().data());
  if (Object::parse(input, o)) {
    if (o.has<Object>("value")) {
      if (o.get<Object>("value").has<Array>("pics")) {
        if (o.get<Object>("value").get<Array>("pics").has<Object>(0)) {
          if (o.get<Object>("value").get<Array>("pics").get<Object>(0).has<Object>("pic_files")) {
            if (o.get<Object>("value").get<Array>("pics").get<Object>(0).get<Object>("pic_files").has<Object>("640r")) {
              if (o.get<Object>("value").get<Array>("pics").get<Object>(0).get<Object>("pic_files").get<Object>("640r").has<string>("img_url"))
                emit openPicture(QUrl(o.get<Object>("value").get<Array>("pics").get<Object>(0).get<Object>("pic_files").get<Object>("640r").get<string>("img_url").c_str()));
            }
          }
        }
      }
    }
  } else
    emit showError("You are not allowed to view this picture.");
}
