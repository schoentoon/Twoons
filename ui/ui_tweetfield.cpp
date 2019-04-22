#include "ui_tweetfield.h"
#include "moc_ui_tweetfield.h"
#include "global.h"

TweetField::TweetField(QWidget *)
{
  setContentsMargins(0,0,0,0);
  layout = new QGridLayout(this);
  layout->setContentsMargins(0,0,0,0);
  field = new QPlainTextEdit(this);
  layout->addWidget(field, 0, 0);
  field->installEventFilter(this);
  sendButton = new QPushButton(this);
  sendButton->setText("Send");
  layout->addWidget(sendButton, 0, 0, Qt::AlignRight|Qt::AlignBottom);
  QObject::connect(sendButton, SIGNAL(clicked())
                  ,this, SLOT(sendTweet()));

  characterCounter = new QLabel(this);
  characterCounter->setText("140");
  characterCounter->setMaximumSize(QSize(30, 16777215));
  layout->addWidget(characterCounter, 0, 0, Qt::AlignRight|Qt::AlignTop);
  QObject::connect(field, SIGNAL(textChanged())
                  ,this, SLOT(updateCounter()));

  retweetButtons = new QHBoxLayout();
  layout->addLayout(retweetButtons, 0, 0, Qt::AlignHCenter|Qt::AlignVCenter);

  retweetButton = new QPushButton(this);
  retweetButton->setText("Retweet");
  retweetButton->setVisible(false);
  retweetButtons->addWidget(retweetButton);
  QObject::connect(retweetButton, SIGNAL(clicked())
                  ,this, SLOT(sendRetweet()));

  editRetweet = new QPushButton(this);
  editRetweet->setText("Edit");
  editRetweet->setVisible(false);
  retweetButtons->addWidget(editRetweet);
  QObject::connect(editRetweet, SIGNAL(clicked())
                  ,this, SLOT(clearRetweet()));

  cancelTweet = new QPushButton(this);
  cancelTweet->setText("Cancel");
  cancelTweet->setVisible(false);
  retweetButtons->addWidget(cancelTweet);
  QObject::connect(cancelTweet, SIGNAL(clicked())
                  ,this, SLOT(cancelTwet()));

  picturePreview = new QLabel(this);
  layout->addWidget(picturePreview, 0, 1);
  pictureCancel = new QPushButton(this);
  pictureCancel->setIcon(*Global::dataStore()->getIcon(dataStorage::Block));
  pictureCancel->setVisible(false);
  QObject::connect(pictureCancel, SIGNAL(clicked())
                  ,this, SLOT(cleanUp()));
  layout->addWidget(pictureCancel, 0, 2);
  progress = new QProgressBar(this);
  layout->addWidget(progress, 0 ,1, Qt::AlignCenter);
  progress->setVisible(false);

  fileDialog = new QFileDialog(this);
  picture = new QFile();
  fileDialog->setNameFilter("Image files (*.jpg *.jpeg *.gif *.png)");
  fileDialog->setFileMode(QFileDialog::ExistingFile);
  selectFile = new QPushButton(this);
  selectFile->setText("Picture");
  layout->addWidget(selectFile, 0, 0, Qt::AlignLeft|Qt::AlignBottom);
  QObject::connect(selectFile, SIGNAL(clicked())
                  ,fileDialog, SLOT(show()));
  QObject::connect(fileDialog, SIGNAL(fileSelected(QString))
                  ,this, SLOT(selectedFile(QString)));
  close = false;
  quit = false;
  network = new QNetworkAccessManager();
  QObject::connect(network, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>))
                  ,Global::dataStore(), SLOT(onIgnoreSSLErrors(QNetworkReply*,QList<QSslError>)));
  networkHandle = NULL;
  uploaders = PictureUpFramework::init(this);
  uploaded = false;
}

bool TweetField::eventFilter(QObject *, QEvent * event)
{
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent* eventKey = static_cast<QKeyEvent*>(event);
    if (eventKey->key() == Qt::Key_Return or eventKey->key() == Qt::Key_Enter) {
      if (eventKey->modifiers() == Qt::ShiftModifier)
        return false;
      else if (!picture->fileName().isEmpty())
        Tweet(field->toPlainText().toLatin1().data(), in_reply_to);
      else if (field->toPlainText().toLatin1().count() < 1)
        Error("How about you enter something to tweet first?");
      else if (field->toPlainText().toLatin1().count() > 140)
        Error("This tweet is to long to be send.");
      else
        Tweet(field->toPlainText().toLatin1().data(), in_reply_to);
      return true;
    } else if (eventKey->key() == Qt::Key_At)
      emit callAutocomplete();
  }
  return false;
}

