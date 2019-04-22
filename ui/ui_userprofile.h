#ifndef UI_USERPROFILE_H
#define UI_USERPROFILE_H

#include "CString.h"
#include "OAuth.h"
#include "list.h"
#include "user.h"
#include "ui_followbutton.h"
#include "ui_tweetbox.h"
#include "ui_listbox.h"
#include "ui_userbox.h"
#include "ui_column.h"
#include "datastorage.h"
/* Qt includes */
#include <QtGui/QGroupBox>
#include <QtGui/QGridLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QScrollArea>
#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStyle>

using namespace std;
using namespace jsonxx;
/**
 * @brief User profile viewer class
 * @section description This class is used to show someone's profile
 */
class Ui_UserProfile: public Ui_Column
{
  Q_OBJECT
public:
  Ui_UserProfile(QWidget *parent = 0);
  void setUser(TwUser tuser);
  void setUserId(unsigned int i) { me = i; };
  void fromMainWindow(Object& data, APItypes type);
  typedef enum {
    Tweets      = 0,
    Favorites   = 1,
    Lists       = 2,
    Followers   = 3,
    Following   = 4,
    Mentions    = 5,
    UserProfile = 6,
    Friendship  = 7
  } CallType;
protected:
  QGridLayout *mainLayout;
  QPushButton *closeButton;
  QLabel *realName, *description, *extraInfo;
  QPushButton *tweetCount, *favoriteCount, *listCount, *followersCount, *followingCount, *mentionsButton;
  Ui_FollowButton *followButton;
  unsigned int me;
  CallType type;
  QNetworkReply *networkHandle;
  CString screen_name;
  unsigned long user_id;
  void getPage(CallType);
signals:
  void sendError(CString);
  void openDM(CString,unsigned int);
private slots:
  void openDM();
public slots:
  void openProfile(unsigned long);
  void openProfile(CString);
  void renderProfile(CString,CallType);
  void cleanUp();
  void getFavorites(bool);
  void getTweets(bool);
  void getLists(bool);
  void getMentions(bool);
  void getFollowers(bool);
  void getFollowing(bool);
  void onRequestCompleted(QNetworkReply *reply);
};

#endif // UI_USERPROFILE_H
