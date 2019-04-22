#ifndef UI_TWEETFIELD_H
#define UI_TWEETFIELD_H

#include "OAuth.h"
#include "OAuthUtils.h"
#include "ui_errorbox.h"
#include "datastorage.h"
#include "status.h"
#include "upframework.h"
/* Qt includes */
#include <QtGui/QPushButton>
#include <QtGui/QHBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QProgressBar>
#include <QtGui/QFileDialog>
#include <QNetworkAccessManager>
#include <QKeyEvent>
#include <QNetworkReply>
#include <QString>
#include <QLabel>
#include <QEvent>
#include <iostream>

using namespace std;
/** @brief TweetField class
 * @section description This class is pretty much the field to tweet from */
class TweetField: public QGroupBox
{
  Q_OBJECT
public:
  /** @brief Constructor */
  TweetField(QWidget *parent = 0);
  QPlainTextEdit* getField() { return field; };
  QPushButton* getRetweetButton() { return retweetButton; };
  QPushButton* getEditRetweetButton() { return editRetweet; };
  QPushButton* getSendButton() { return sendButton; };
  bool eventFilter(QObject * object, QEvent * event);
  void setInReplyTo(unsigned long long tmp) { in_reply_to = tmp; };
  void setRetweet(unsigned long long tmp) { in_reply_to = tmp; };
  void hideAfterTweet(bool close) { this->close = close; };
  void quitAfterTweet(bool quit) { this->quit = quit; };
  bool isSending() { return !field->isEnabled(); }
protected:
  QGridLayout *layout;
  QPlainTextEdit *field;
  QPushButton *sendButton;
  QHBoxLayout *retweetButtons;
  QPushButton *retweetButton, *editRetweet, *cancelTweet;
  QLabel *characterCounter;
  QNetworkReply *networkHandle;
  QLabel *picturePreview;
  QPushButton *pictureCancel;
  QProgressBar *progress;
  QFile *picture;
  QPushButton *selectFile;
  QFileDialog *fileDialog;
  unsigned long long in_reply_to;
  QNetworkAccessManager *network;
  vector<PictureUpFramework*> uploaders;
  bool uploaded;
  bool close;
  bool quit;
  void Tweet(CString msg, unsigned long long in_reply_to);
  void Retweet(unsigned long long id);
  void Error(CString);
protected slots:
  void sendTweet();
  void sendRetweet();
  void updateCounter();
  void clearRetweet();
  void cleanUp();
  void cancelTwet();
  void selectedFile(QString);
  void uploadProgress(qint64,qint64);
public slots:
  void getNickname(CString);
  void tweetButtonClicked(bool);
  void onRequestCompleted(QNetworkReply *reply);
  void resizeField(int);
  void appendUrl(QString);
  void focus();
signals:
  void callAutocomplete();
  void retweeted(unsigned long long);
  void sendError(CString);
};

#endif // UI_TWEETFIELD_H
