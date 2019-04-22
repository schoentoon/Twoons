#include "ui_tweetbox.h"
#include "moc_ui_tweetbox.h"
#include "global.h"

TweetBox::TweetBox(TwStatus *tweet)
{
  setObjectName("tweet");
  pTweet = tweet;
  setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
  Global::dataStore()->addUser(new StoreUser(pTweet->getUser()));
  CString tmp = "This tweet was sent at: " + pTweet->getCreated_at().toString("ddd d MMM hh:mm:ss").toStdString();
  setToolTip(tmp.c_str());
  setContentsMargins(5, 5, 5, 5);
  layout = new QGridLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  profilePicture = new QLabel(this);
  profilePicture->setMaximumSize(0,0);
  if (pTweet->isRetweet())
    layout->addWidget(profilePicture, 1, 0);
  else
    layout->addWidget(profilePicture, 0, 0, 2, 1);
  QPixmap pixmap;
  if (pTweet->isRetweet()) {
    if (QPixmapCache::find(pTweet->getRetweet()->getUser()->getProfilePicture().c_str(), &pixmap))
      profilePicture->setPixmap(pixmap);
  } else {
    if (QPixmapCache::find(pTweet->getUser()->getProfilePicture().c_str(), &pixmap))
      profilePicture->setPixmap(pixmap);
  }
  if (profilePicture->pixmap() == 0) {
    QObject::connect(Global::dataStore(), SIGNAL(sendProfilePicture(CString,QPixmap))
                    ,this, SLOT(getProfilePicture(CString,QPixmap)));
    if (pTweet->isRetweet())
      Global::dataStore()->getProfilePicture(pTweet->getRetweet()->getUser()->getProfilePicture());
    else
      Global::dataStore()->getProfilePicture(pTweet->getUser()->getProfilePicture());
  } else
    profilePicture->setMaximumSize(pixmap.width(), pixmap.height());

  userName = new QLabel(this);
  userName->setTextFormat(Qt::AutoText);
  userName->setAlignment(Qt::AlignCenter);
  if (pTweet->isRetweet())
    userName->setText(CString("<span style=\"font-size:8pt\"><a href=\"user_id:" + CString(pTweet->getRetweet()->getUser()->getId()) +
                      ";screen_name:" + pTweet->getRetweet()->getUser()->getName() + "\">" +
                      pTweet->getRetweet()->getUser()->getName() + "</a> RT by <a href=\"user_id:" +
                      CString(pTweet->getUser()->getId()) + ";screen_name:" + pTweet->getUser()->getName() + "\">" +
                      pTweet->getUser()->getName() + "</a></span>").c_str());
  else
    userName->setText(CString("<span style=\"font-size:8pt\"><a href=\"user_id:" +
                      CString(pTweet->getUser()->getId()) + ";screen_name:" + pTweet->getUser()->getName() + "\">" +
                      pTweet->getUser()->getName() + "</a></span>").c_str());
  layout->addWidget(userName, 0, 0, 1, -1, Qt::AlignHCenter);

  tweetText = new QTextBrowser(this);
  tweetText->setContentsMargins(0,0,0,0);
  tweetText->setAcceptRichText(true);
  if (pTweet->isRetweet())
    tweetText->setText(pTweet->getRetweet()->getText(true).c_str());
  else
    tweetText->setText(pTweet->getText(true).c_str());
  tweetText->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);
  tweetText->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  tweetText->setFrameShape(QTextEdit::NoFrame);
  tweetText->setFrameShadow(QTextEdit::Plain);
  tweetText->viewport()->setAutoFillBackground(false);
  tweetText->setReadOnly(true);
  tweetText->setContentsMargins(5,0,0,0);
  tweetText->setOpenLinks(false);
  QObject::connect(tweetText->document()->documentLayout(), SIGNAL(documentSizeChanged(QSizeF))
                  ,this, SLOT(adjustTweetText(QSizeF)));
  QObject::connect(tweetText, SIGNAL(anchorClicked(QUrl))
                  ,this, SLOT(urlClicked(QUrl)));
  layout->addWidget(tweetText, 1, 1, 1, 1, Qt::AlignTop);

  sourceText = new QLabel(this);
  tmp = "<span style=\"font-size:7pt\">";
  tmp = tmp + pTweet->getCreated_at().toString("hh:mm").toStdString() + " via ";
  tmp = tmp + pTweet->getSource(true);
  if (pTweet->isRetweet()) {
    if (pTweet->getRetweet()->getInReplyTo() > 0) {
      tmp = tmp + " in reply to <a href=\"reply://" + CString(pTweet->getRetweet()->getId()) + "\">";
      tmp = tmp + pTweet->getRetweet()->getInReplyToUser() + "</a>";
    }
  } else {
    if (pTweet->getInReplyTo() > 0) {
      tmp = tmp + " in reply to <a href=\"reply://" + CString(pTweet->getId()) + "\">";
      tmp = tmp + pTweet->getInReplyToUser() + "</a>";
    }
  }
  tmp = tmp + "</span>";
  sourceText->setText(tmp.c_str());
  sourceText->setTextFormat(Qt::RichText);
  sourceText->setMaximumHeight(16);
  layout->addWidget(sourceText, 2, 0, 1, -1);

  buttonLayout = new QGridLayout();
  buttonLayout->setContentsMargins(0,0,0,10);
  layout->addLayout(buttonLayout, 0, 0, -1, 0, Qt::AlignRight|Qt::AlignBottom);

  extraButton = new QPushButton(this);
  extraButton->setVisible(false);
  extraButton->setToolTip("Show some extra options");
  extraButton->setMaximumSize(QSize(32, 32));
  extraButton->setText("^");
  buttonLayout->addWidget(extraButton, 1, 0);

  replyButton = new QPushButton(this);
  replyButton->setVisible(false);
  replyButton->setToolTip("Reply");
  replyButton->setMaximumSize(QSize(32, 32));
  replyButton->setFlat(true);
  replyButton->setIcon(*Global::dataStore()->getIcon(dataStorage::Reply));
  buttonLayout->addWidget(replyButton, 1, 1);

  retweetButton = new QPushButton(this);
  retweetButton->setVisible(false);
  retweetButton->setToolTip("Retweet");
  retweetButton->setFlat(true);
  retweetButton->setMaximumSize(QSize(32, 32));
  if (pTweet->isRetweeted())
    retweetButton->setIcon(*Global::dataStore()->getIcon(dataStorage::Retweeted));
  else
    retweetButton->setIcon(*Global::dataStore()->getIcon(dataStorage::Retweet));
  buttonLayout->addWidget(retweetButton, 1, 2);

  favoriteButton = new QPushButton(this);
  if (pTweet->isFavorited())
    favoriteButton->setToolTip("Unfavorite");
  else
    favoriteButton->setToolTip("Favorite");
  favoriteButton->setVisible(false);
  favoriteButton->setFlat(true);
  favoriteButton->setMaximumSize(QSize(32, 32));
  if (pTweet->isFavorited())
    favoriteButton->setIcon(*Global::dataStore()->getIcon(dataStorage::Favorited));
  else
    favoriteButton->setIcon(*Global::dataStore()->getIcon(dataStorage::Favorite));

  if (pTweet->isMe()) {
    deleteButton = new QPushButton(this);
    deleteButton->setVisible(false);
    deleteButton->setToolTip("Delete");
    deleteButton->setMaximumSize(QSize(32, 32));
    deleteButton->setIcon(*Global::dataStore()->getIcon(dataStorage::Delete));
    deleteButton->setFlat(true);
    buttonLayout->addWidget(deleteButton, 0, 1);
    buttonLayout->addWidget(favoriteButton, 0, 2);
    QObject::connect(deleteButton, SIGNAL(clicked())
                    ,SLOT(deleteButtonClicked()));
  } else {
    blockButton = new QPushButton(this);
    blockButton->setVisible(false);
    blockButton->setToolTip("Block");
    blockButton->setMaximumSize(QSize(32, 32));
    blockButton->setIcon(*Global::dataStore()->getIcon(dataStorage::Block));
    blockButton->setFlat(true);
    buttonLayout->addWidget(blockButton, 0, 0);

    reportButton = new QPushButton(this);
    reportButton->setVisible(false);
    reportButton->setToolTip("Report");
    reportButton->setMaximumSize(QSize(32, 32));
    reportButton->setIcon(*Global::dataStore()->getIcon(dataStorage::Report));
    reportButton->setFlat(true);
    buttonLayout->addWidget(reportButton, 0, 1);
    buttonLayout->addWidget(favoriteButton, 0, 2);

    QObject::connect(blockButton, SIGNAL(clicked())
                    ,SLOT(blockButtonClicked()));
    QObject::connect(reportButton, SIGNAL(clicked())
                    ,SLOT(reportButtonClicked()));
  }

  adjustTweetText(tweetText->document()->size());
  QObject::connect(extraButton, SIGNAL(clicked())
                  ,SLOT(showExtraButtons()));
  QObject::connect(replyButton, SIGNAL(clicked())
                  ,SLOT(replyButtonClicked()));
  QObject::connect(retweetButton, SIGNAL(clicked())
                  ,SLOT(retweetButtonClicked()));
  QObject::connect(favoriteButton, SIGNAL(clicked())
                  ,SLOT(favoriteButtonClicked()));
  QObject::connect(userName, SIGNAL(linkActivated(QString))
                  ,SLOT(getUserProfile(QString)));
  QObject::connect(sourceText, SIGNAL(linkActivated(QString))
                  ,SLOT(conversationView(QString)));
}

