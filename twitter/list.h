#ifndef TWLIST_H
#define TWLIST_H

#include <CString.h>
#include "jsonxx.h"
#include "user.h"
#include <sstream>
#include <string>
#include <iostream>

using namespace jsonxx;
using namespace std;

/**
 * @brief Class that contains info about a list
 */
class TwList {
public:
  /** @brief Constructor
   *  @param List The list to parse
   *  @param user_id The user id of the user, used to determine if it's yours */
  TwList(Object& List, unsigned int user_id);
  TwList();
  /** @brief Get the description from the list */
  CString getDescription(bool pretty = false);
  /** @brief Get the full name of the list */
  CString getFullName() { return fullName; };
  /** @brief Get the url to access the list on web */
  CString getUrl() { return CString("http://twitter.com" + url); };
  /** @brief Get the name of the list */
  CString getName() { return name; }
  /** @brief Get the amount of people on the list */
  unsigned int getMembers() { return members; };
  /** @brief Get the amount of people that follow the list */
  unsigned int getSubscribers() { return subscribers; };
  /** @brief Get the unique id from the list */
  unsigned int getId() { return id; };
  /** @brief Get the user from the tweet */
  TwUser* getUser() { return user; };
protected:
  CString fullName;
  CString name;
  CString description;
  CString url;
  unsigned int id;
  unsigned int members;
  unsigned int subscribers;
  TwUser* user;
};
#endif // !TWLIST_H