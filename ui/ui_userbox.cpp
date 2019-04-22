#include "ui_userbox.h"
#include "moc_ui_userbox.h"
#include "global.h"

UserBox::UserBox(TwUser *user)
{
  setObjectName("user");
  pUser = user;
  QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
  setSizePolicy(sizePolicy);
  setMinimumSize(QSize(250, 50));
  setMaximumSize(QSize(250, 9001));
  QFont font;
  font.setPointSize(8);
  setFont(font);
  layout = new QGridLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);

  profilePicture = new QLabel(this);
  profilePicture->setMaximumSize(0,0);
  layout->addWidget(profilePicture, 0, 0, 2, 1);
  QPixmap pixmap;
  if (QPixmapCache::find(pUser->getProfilePicture().c_str(), &pixmap)) {
    profilePicture->setPixmap(pixmap);
    profilePicture->setMaximumSize(pixmap.width(), pixmap.height());
  } else {
    QObject::connect(Global::dataStore(), SIGNAL(sendProfilePicture(CString,QPixmap))
                    ,this, SLOT(getProfilePicture(CString,QPixmap)));
    Global::dataStore()->getProfilePicture(pUser->getProfilePicture());
  }

  userName = new QLabel(this);
  userName->setTextFormat(Qt::RichText);
  userName->setAlignment(Qt::AlignCenter);
  userName->setText(CString("<a href=\"user_id:" +
                    CString(pUser->getId()) + ";screen_name:" + pUser->getName() + "\">" +
                    pUser->getName() + "</a>").c_str());
  layout->addWidget(userName, 0, 0, 1, -1, Qt::AlignHCenter);

  description = new QLabel(this);
  font.setPointSize(10);
  description->setFont(font);
  description->setText(pUser->getDescription(true).c_str());
  description->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);
  description->setTextFormat(Qt::RichText);
  description->setWordWrap(true);
  description->setContentsMargins(5,0,0,0);
  layout->addWidget(description, 1, 1, 1, 1, Qt::AlignTop);

  buttonLayout = new QGridLayout();
  layout->addLayout(buttonLayout, 0, 0, -1, 0, Qt::AlignRight|Qt::AlignBottom);

  replyButton = new QPushButton(this);
  replyButton->setVisible(false);
  replyButton->setToolTip("Reply");
  replyButton->setMaximumSize(QSize(32, 32));
  replyButton->setFlat(true);
  replyButton->setIcon(*Global::dataStore()->getIcon(dataStorage::Reply));
  buttonLayout->addWidget(replyButton, 1, 2);

  if (!pUser->isMe()) {
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

    unfollowButton = new QPushButton(this);
    unfollowButton->setVisible(false);
    unfollowButton->setPalette(QPalette(Qt::darkRed));
    unfollowButton->setToolTip("Unfollow");
    unfollowButton->setMaximumSize(QSize(16, 32));
    buttonLayout->addWidget(unfollowButton, 0, 2, Qt::AlignLeft);

    followButton = new QPushButton(this);
    followButton->setVisible(false);
    followButton->setPalette(QPalette(Qt::darkGreen));
    followButton->setToolTip("Follow");
    followButton->setMaximumSize(QSize(16, 32));
    buttonLayout->addWidget(followButton, 0, 2, Qt::AlignRight);
    QObject::connect(blockButton, SIGNAL(clicked())
                    ,SLOT(blockButtonClicked()));
    QObject::connect(reportButton, SIGNAL(clicked())
                    ,SLOT(reportButtonClicked()));
    QObject::connect(followButton, SIGNAL(clicked())
                    ,SLOT(followButtonClicked()));
    QObject::connect(unfollowButton, SIGNAL(clicked())
                    ,SLOT(unfollowButtonClicked()));
  }

  QObject::connect(replyButton, SIGNAL(clicked())
                  ,SLOT(replyButtonClicked()));
  QObject::connect(userName, SIGNAL(linkActivated(QString))
                  ,SLOT(getUserProfile(QString)));
}

UserBox::~UserBox()
{
  delete layout;
  delete userName;
  delete description;
  delete profilePicture;
  if (!pUser->isMe()) {
    delete replyButton;
    delete blockButton;
    delete reportButton;
    delete unfollowButton;
  }
  delete pUser;
}

void UserBox::enterEvent(QEvent *)
{
  replyButton->setVisible(true);
  if (!pUser->isMe()) {
    blockButton->setVisible(true);
    reportButton->setVisible(true);
    unfollowButton->setVisible(true);
    followButton->setVisible(true);
  }
}

void UserBox::leaveEvent(QEvent *)
{
  replyButton->setVisible(false);
  if (!pUser->isMe()) {
    blockButton->setVisible(false);
    reportButton->setVisible(false);
    unfollowButton->setVisible(false);
    followButton->setVisible(false);
  }
}

void UserBox::getUserProfile(QString link)
{
  QObject::connect(this, SIGNAL(sendUserProfile(QString))
                  ,Global::Get(), SLOT(getUserProfile(QString)));
  emit sendUserProfile(link);
}

/* Button clicked slots */

void UserBox::replyButtonClicked()
{
  QObject::connect(this, SIGNAL(sendReply(CString,CString))
                  ,Global::Get(), SLOT(inReplyTo(CString,CString)));
  emit sendReply("", "@" + pUser->getName() + " ");
}

void UserBox::blockButtonClicked()
{
  QObject::connect(this, SIGNAL(sendFavoriteThread(CString,unsigned long))
                  ,Global::Get(), SLOT(getFavoriteThread(CString,unsigned long)));
  emit sendFavoriteThread(CString(pUser->getId()),3);
}

void UserBox::reportButtonClicked()
{
  QObject::connect(this, SIGNAL(sendFavoriteThread(CString,unsigned long))
                  ,Global::Get(), SLOT(getFavoriteThread(CString,unsigned long)));
  emit sendFavoriteThread(CString(pUser->getId()),4);
}

void UserBox::followButtonClicked()
{
  QObject::connect(this, SIGNAL(sendFavoriteThread(CString,unsigned long))
                  ,Global::Get(), SLOT(getFavoriteThread(CString,unsigned long)));
  emit sendFavoriteThread(CString(pUser->getId()),5);
}

void UserBox::unfollowButtonClicked()
{
  QObject::connect(this, SIGNAL(sendFavoriteThread(CString,unsigned long))
                  ,Global::Get(), SLOT(getFavoriteThread(CString,unsigned long)));
  emit sendFavoriteThread(CString(pUser->getId()),6);
}

void UserBox::getProfilePicture(CString url,QPixmap pixmap)
{
  if (profilePicture->pixmap() != 0)
    return;
  if (pUser->getProfilePicture().Equals(url))
    profilePicture->setPixmap(pixmap);
  if (profilePicture->pixmap() != 0)
    profilePicture->setMaximumSize(pixmap.width(), pixmap.height());
}