TweetBox::~TweetBox()
{
  delete layout;
  delete sourceText;
  delete userName;
  delete profilePicture;
  delete tweetText;
  delete replyButton;
  delete retweetButton;
  delete favoriteButton;
  delete extraButton;
  if (pTweet->isMe())
    delete deleteButton;
  else {
    delete blockButton;
    delete reportButton;
  }
  delete pTweet;
}

void TweetBox::enterEvent(QEvent *)
{
  extraButton->setVisible(true);
  replyButton->setVisible(true);
  retweetButton->setVisible(true);
  QTimer::singleShot(5000, this, SLOT(hideButtons()));
}

void TweetBox::leaveEvent(QEvent *)
{
  hideButtons();
}

void TweetBox::hideButtons()
{
  extraButton->setVisible(false);
  replyButton->setVisible(false);
  retweetButton->setVisible(false);
  favoriteButton->setVisible(false);
  if (pTweet->isMe())
    deleteButton->setVisible(false);
  else {
    blockButton->setVisible(false);
    reportButton->setVisible(false);
  }
}

void TweetBox::showExtraButtons()
{
  favoriteButton->setVisible(true);
  if (pTweet->isMe())
    deleteButton->setVisible(true);
  else {
    blockButton->setVisible(true);
    reportButton->setVisible(true);
  }
}

