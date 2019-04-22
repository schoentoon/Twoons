#ifndef TWUSER_H
#define TWUSER_H

#include <CString.h>
#include <string.h>
#include "jsonxx.h"
#include <sstream>
#include <string>

using namespace jsonxx;
using namespace std;

class TwUser;
/** @brief Class that contains info about a user */
class TwUser {
public:
  /** @brief Constructor
   * @param User The user to parse 
   * @param me The user id of you, used to determine if it's you */
  TwUser(Object& User, unsigned long me);
  /** @brief Get the screen name from the user
   * @return The screen name of the user */
  CString getName() { return screen_name; };
  /** @brief Get the description from the user
   * @param pretty If this is set to true I'll make the unicode visible
   * @return The description of the user */
  CString getDescription(bool pretty = false);
  /** @brief Get the id from the user
   * @return The id of the user */
  unsigned long getId() { return id; };
  /** @brief Get the amount of people that follow this user
   * @return Amount of followers */
  unsigned int getFollowers() { return followers; };
  /** @brief Get the amount of people that this user follows
   * @return Amount of following */
  unsigned int getFollowing() { return friends; };
  /** @brief Get the amount of tweets that this users favorited
   * @return Amount of favorites */
  unsigned int getFavorites() { return favorites; };
  /** @brief Get the amount of tweets placed by this user
   * @return Amount of tweets */
  unsigned int getTweets() { return tweets; };
  /** @brief See if this user is protected or not
   * @return True if protected */
  bool getProtected() { return locked; };
  /** @brief See if this user is verified or not
   * @return True if verified */
  bool getVerified() { return verified; };
  /** @brief See if this is you */
  bool isMe() { return me; };
  CString getProfilePicture() { return profilePicture; };
protected:
  CString screen_name;
  CString description;
  CString profilePicture;
  unsigned long id;
  unsigned int  followers;
  unsigned int  friends;
  unsigned int  favorites;
  unsigned int  tweets;
  bool    locked;
  bool    verified;
  bool    me;
};
#endif // !TWUSER_H
