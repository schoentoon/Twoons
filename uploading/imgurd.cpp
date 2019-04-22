#include "uploading/imgurd.h"

#ifndef IMGUR_KEY
#error "You should go and define a IMGUR_KEY in imgur.h"
#endif

QNetworkReply* uImgurD::upload(QFile *file)
{
  map<CString,CString> meh;
  meh["key"] = IMGUR_KEY;
  QNetworkRequest request;
  request.setUrl(QUrl("http://api.imgur.com/2/upload.json"));
  QString boundary = "----1010101010";
  request.setRawHeader("Content-Type",QString("multipart/form-data; boundary=" + boundary).toUtf8());
  QByteArray postData;
  postData = QString("--" + boundary + "\r\n").toAscii();
  for (map<CString,CString>::const_iterator iter = meh.begin(); iter != meh.end(); iter++) {
    postData += "Content-Disposition: form-data; name=\"";
    postData += iter->first.c_str();
    postData += QString("\"\r\n\r\n").toAscii();
    postData += QString(iter->second.c_str()).toAscii();
    postData += QString("\r\n").toAscii();
    postData += QString("--" + boundary + "\r\n").toAscii();
  }
  if (!file->fileName().isEmpty() and file->open(QIODevice::ReadOnly)) {
    QFileInfo info(file->fileName());
    postData += "Content-Disposition: form-data; name=\"image\"; filename=\"" + info.fileName() + "\"\r\n";
    postData += "Content-Type: application/octet-stream\r\n\r\n";
    postData += file->readAll();
    postData += "\r\n";
    postData += QString("--" + boundary + "--\r\n").toAscii();
  }
  file->close();
  request.setHeader(QNetworkRequest::ContentLengthHeader, QString::number(postData.length()));
  request.setRawHeader("User-Agent", "Twoons for " OS " ~ " VERSION);
  return network->post(request,postData);
}

unsigned int uImgurD::getUniqueId()
{
  return 3;
}

QString uImgurD::getName()
{
  return "Imgur (direct link)";
}

void uImgurD::postUpload(QNetworkReply *reply)
{
  istringstream input(reply->readAll().data());
  Object o;
  Object::parse(input, o);
  if (o.has<Object>("upload")) {
    if (o.get<Object>("upload").has<Object>("links")) {
      if (o.get<Object>("upload").get<Object>("links").has<string>("original"))
        emit sendUrl(QString(o.get<Object>("upload").get<Object>("links").get<string>("original").c_str()).replace("http://imgur.com","http://i.imgur.com"));
    }
  }
}
