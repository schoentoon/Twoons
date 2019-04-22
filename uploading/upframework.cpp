#include "upframework.h"
#include "moc_upframework.h"

/* Uploading service includes */
#include "uploading/twitpic.h"
#include "uploading/imgurd.h"
#include "uploading/imgur.h"
#include "uploading/imgly.h"

PictureUpFramework::PictureUpFramework()
{
  network = new QNetworkAccessManager();
  QObject::connect(network, SIGNAL(finished(QNetworkReply*))
                  ,this, SLOT(finished(QNetworkReply*)));
}

void PictureUpFramework::finished(QNetworkReply *reply)
{
  reply->deleteLater();
  postUpload(reply);
}

void PictureUpFramework::postUpload(QNetworkReply*)
{
}

QString PictureUpFramework::url()
{
  return "";
}

QNetworkReply* PictureUpFramework::upload(QFile*)
{
  return 0; /* WATCH OUT HERE! */
}

unsigned int PictureUpFramework::getUniqueId()
{
  return 0;
}

QString PictureUpFramework::getName()
{
  return "";
}

vector<PictureUpFramework*> PictureUpFramework::init(TweetField *field)
{
  vector<PictureUpFramework*> tmp;
  tmp.push_back(new uTwitPic);
  tmp.push_back(new uImgur);
  tmp.push_back(new uImgurD);
  tmp.push_back(new uImgLy);
  if (field == 0)
    return tmp;
  for (unsigned int i = 0; i < tmp.size(); i++)
    QObject::connect(tmp[i], SIGNAL(sendUrl(QString))
                    ,field, SLOT(appendUrl(QString)));
  return tmp;
}
