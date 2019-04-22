#ifndef UI_USERBOX_H
#define UI_USERBOX_H

#include "user.h"
#include "datastorage.h"
/* Qt includes */
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QPixmapCache>
#include <iostream>

using namespace std;
/** @brief UserBox class
 * @section description This class is used to display an actual tweet */
class UserBox: public QGroupBox
{
  Q_OBJECT
public:
  /** @brief Constructor
    * @param user The tweet to use in this TweetBox */
  UserBox(TwUser *user);
  /** @brief Destructor */
  ~UserBox();
  /** @brief overriden from QGroupBox
   * this will simply make the reply button visible when hovering over it */
  void enterEvent(QEvent *);
  /** @brief overriden from QGroupBox
   * this will simply make the reply button invisible again when mouse is gone */
  void leaveEvent(QEvent *);
  TwUser* getUser() { return pUser; };
protected:
  QGridLayout *layout, *buttonLayout;
  QLabel *userName, *description, *profilePicture;
  QPushButton *replyButton, *blockButton, *reportButton, *unfollowButton, *followButton;
  TwUser *pUser;
public slots:
  void getUserProfile(QString);
  void replyButtonClicked();
  void blockButtonClicked();
  void reportButtonClicked();
  void unfollowButtonClicked();
  void followButtonClicked();
  void getProfilePicture(CString,QPixmap);
signals:
  void sendReply(CString,CString);
  void sendRetweet(CString,CString);
  void sendUserProfile(QString);
  void sendFavoriteThread(CString,unsigned long long);
};

#endif // UI_USERBOX_H