void TweetField::sendTweet()
{
  if (!picture->fileName().isEmpty())
    Tweet(field->toPlainText().toLatin1().data(), in_reply_to);
  else if (field->toPlainText().toLatin1().count() < 1)
    Error("How about you enter something to tweet first?");
  else if (field->toPlainText().toLatin1().count() > 140)
    Error("This tweet is to long to be send.");
  else
    Tweet(field->toPlainText().toLatin1().data(), in_reply_to);
}

void TweetField::sendRetweet()
{
  Retweet(in_reply_to);
}

void TweetField::updateCounter()
{
  if (field->toPlainText().toLatin1().count() > 140)
    characterCounter->setText(CString("<span style=\"color:#ff0000;font-weight:600;\">" +
                      CString(140 - field->toPlainText().toLatin1().count()) + " </span>").c_str());
  else
    characterCounter->setText(CString(CString(140 - field->toPlainText().toLatin1().count()).c_str() + CString(" ")).c_str());
}

void TweetField::clearRetweet()
{
  retweetButton->setVisible(false);
  editRetweet->setVisible(false);
  sendButton->setEnabled(true);
  selectFile->setEnabled(true);
  field->setEnabled(true);
  field->setFocus();
}

void TweetField::cleanUp()
{
  clearRetweet();
  field->clear();
  cancelTweet->setVisible(false);
  picturePreview->setPixmap(QPixmap());
  pictureCancel->setVisible(false);
  progress->setVisible(false);
  if (picture)
    delete picture;
  picture = new QFile();
  if (close)
    hide();
  if (quit)
    qApp->quit();
}

void TweetField::getNickname(CString tmp)
{
  QTextCursor cursor = field->textCursor();
  cursor.insertText(CString(tmp + " ").c_str());
  field->setTextCursor(cursor);
  field->setFocus();
}

void TweetField::focus()
{
  field->setFocus();
}

void TweetField::tweetButtonClicked(bool tmp)
{
  if (tmp) {
    setVisible(true);
    QTextCursor cursor = field->textCursor();
    cursor.setPosition(field->toPlainText().toLatin1().count());
    field->setTextCursor(cursor);
    field->setFocus();
  } else
    setVisible(false);
}

void TweetField::Tweet(CString msg, unsigned long long in_reply_to)
{
  map<CString,CString> meh;
  meh["status"] = msg;
  if (in_reply_to > 0)
    meh["in_reply_to_status_id"] = CString(in_reply_to);
  if (!picture->fileName().isEmpty() and !uploaded) {
    if (Global::dataStore()->getUploadingService() == 0
        or Global::dataStore()->getUploadingService() > uploaders.size()) {
      if (picture->open(QIODevice::ReadOnly)) {
        QObject::connect(network, SIGNAL(finished(QNetworkReply*))
                        ,this, SLOT(onRequestCompleted(QNetworkReply *)));
        QNetworkRequest tmp = Global::dataStore()->getOAuth()->APICall("https://api.twitter.com/1.1/statuses/update_with_media.json", "POST");
        QString boundary = "----1010101010";
        tmp.setRawHeader("Content-Type",QString("multipart/form-data; boundary=" + boundary).toUtf8());
        QByteArray postData;
        postData = QString("--" + boundary + "\r\n").toAscii();
        for (map<CString,CString>::const_iterator iter = meh.begin(); iter != meh.end(); iter++) {
          postData += "Content-Disposition: form-data; name=\"";
          postData += iter->first.c_str();
          postData += QString("\"\r\n\r\n").toAscii();
          postData += QString(iter->second.c_str()).toAscii();
          postData += QString("\r\n\r\n").toAscii();
          postData += QString("--" + boundary + "\r\n").toAscii();
        }
        QFileInfo info(picture->fileName());
        postData += "Content-Disposition: form-data; name=\"media\"; filename=\"" + info.fileName() + "\"\r\n";
        postData += "Content-Type: application/octet-stream\r\n\r\n";
        postData += picture->readAll();
        postData += "\r\n";
        postData += QString("--" + boundary + "--\r\n").toAscii();
        tmp.setHeader(QNetworkRequest::ContentLengthHeader, QString::number(postData.length()));
        networkHandle = network->post(tmp,postData);
        progress->setMaximum(1);
        progress->setValue(0);
        QObject::connect(networkHandle, SIGNAL(uploadProgress(qint64,qint64))
                        ,this, SLOT(uploadProgress(qint64,qint64)));
        picture->close();
      }
    } else {
      for (unsigned int i = 0; i < uploaders.size(); i++) {
        if (uploaders[i]->getUniqueId() == Global::dataStore()->getUploadingService()) {
          networkHandle = uploaders[i]->upload(picture);
          QObject::connect(networkHandle, SIGNAL(uploadProgress(qint64,qint64))
                ,this, SLOT(uploadProgress(qint64,qint64)));
          break;
        }
      }
    }
  } else {
    meh["include_entities"] = "t";
    QObject::connect(network, SIGNAL(finished(QNetworkReply*))
                    ,this, SLOT(onRequestCompleted(QNetworkReply*)));
    networkHandle = network->post(Global::dataStore()->getOAuth()->APICall("http://api.twitter.com/1.1/statuses/update.json", meh, "POST"),OAuthUtils::JoinPostParams(meh,true).c_str());
  }
  field->setEnabled(false);
  sendButton->setEnabled(false);
  cancelTweet->setVisible(true);
  selectFile->setEnabled(false);
}

