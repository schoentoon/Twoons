#include "user.h"

TwUser::TwUser(Object& User, unsigned long user_id)
{
  if (User.has<string>("screen_name"))
    screen_name = User.get<string>("screen_name");
  if (User.has<string>("description"))
    description = CString(User.get<string>("description")).Replace_n("&amp;", "&")
                         .Replace_n("&lt;", "<").Replace_n("&gt;", ">")
                         .Replace_n("&qout;", "\"");
  if (User.has<double>("id")) {
    id = User.get<double>("id");
    if (id == user_id)
      me = true;
    else
      me = false;
  } else
    me = false;
  if (User.has<double>("followers_count"))
    followers = User.get<double>("followers_count");
  else
    followers = 0;
  if (User.has<double>("friends_count"))
    friends = User.get<double>("friends_count");
  else
    friends = 0;
  if (User.has<double>("favourites_count"))
    favorites = User.get<double>("favourites_count");
  else
    favorites = 0;
  if (User.has<double>("statuses_count"))
    tweets = User.get<double>("statuses_count");
  else
    tweets = 0;
  if (User.has<bool>("protected"))
    locked = User.get<bool>("protected");
  else
    locked = false;
  if (User.has<bool>("verified"))
    verified = User.get<bool>("verified");
  else
    verified = false;
  if (User.has<string>("profile_image_url"))
   profilePicture = User.get<string>("profile_image_url"); 
}

CString TwUser::getDescription(bool pretty)
{
  if (pretty)
    return CString::ConvertUnicode(description);
  return description;
}
