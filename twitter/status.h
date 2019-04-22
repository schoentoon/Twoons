#ifndef TWSTATUS_H
#define TWSTATUS_H

#include <CString.h>
#include <string.h>
#include "jsonxx.h"
#include "user.h"
#include <sstream>
#include <string>
#include <time.h>
#include <vector>
#include <map>
#include <iostream>
/* Qt includes */
#include <QDateTime>
#include <QStringList>
#include <QRegExp>

using namespace jsonxx;
using namespace std;

struct url {
  url(Object &json) {
    if (json.has<string>("url"))
      short_url = json.get<string>("url");
    else
      short_url = "";
    if (json.has<string>("display_url"))
      display_url = json.get<string>("display_url");
    else
      display_url = short_url;
    if (json.has<string>("expanded_url"))
      expanded_url = json.get<string>("expanded_url");
    else
      expanded_url = short_url;
    if (json.has<string>("media_url"))
      media_url = json.get<string>("media_url");
  }
  CString display_url;
  CString short_url;
  CString expanded_url;
  CString media_url;
};

class TwStatus;
/** @brief Class that contains info of a tweet, also know as Status */
class TwStatus {
public:
  /** @brief Constructor
   * @param Tweet The tweet to parse
   * @return The TwStatus object */
  TwStatus(Object& Tweet);
  TwStatus(const TwStatus& t);
  ~TwStatus();
  /** @brief Get the text from the tweet
   * @return The text of the tweet */
  CString getText(bool pretty = false);
  /** @brief Get the source from the tweet
   * @return The source of the tweet */
  CString getSource(bool urls = true);
  /** @brief Get the unique id from the tweet
   * @return The unique id of the tweet */
  unsigned long long getId() { return id; };
  /** @brief Get the id of the tweet this is a reply to
   * @return The id of the tweet this is a in reply to */
  unsigned long long getInReplyTo() { return in_reply_to; };
  /** @brief Get the user id the reply was made to */
  unsigned long long getInReplyToUserId() { return in_reply_to_user; };
  /** @brief Get the name of the user the reply was made to */
  CString getInReplyToUser();
  /** @brief Get the create date of the tweet
   * @return The create date of the tweet */
  QDateTime getCreated_at() { return created_at; };
  /** @brief Get the user from the tweet
   * @return The user from the tweet */
  TwUser* getUser() { return user; };
  /** @brief Get the output for in the replybox
   * @return All the nicknames associated with this tweet */
  CString replyAll();
  /** @brief Return the tweet object that got retweeted
   * Be sure to check if it's a retweet with isRetweet() first! */
  TwStatus* getRetweet() { return retweeted_status; };
  /** @brief Used to check if it's a retweet or not
   * @return True if it's a retweet */
  bool isRetweet() { return retweeted_status; };
  /** @brief Used to check if this tweet is favorited or not
   * @return True if favorited */
  bool isFavorited() { return favorited; };
  /** @brief Used to check if this tweet is retweeted or not
   * @return True if retweeted */
  bool isRetweeted() { return retweeted; };
  /** @brief Used to check if this tweet is mine */
  bool isMe() { return user->isMe(); };
  /** @brief Used to mark the tweet as favorited during runtime */
  void setFavorite(bool tmp) { favorited = tmp; };
  /** @brief Used to mark the tweet as retweeted during runtime */
  void setRetweeted(bool tmp) { retweeted = tmp; };
protected:
  CString tweet;
  CString source;
  QDateTime created_at;
  TwUser* user;
  unsigned long long id;
  unsigned long long in_reply_to;
  unsigned long in_reply_to_user;
  TwStatus* retweeted_status;
  bool favorited;
  bool retweeted;
  map<unsigned long,CString> users;
  vector<url> urls;
};
#endif // !TWSTATUS_H