void TweetBox::alert()
{
  if (Notification::isInitted()) {
    Notification *notif = new Notification(QString(pTweet->getUser()->getName().c_str()), QString(pTweet->getText().c_str()), "", this);
    notif->addAction("reply","Reply");
    notif->addAction("retweet","Retweet");
    QObject::connect(notif, SIGNAL(actionInvoked(QString))
                    ,this, SLOT(notificationClicked(QString)));
    notif->show();
  }
}

void TweetBox::getUserProfile(QString link)
{
  QObject::connect(this, SIGNAL(sendUserProfile(QString))
                  ,Global::Get(), SLOT(getUserProfile(QString)));
  emit sendUserProfile(link);
}

/* Tweet markers */

void TweetBox::markRetweeted()
{
  pTweet->setRetweeted(true);
  retweetButton->setIcon(*Global::dataStore()->getIcon(dataStorage::Retweeted));
}

void TweetBox::markFavorited()
{
  pTweet->setFavorite(true);
  favoriteButton->setIcon(*Global::dataStore()->getIcon(dataStorage::Favorited));
  favoriteButton->setToolTip("Unfavorite");
}

void TweetBox::markUnFavorited()
{
  pTweet->setFavorite(false);
  favoriteButton->setIcon(*Global::dataStore()->getIcon(dataStorage::Favorite));
  favoriteButton->setToolTip("Favorite");
}

