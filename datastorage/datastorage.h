#ifndef DATASTORAGE_H
#define DATASTORAGE_H

class dataStorage; /* Else it refuses to compile */

#include <iostream>
#include <vector>
#include <time.h>
#include "user_store.h"
#include "OAuth.h"
/* Qt includes */
#include <QIcon>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmapCache>

#ifndef VERSION
#  warning "No version set."
#  define VERSION ""
#endif
#ifdef linux
#  define OS "Linux"
#else
#  define OS "Windows"
#endif

using namespace std;
/** @brief Datastorage class
 * @section description Used to globally access certain stuff */
class dataStorage : public QObject
{
  Q_OBJECT
public:
  dataStorage();
  typedef enum {
    Reply     = 0,
    Retweet   = 1,
    Retweeted = 2,
    Favorite  = 3,
    Favorited = 4,
    Delete    = 5,
    Block     = 6,
    Report    = 7
  } Icons;
  QIcon* getIcon(Icons i);
  /** @brief This method allows us to add a new user object to our store */
  void addUser(StoreUser* user);
  VCString queryUserDB(CString query);
  QNetworkAccessManager* getNetworkManager() { return network; };
  void setOAuth(OAuth *user);
  OAuth* getOAuth() { return user; };
  void addFriend(unsigned long user_id) { friends.push_back(user_id); };
  void removeFriend(unsigned long long user_id);
  bool isFriend(unsigned long long);
  bool isBlocked(unsigned long long);
  bool wantsRetweetsFrom(unsigned long long);
  void clearFriends();
  void getProfilePicture(CString url);
  void killAllRequests();
  CString printSettings();
  void importSettings(Object&);
  void addBlock(unsigned long long);
  VCString getUserFilters() { return userFilter; };
  VCString getSourceFilters() { return sourceFilter; };
  VCString getWordFilters() { return wordFilter; };
  unsigned int getUploadingService() { return currentUploadingService; };
  void setUploadingService(unsigned int service) { currentUploadingService = service; };
protected:
  QIcon *replyIcon, *retweetIcon, *retweetOnIcon, *favoriteIcon, *favoriteOnIcon, *deleteIcon, *blockIcon, *spamIcon;
  VCString userFilter, wordFilter, sourceFilter;
  vector<StoreUser*> users;
  QNetworkAccessManager *network;
  QNetworkAccessManager *profilePictureNetwork;
  OAuth *user;
  vector<unsigned long long> friends;
  vector<unsigned long long> blocked;
  vector<unsigned long long> no_retweets;
  vector<QNetworkReply*> profilePictureReplies;
  QNetworkReply* rBlocked;
  QNetworkReply* rNo_retweets;
  QTimer *infoRequestTimer;
  unsigned int lastRequest;
  unsigned int currentUploadingService;
public slots:
  void setFilters(CString field, VCString info);
  void requestInfo();
  void onIgnoreSSLErrors(QNetworkReply* reply, QList<QSslError>);
signals:
  void sendProfilePicture(CString,QPixmap);
  void newGlobalFilters();
  void saveSettings();
private slots:
  void onRequestCompleted(QNetworkReply *reply);
  void info(QNetworkReply *reply);
};

#endif // DATASTORAGE_H
