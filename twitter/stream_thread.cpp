#include "stream_thread.h"
#include "moc_stream_thread.h"
#include "global.h"

StreamThread::StreamThread(bool tmp)
{
  replies=tmp;
  stream = NULL;
  timer = new QTimer(this); /* Checking every 10 seconds should be enough. */
  timer->setInterval(10000); /* As twitter will ALWAYS send stuff while streaming */
  //timer->start(); /* Every 30 seconds to be exact, to keep the stream alive. */
  QObject::connect(timer, SIGNAL(timeout()) /* source: https://dev.twitter.com/docs/streaming-api/user-streams/suggestions */
                  ,this, SLOT(checkConnection()));
  QObject::connect(qApp, SIGNAL(aboutToQuit())
                  ,this, SLOT(closeConnection()));
  network = new QNetworkAccessManager();
  QObject::connect(network, SIGNAL(finished(QNetworkReply*))
                  ,this, SLOT(onRequestCompleted(QNetworkReply*)));
  QObject::connect(network, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>))
                  ,Global::dataStore(), SLOT(onIgnoreSSLErrors(QNetworkReply*,QList<QSslError>)));
}

void StreamThread::start()
{
  if (stream != NULL)
    return;
  map<CString,CString> meh;
  if (replies)
    meh["replies"] = "all";
  QNetworkRequest request = Global::dataStore()->getOAuth()->APICall("https://userstream.twitter.com/1.1/user.json", meh, "GET");
  request.setRawHeader("Accept-Encoding","identity");
  request.setRawHeader("Connection","Keep-Alive");
  request.setRawHeader("Keep-Alive","timeout=10, max=3");
  stream = network->get(request);
  QObject::connect(stream, SIGNAL(readyRead())
                  ,this, SLOT(newData()));
  if (!timer->isActive())
    timer->start();
}

void StreamThread::newData()
{
  while (stream->canReadLine())
    emit sendData(stream->readLine().data());
  counter = 0;
}

void StreamThread::stop()
{
  closeConnection();
  timer->stop();
}

void StreamThread::checkConnection()
{
  counter++;
  if (counter > 3) {
    if (counter % 3 == 0) {
      if (stream)
        stream->abort();
      stream = NULL;
      start();
    }
    emit needBackfill();
  }
}

void StreamThread::closeConnection()
{
  if (stream)
    stream->abort();
}

void StreamThread::setReplies(bool tmp)
{
  if (tmp != replies) {
    replies = tmp;
    if (stream) {
      if (stream->isRunning()) {
        stream->abort();
        stream = NULL;
        start();
      }
    }
  }
}

void StreamThread::onRequestCompleted(QNetworkReply* reply)
{
  reply->deleteLater();
  if (reply == stream) {
    stream = NULL;
    DEBUG("Error: " << reply->error());
    DEBUG("Stream closed with a " << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
  }
}