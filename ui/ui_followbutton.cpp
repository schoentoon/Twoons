#include "ui_followbutton.h"
#include "moc_ui_followbutton.h"
#include "global.h"

Ui_FollowButton::Ui_FollowButton(QWidget* parent)
: QGroupBox(parent)
{
  mainLayout = new QGridLayout(this);
  setLayout(mainLayout);

  followButton = new QPushButton(this);
  QObject::connect(followButton, SIGNAL(clicked())
                  ,this, SLOT(followButtonClicked()));
  mainLayout->addWidget(followButton, 0, 0);

  dmButton = new QPushButton(this);
  dmButton->setIcon(QIcon(":icons/dm.png"));
  mainLayout->addWidget(dmButton, 0, 1);
  dmButton->setMaximumWidth(dmButton->height()*1.4);
  QObject::connect(dmButton, SIGNAL(clicked())
                  ,this, SLOT(dmButtonClicked()));
}

void Ui_FollowButton::setFriendShip(Object& o)
{
  if (o.has<string>("mhmm")) {
    followButton->setText(o.get<string>("mhmm").c_str());
    followButton->setProperty("me",true);
    followButton->setEnabled(false);
    dmButton->setVisible(false);
  } else {
    followButton->setProperty("me",false);
    if (o.has<Object>("relationship")) {
      if (o.get<Object>("relationship").has<Object>("target")
        and o.get<Object>("relationship").has<Object>("source")) {
        if (o.get<Object>("relationship").get<Object>("target").has<string>("id_str"))
          user_id = CString(o.get<Object>("relationship").get<Object>("target").get<string>("id_str")).ToULong();
        if (o.get<Object>("relationship").get<Object>("target").has<bool>("followed_by")
          and o.get<Object>("relationship").get<Object>("source").has<bool>("blocking")) {
          if (o.get<Object>("relationship").get<Object>("source").get<bool>("blocking")) {
            followButton->setProperty("follow",false);
            followButton->setProperty("blocked",true);
          } else if (o.get<Object>("relationship").get<Object>("target").get<bool>("followed_by")) {
            followButton->setProperty("follow",true);
            followButton->setProperty("blocked",false);
          } else {
            followButton->setProperty("follow",false);
            followButton->setProperty("blocked",false);
          }
          followButton->setEnabled(true);
          leaveEvent(NULL);
          if (o.get<Object>("relationship").get<Object>("source").has<bool>("followed_by")
            and o.get<Object>("relationship").get<Object>("source").has<bool>("blocking")) {
            if (o.get<Object>("relationship").get<Object>("source").get<bool>("blocking"))
              followInfo = "You've blocked this person";
            else if (o.get<Object>("relationship").get<Object>("target").get<bool>("followed_by") and
                o.get<Object>("relationship").get<Object>("source").get<bool>("followed_by"))
              followInfo = "You're following each other";
            else if (o.get<Object>("relationship").get<Object>("target").get<bool>("followed_by"))
              followInfo = "You're following this person";
            else if (o.get<Object>("relationship").get<Object>("source").get<bool>("followed_by"))
              followInfo = "You're followed by this person";
            else
              followInfo = "You're not following this person";
          }
          if (o.get<Object>("relationship").get<Object>("source").has<bool>("can_dm"))
            dmButton->setVisible(o.get<Object>("relationship").get<Object>("source").get<bool>("can_dm"));
        }
      }
    }
  }
  style()->unpolish(qApp);
  style()->polish(qApp);
  setVisible(true);
}

void Ui_FollowButton::enterEvent(QEvent*)
{
  followButton->setText(followInfo);
}

void Ui_FollowButton::leaveEvent(QEvent*)
{
  if (followButton->property("blocked").toBool())
    followButton->setText("Unblock");
  else
    followButton->setText(followButton->property("follow").toBool() ? "Unfollow" : "Follow");
}

void Ui_FollowButton::followButtonClicked()
{
  QObject::connect(this, SIGNAL(sendAPI(unsigned long long,APItypes))
                  ,Global::Get(), SLOT(sendAPI(unsigned long long,APItypes)));
  if (followButton->property("blocked").toBool())
    emit sendAPI(user_id,UnBlock);
  else if (followButton->property("follow").toBool())
    emit sendAPI(user_id,UnFollow);
  else
    emit sendAPI(user_id,Follow);
}

void Ui_FollowButton::dmButtonClicked()
{
  emit openDM();
}