#include "flickr.h"
#include "moc_flickr.h"

#ifndef FLICKR_KEY
#error "You should go and define a FLICKR_KEY"
#endif

pFlickr::pFlickr()
{
  network = new QNetworkAccessManager();
  QObject::connect(network, SIGNAL(finished(QNetworkReply*))
                  ,this, SLOT(finished(QNetworkReply*)));
}

pFlickr::~pFlickr()
{
  delete network;
}

bool pFlickr::checkLink(QUrl url) {
  return ((url.host().endsWith("flic.kr") and url.path().startsWith("/p/"))
    or (url.host().endsWith("flickr.com") and url.path().startsWith("/photos/")));
}

void pFlickr::openImage(QUrl url)
{
  QString id = "";
  if (url.host().endsWith("flickr.com")) {
    QString tmp = url.path();
    if (tmp.endsWith("/"))
      tmp.chop(1);
    QStringList split = tmp.split("/");
    bool ok = false;
    for (unsigned int i = 0; i < (unsigned) split.size(); i++) {
      split.at(i).toULong(&ok);
      if (ok) {
        id = split.at(i);
        break;
      }
    }
  } else if (url.host().endsWith("flic.kr")) {
    QString tmp = url.path();
    tmp.replace("/p/","");
    unsigned long longId = 0;
    if (decode(tmp.toAscii().constData(), &longId))
      id = QString::number(longId);
    else
      return;
  }
  if (id.isEmpty())
    return;
  QUrl api("http://api.flickr.com/services/rest/");
  api.addQueryItem("method","flickr.photos.getSizes");
  api.addQueryItem("photo_id",id);
  api.addQueryItem("api_key",FLICKR_KEY);
  api.addQueryItem("format","json");
  network->get(QNetworkRequest(api));
}

void pFlickr::finished(QNetworkReply *reply)
{
  reply->deleteLater();
  QByteArray json = reply->readAll();
  json.replace("jsonFlickrApi(","");
  if (json.endsWith(")"))
    json.remove(json.size() - 1, 1);
  Object o;
  istringstream input(json.data());
  if (Object::parse(input, o)) {
    if (o.has<Object>("sizes")) {
      if (o.get<Object>("sizes").has<Array>("size")) {
        Array &a = o.get<Object>("sizes").get<Array>("size");
        for (unsigned int i = 0; i < a.size(); i++) {
          if (a.has<Object>(i)) {
            if (a.get<Object>(i).has<string>("label")) {
              if (CString(a.get<Object>(i).get<string>("label")).Equals("large")) {
                if (a.get<Object>(i).has<string>("source")) {
                  emit openPicture(QUrl(a.get<Object>(i).get<string>("source").c_str()));
                  break;
                }
              }
            }
          }
        }
      }
    }
  }
}

int pFlickr::decode_char(unsigned char ch)
{
  if (ch >= '1' && ch <= '9')
    return ch - '1';
  if (ch >= 'a' && ch <= 'k')
    return ch - 'a' + 9;
  if (ch >= 'm' && ch <= 'z')
    return ch - 'm' + 20;
  if (ch >= 'A' && ch <= 'H')
    return ch - 'A' + 34;
  if (ch >= 'J' && ch <= 'N')
    return ch - 'J' + 42;
  if (ch >= 'P' && ch <= 'Z')
    return ch - 'P' + 47;
  return -1;
}

bool pFlickr::decode(const char* encoded, unsigned long* output)
{
  if (encoded == 0)
    return 0;
  size_t encoded_len = strlen(encoded);
  if (encoded_len >= 12)
    return 0;
  if (output == 0)
    return 0;
  unsigned long output_num = 0;
  size_t unprocessed = encoded_len;
  while (--unprocessed) {
    int idx = encoded_len - unprocessed - 1;
    int num = decode_char(encoded[idx]);
    if (num == -1)
      return 0;
    output_num += num;
    output_num *= 58;
  }
  int remainder = decode_char(encoded[encoded_len-1]);
  if (remainder == -1)
    return 0;
  *output = output_num + remainder;
  return true;
}