/* Button clicked slots */

void TweetBox::replyButtonClicked()
{
  QObject::connect(this, SIGNAL(sendReply(unsigned long long,CString))
                  ,Global::Get(), SLOT(inReplyTo(unsigned long long,CString)));
  emit sendReply(pTweet->getId(), pTweet->replyAll());
}

void TweetBox::retweetButtonClicked()
{
  QObject::connect(this, SIGNAL(sendRetweet(unsigned long long,CString))
                  ,Global::Get(), SLOT(sendRetweet(unsigned long long,CString)));
  if (pTweet->isRetweet())
    emit sendRetweet(pTweet->getRetweet()->getId(), "RT @" + pTweet->getRetweet()->getUser()->getName() +
                                                   ": " + pTweet->getRetweet()->getText());
  else
    emit sendRetweet(pTweet->getId(), "RT @" + pTweet->getUser()->getName() + ": " + pTweet->getText());
}

void TweetBox::favoriteButtonClicked()
{
  QObject::connect(this, SIGNAL(sendAPI(unsigned long long,APItypes))
                  ,Global::Get(), SLOT(sendAPI(unsigned long long,APItypes)));
  if (pTweet->isFavorited())
    emit sendAPI(pTweet->getId(),UnFavorite);
  else
    emit sendAPI(pTweet->getId(),Favorite);
}

void TweetBox::deleteButtonClicked()
{
  QObject::connect(this, SIGNAL(sendAPI(unsigned long long,APItypes))
                  ,Global::Get(), SLOT(sendAPI(unsigned long long,APItypes)));
  emit sendAPI(pTweet->getId(),Delete);
}

void TweetBox::blockButtonClicked()
{
  QObject::connect(this, SIGNAL(sendAPI(unsigned long long,APItypes))
                  ,Global::Get(), SLOT(sendAPI(unsigned long long,APItypes)));
  emit sendAPI(pTweet->getUser()->getId(),Block);
}

void TweetBox::reportButtonClicked()
{
  QObject::connect(this, SIGNAL(sendAPI(unsigned long long,APItypes))
                  ,Global::Get(), SLOT(sendAPI(unsigned long long,APItypes)));
  emit sendAPI(pTweet->getUser()->getId(),Report);
}

void TweetBox::getProfilePicture(CString url,QPixmap pixmap)
{
  if (profilePicture->pixmap() != 0)
    return;
  if (pTweet->isRetweet()) {
    if (pTweet->getRetweet()->getUser()->getProfilePicture().Equals(url))
      profilePicture->setPixmap(pixmap);
  } else {
    if (pTweet->getUser()->getProfilePicture().Equals(url))
      profilePicture->setPixmap(pixmap);
  }
  if (profilePicture->pixmap() != 0)
    profilePicture->setMaximumSize(pixmap.width(),pixmap.height());
}

void TweetBox::adjustTweetText(QSizeF size)
{
  tweetText->setMinimumHeight(size.height() + 2 * tweetText->frameWidth() + 5);
  tweetText->setMaximumHeight(size.height() + 2 * tweetText->frameWidth() + 5);
}

void TweetBox::urlClicked(QUrl url)
{
  QObject::connect(this, SIGNAL(sendUserProfile(QString))
                  ,Global::Get(), SLOT(getUserProfile(QString)));
  if (url.host() == "twitter.com" and url.hasQueryItem("user_id"))
    emit sendUserProfile("user_id:" + url.encodedQueryItemValue("user_id") + ";screen_name:" + url.path().replace("/",""));
  else
    emit sendUserProfile(url.toString());
}

void TweetBox::conversationView(QString link)
{
  QUrl url = QUrl(link);
  if (url.scheme() == "reply")
    Global::Get()->openConversationView(CString(url.host().toStdString()).ToULong(), this);
  else
    QDesktopServices::openUrl(url);
}

void TweetBox::notificationClicked(QString actionKey)
{
  if (actionKey == "reply")
    replyButtonClicked();
  else if (actionKey == "retweet")
    retweetButtonClicked();
}
