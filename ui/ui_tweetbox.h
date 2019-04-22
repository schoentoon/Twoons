#ifndef UI_TWEETBOX_H
#define UI_TWEETBOX_H

#include "status.h"
#include "datastorage.h"
#include "notification.h"
#include "actions.h"
/* Qt includes */
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QtGui/QTextBrowser>
#include <QAbstractTextDocumentLayout>
#include <iostream>

using namespace std;
/** @brief TweetBox class
 * @section description This class is used to display an actual tweet */
class TweetBox: public QGroupBox
{
  Q_OBJECT
public:
  /** @brief Constructor
    * @param tweet The tweet to use in this TweetBox */
  TweetBox(TwStatus *tweet);
  /** @brief Destructor */
  ~TweetBox();
  /** @brief overriden from QGroupBox
   * this will simply make the reply button visible when hovering over it */
  void enterEvent(QEvent *);
  /** @brief overriden from QGroupBox
   * this will simply make the reply button invisible again when mouse is gone */
  void leaveEvent(QEvent *);
  /** @return The TwStatus object */
  TwStatus* getTweet() { return pTweet; };
  void markRetweeted();
  void markFavorited();
  void markUnFavorited();
  void alert();
protected:
  QGridLayout *layout, *buttonLayout;
  QLabel *sourceText, *userName, *profilePicture;
  QTextBrowser *tweetText;
  QPushButton *replyButton, *retweetButton, *favoriteButton, *extraButton, *deleteButton;
  QPushButton *blockButton, *reportButton;
  TwStatus *pTweet;
public slots:
  void replyButtonClicked();
  void retweetButtonClicked();
  void favoriteButtonClicked();
  void getUserProfile(QString);
  void deleteButtonClicked();
  void blockButtonClicked();
  void reportButtonClicked();
  void getProfilePicture(CString,QPixmap);
  void conversationView(QString);
protected slots:
  void showExtraButtons();
  void adjustTweetText(QSizeF);
  void urlClicked(QUrl);
  void hideButtons();
  void notificationClicked(QString);
signals:
  void sendReply(unsigned long long,CString);
  void sendRetweet(unsigned long long,CString);
  void sendUserProfile(QString);
  void sendAPI(unsigned long long,APItypes);
};

#endif // UI_TWEETBOX_H
