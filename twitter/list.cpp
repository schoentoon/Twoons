#include "list.h"

TwList::TwList(Object& List, unsigned int user_id)
{
  if (List.has<string>("full_name"))
    fullName = List.get<string>("full_name");
  if (List.has<string>("name"))
    name = List.get<string>("name");
  if (List.has<string>("description")) {
    description = List.get<string>("description");
  }
  if (List.has<string>("id_str"))
    id = CString(List.get<string>("id_str")).ToUInt();
  if (List.has<string>("uri"))
    url = List.get<string>("uri");
  if (List.has<double>("member_count"))
    members = List.get<double>("member_count");
  if (List.has<double>("subscriber_count"))
    subscribers = List.get<double>("subscriber_count");
  if (List.has<Object>("user"))
    user = new TwUser(List.get<Object>("user"), user_id);
}

TwList::TwList()
{
}

CString TwList::getDescription(bool pretty)
{
  if (pretty)
    return CString::ConvertUnicode(description);
  return description;
}