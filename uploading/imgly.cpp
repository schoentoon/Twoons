#include "uploading/imgly.h"

QNetworkReply* uImgLy::upload(QFile *file)
{
  map<CString,CString> meh = Global::dataStore()->getOAuth()->Echo("https://api.twitter.com/1.1/account/verify_credentials.json");
  QNetworkRequest request = Global::dataStore()->getOAuth()->Echo(meh,"http://api.twitter.com/","X-Verify-Credentials-Authorization");
  request.setRawHeader("X-Auth-Service-Provider","https://api.twitter.com/1.1/account/verify_credentials.json");
  request.setUrl(QUrl("http://img.ly/api/2/upload.json"));
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
    postData += "Content-Disposition: form-data; name=\"media\"; filename=\"" + info.fileName() + "\"\r\n";
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

unsigned int uImgLy::getUniqueId()
{
  return 4;
}

QString uImgLy::getName()
{
  return "ImgLy";
}

void uImgLy::postUpload(QNetworkReply *reply)
{
  istringstream input(reply->readAll().data());
  Object o;
  Object::parse(input, o);
  if (o.has<string>("url"))
    emit sendUrl(QString(o.get<string>("url").c_str()));
}