void TweetField::Retweet(unsigned long long id)
{
  map<CString,CString> meh;
  meh["include_entities"] = "t";
  field->setEnabled(false);
  sendButton->setEnabled(false);
  cancelTweet->setVisible(true);
  selectFile->setEnabled(false);
  QObject::connect(network, SIGNAL(finished(QNetworkReply*))
                  ,this, SLOT(onRequestCompleted(QNetworkReply *)));
  networkHandle = network->post(Global::dataStore()->getOAuth()->APICall("http://api.twitter.com/1.1/statuses/retweet/" + CString(id) + ".json", meh, "POST"),OAuthUtils::JoinPostParams(meh,true).c_str());
}

void TweetField::Error(CString error)
{
  emit sendError(error);
}

void TweetField::onRequestCompleted(QNetworkReply *reply)
{
  reply->deleteLater();
  if (reply == networkHandle) {
    istringstream input(reply->readAll().data());
    Object o;
    if (Object::parse(input, o)) {
      if (o.has<string>("error")) {
        if (pictureCancel->isVisible()) {
          if (picture)
            delete picture;
          picture = new QFile();
          picturePreview->setPixmap(QPixmap());
          pictureCancel->setVisible(false);
          progress->setVisible(false);
        }
        Error(o.get<string>("error"));
      } else
        cleanUp();
    }
    cancelTweet->setVisible(false);
    field->setEnabled(true);
    sendButton->setEnabled(true);
    progress->setVisible(false);
    selectFile->setEnabled(true);
    field->setFocus();
    networkHandle = NULL;
    uploaded = false;
  }
}

void TweetField::cancelTwet()
{
  if (networkHandle)
    networkHandle->abort();
  field->setEnabled(true);
  sendButton->setEnabled(true);
  cancelTweet->setVisible(false);
  retweetButton->setVisible(false);
  editRetweet->setVisible(false);
}

void TweetField::resizeField(int i)
{
  setMaximumHeight(i);
}

void TweetField::selectedFile(QString file)
{
  QFile picture(file);
  if (!picture.open(QIODevice::ReadOnly))
    Error("Can't open file");
  else {
    QPixmap pixmap;
    if (pixmap.loadFromData(picture.readAll())) {
      pixmap=pixmap.scaled(maximumHeight(),maximumHeight(),Qt::KeepAspectRatio);
      pictureCancel->setVisible(true);
      picturePreview->setPixmap(pixmap);
      progress->setVisible(true);
      progress->setMaximumWidth((pixmap.width()/4)*3);
      progress->setMaximumHeight(pixmap.height()/4);
      progress->setMaximum(1);
      progress->setValue(0);
      picture.close();
      if (this->picture)
        delete this->picture;
      this->picture=new QFile(file);
    } else
      Error("Invalid file.");
  }
}

void TweetField::uploadProgress(qint64 sendBytes, qint64 totalBytes)
{
  progress->setMaximum(totalBytes);
  progress->setValue(sendBytes);
}

void TweetField::appendUrl(QString url)
{
  QString tmp = field->toPlainText();
  field->setPlainText(tmp + " " + url);
  uploaded = true;
  Tweet(field->toPlainText().toLatin1().data(), in_reply_to);
}
