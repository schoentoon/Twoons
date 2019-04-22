#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include "OAuth.h"
#include "jsonxx.h"
#include <string>
#include <sstream>
#include <map>
#include <vector>
/* Ui includes */
#include "ui_tweetbox.h"
#include "ui_tweetfield.h"
#include "ui_column.h"
#include "ui_autocomplete.h"
#include "status.h"
#include "ui_errorbox.h"
#include "ui_userprofile.h"
#include "ui_conversationview.h"
#include "ui_settings.h"
#include "ui_dm.h"
#include "datastorage.h"
#include "ui_auth.h"
#include "ui_imageview.h"
#include "stream_thread.h"
#include "picture/framework.h"
#include "notification.h"
/* Qt includes */
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QScrollArea>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>
#include <QtGui/QTextCursor>
#include <QtGui/QInputDialog>
#include <QtGui/QLineEdit>
#include <QNetworkReply>
#include <QDesktopServices>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QProgressBar>
#include <QTextStream>
#include <QTimer>
#include <QFile>
#include <QUrl>

using namespace jsonxx;

/** @brief The main gui */
class Ui_MainWindow : public QWidget
{
  Q_OBJECT
public:
  Ui_MainWindow(QWidget *parent = 0);
  void openConversationView(unsigned long long id, TweetBox *ptr);
  void refreshTheme();
  TwStatus* getTweet(unsigned long long id);
protected:
  void closeEvent(QCloseEvent *event);
  QWidget *scrollAreaWidgetContents;
  QGridLayout *mainLayout;
  QScrollArea *scrollArea_Columns;
  QHBoxLayout *columnViewLayout, *horizontalLayout;
  QPushButton *tweetButton;
  QLabel *lookupLabel, *versionLabel;
  QTextEdit *lookupField;
  QLabel *errorBox;
  vector<Ui_Column*> columns;
  TweetField *tweetField;
  TweetField *tweetFieldWindow;
  QNetworkReply *networkHandle;
  CString in_reply_to;
  QPushButton *settingsButton;
  Ui_UserProfile *userProfile;
  Ui_Autocomplete *autoComplete;
  Ui_Settings *settingsWindow;
  APItypes type;
  bool openweb;
  bool stream;
  Ui_Auth *auth;
  StreamThread *stream_thread;
  CString buffer;
  QTimer *errorTimer;
  QSystemTrayIcon *trayIcon;
  QInputDialog *passwordBox;
  Ui_DirectMessage *directMessages;
  QPushButton *showDMs;
  Ui_ImageView *imageView;
  vector<PictureFramework*> pictureHandlers;
  /* Functions */
  /** @brief Used so it can fields can react to the return key being pressed */
  bool eventFilter(QObject * object, QEvent * event);
private:
  CString aKey;
  CString aSec;
  QString password; /* No I'm not going to be an idiot and store the actual password here.. */
  QFile *config;
public slots:
  void inReplyTo(unsigned long long replyId, CString screen_name);
  void sendRetweet(unsigned long long retweetId, CString tmp);
  void sendError(CString error);
  void writeSettings();
  void getUserProfile(QString);
  void markRetweeted(unsigned long long);
  void sendAPI(unsigned long long,APItypes);
  void getKeys(CString,CString);
  void receiveData(CString);
  void onRequestCompleted(QNetworkReply *reply);
  void needBackfill();
  void getNewColumns();
  void iconActivated(QSystemTrayIcon::ActivationReason);
  void setPassword(QString);
  void trayIconClicked();
  void passwordAccepted();
  void newDM();
  void toggleDMs();
  void openUrl(QUrl);
  void initNotif();
  void openDM(CString,unsigned int);
signals:
  void error(QString);
  void openProfile(unsigned long);
  void openProfile(CString);
  void markRetweetedCol(unsigned long long);
};

#endif // UI_MAINWINDOW_H
