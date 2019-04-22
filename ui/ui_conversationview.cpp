#include "ui_conversationview.h"
#include "moc_ui_conversationview.h"
#include "global.h"

Ui_Conversation::Ui_Conversation(unsigned long long start_id, QWidget *parent)
: Ui_Column("",parent)
{
  setObjectName("conversation");
  closeButton = new QPushButton(this);
  closeButton->setText("X");
  closeButton->setMinimumSize(QSize(26, 26));
  closeButton->setMaximumSize(QSize(26, 26));
  columnLayout->addWidget(closeButton, 0, 0, Qt::AlignRight);

  this->start_id = start_id;
  this->next_id = start_id;
  network = new QNetworkAccessManager();

  QObject::connect(closeButton, SIGNAL(clicked())
                  ,this, SLOT(cleanUp()));
  QObject::connect(network, SIGNAL(finished(QNetworkReply*))
                  ,this, SLOT(onRequestCompleted(QNetworkReply*)));
  QObject::connect(network, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>))
                  ,Global::dataStore(), SLOT(onIgnoreSSLErrors(QNetworkReply*,QList<QSslError>)));
  fill(true);
}

void Ui_Conversation::fill(bool request)
{
  TwStatus *tmp = getTweet(next_id);
  while (tmp) {
    request = true;
    tweetsLayout->addWidget(new TweetBox(tmp));
    if (tmp->isRetweet()) {
      next_id = tmp->getRetweet()->getInReplyTo();
      user_id = tmp->getRetweet()->getInReplyToUserId();
    } else {
      next_id = tmp->getInReplyTo();
      user_id = tmp->getInReplyToUserId();
    }
    tmp = getTweet(next_id);
  }
  if (request and next_id > 0) {
    map<CString,CString> meh;
    meh["user_id"] = CString(user_id);
    meh["include_entities"] = "t";
    meh["include_rts"] = "f";
    meh["max_id"] = CString(next_id+1000);
    meh["count"] = "50";
    networkHandle = network->get(Global::dataStore()->getOAuth()->APICall("http://api.twitter.com/1.1/statuses/user_timeline.json", meh, "GET"));
    newTitle("Looking up some tweets.");
  } else
    newTitle("Done.");
}

TwStatus* Ui_Conversation::getTweet(unsigned long long id)
{
  TwStatus *tmp = Global::Get()->getTweet(id);
  if (tmp)
    return tmp;
  for (unsigned int i = 0; i < tweetBuffer.size(); i++) {
    if (tweetBuffer[i]->getId() == id) {
      tmp = tweetBuffer[i];
      tweetBuffer.erase(tweetBuffer.begin() + i);
      break;
    }
    if (tweetBuffer[i]->isRetweet()) {
      if (tweetBuffer[i]->getRetweet()->getId() == id) {
        tmp = tweetBuffer[i];
        tweetBuffer.erase(tweetBuffer.begin() + i);
        break;
      }
    }
  }
  return tmp;
}

Ui_Conversation::~Ui_Conversation()
{
  delete closeButton;
  if (networkHandle) {
    networkHandle->abort();
    networkHandle->deleteLater();
  }
  for (unsigned int i = 0; i < tweetBuffer.size(); i++)
    delete tweetBuffer[i];
}

void Ui_Conversation::cleanUp()
{
  setVisible(false);
  QLayoutItem* item;
  while ((item = tweetsLayout->takeAt(0)) != 0) {
    delete item->widget();
    delete item;
  }
  deleteLater();
}

void Ui_Conversation::onRequestCompleted(QNetworkReply *reply)
{
  reply->deleteLater();
  if (reply->error() == QNetworkReply::NoError) {
    newTitle("");
    Array a;
    istringstream input(reply->readAll().data());
    if (Array::parse(input, a)) {
      cout << a << endl;
      for (unsigned int i = 0; i < a.size(); i++) {
        if (a.has<Object>(i))
          tweetBuffer.push_back(new TwStatus(a.get<Object>(i)));
      }
      fill(false);
    }
  } else
    newTitle("Something went wrong, stopping here.");
  networkHandle = NULL;
}
