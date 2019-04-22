#include "status.h"
#include "global.h"

TwStatus::TwStatus(Object& Tweet)
{
  if (Tweet.has<Object>("retweeted_status"))
    retweeted_status = new TwStatus(Tweet.get<Object>("retweeted_status"));
  else {
    if (Tweet.has<string>("text"))
      tweet = CString(Tweet.get<string>("text"));
    retweeted_status = NULL;
  }
  if (Tweet.has<string>("source"))
    source = Tweet.get<string>("source");
  if (Tweet.has<string>("created_at")) {
    created_at = QDateTime::fromString(CString(Tweet.get<string>("created_at")).Replace_n("+0000","").c_str());
    created_at.setTimeSpec(Qt::UTC);
    created_at = created_at.toTimeSpec(Qt::LocalTime);
  }
  if (Tweet.has<string>("id_str"))
    id = CString(Tweet.get<string>("id_str")).ToULongLong();
  if (Tweet.has<string>("in_reply_to_status_id_str"))
    in_reply_to = CString(Tweet.get<string>("in_reply_to_status_id_str")).ToULongLong();
  else
    in_reply_to = 0;
  if (Tweet.has<double>("in_reply_to_user_id"))
    in_reply_to_user = (unsigned long long) Tweet.get<double>("in_reply_to_user_id");
  else
    in_reply_to_user = 0;
  if (Tweet.has<Object>("user"))
    user = new TwUser(Tweet.get<Object>("user"), Global::dataStore()->getOAuth()->getUserId());
  if (Tweet.has<bool>("favorited"))
    favorited = Tweet.get<bool>("favorited");
  else
    favorited = false;
  if (Tweet.has<bool>("retweeted"))
    retweeted = Tweet.get<bool>("retweeted");
  else
    retweeted = false;
  if (Tweet.has<Object>("entities")) {
    if (Tweet.get<Object>("entities").has<Array>("user_mentions")) {
      Array& tmp = Tweet.get<Object>("entities").get<Array>("user_mentions");
      for (unsigned int i = 0; i < tmp.size(); i++) {
        if (tmp.get<Object>(i).has<double>("id") and tmp.get<Object>(i).has<string>("screen_name"))
          users[tmp.get<Object>(i).get<double>("id")] = tmp.get<Object>(i).get<string>("screen_name");
      }
    }
    if (Tweet.get<Object>("entities").has<Array>("urls")) {
      Array& tmp = Tweet.get<Object>("entities").get<Array>("urls");
      for (unsigned int i = 0; i < tmp.size(); i++) {
        if (tmp.has<Object>(i))
          urls.push_back(url(tmp.get<Object>(i)));
      }
    }
    if (Tweet.get<Object>("entities").has<Array>("media")) {
      Array& tmp = Tweet.get<Object>("entities").get<Array>("media");
      for (unsigned int i = 0; i < tmp.size(); i++) {
        if (tmp.has<Object>(i))
          urls.push_back(url(tmp.get<Object>(i)));
      }
    }
  }
  if (retweeted_status) {
    if (users.count(retweeted_status->getUser()->getId()) < 1)
      users[retweeted_status->getUser()->getId()] = retweeted_status->getUser()->getName();
  }
}

TwStatus::TwStatus(const TwStatus& t)
{
  this->tweet = t.tweet;
  this->source = t.source;
  this->created_at = t.created_at;
  this->id = t.id;
  this->in_reply_to = t.in_reply_to;
  this->in_reply_to_user = t.in_reply_to_user;
  this->favorited = t.favorited;
  this->retweeted = t.retweeted;
  this->users = t.users;
  this->urls = t.urls;
  this->user = new TwUser(*t.user);
  if (t.retweeted_status)
    this->retweeted_status = new TwStatus(*t.retweeted_status);
  else
    this->retweeted_status = NULL;
}

TwStatus::~TwStatus()
{
  if (user)
    delete user;
  if (retweeted_status)
    delete retweeted_status;
}

CString TwStatus::replyAll()
{
  CString tmp = "@" + user->getName() + " ";
  for (map<unsigned long,CString>::const_iterator iter = users.begin();
      iter != users.end(); ++iter) {
    if (iter->first != Global::dataStore()->getOAuth()->getUserId()
        and !tmp.Token(0).LeftChomp_n(1).Equals(iter->second))
      tmp = tmp + "@" + iter->second + " ";
  }
  return tmp;
}

CString TwStatus::getText(bool pretty)
{
  if (pretty) {
    QString qtmp = tweet.c_str();
    for (vector<url>::iterator iter = urls.begin(); iter != urls.end(); ++iter) {
      if (iter->media_url.size() > 0)
        qtmp.replace(iter->short_url.c_str(), CString("<a href=\"" + iter->expanded_url + "?media_url=" + iter->media_url + "\">" + iter->display_url + "</a>").c_str());
      else
        qtmp.replace(iter->short_url.c_str(), CString("<a href=\"" + iter->expanded_url + "\">" + iter->display_url + "</a>").c_str());
    }
    for (map<unsigned long,CString>::const_iterator iter = users.begin(); iter != users.end(); ++iter)
      qtmp.replace(QRegExp(CString("@(" + iter->second + ")\\b").c_str(), Qt::CaseInsensitive),
                   QString(CString("@<a href=\"http://twitter.com/" + iter->second + "?user_id=" + CString(iter->first).c_str() + "\">\\1</a>").c_str()));
    return "<div>" + CString::ConvertUnicode(qtmp.toLatin1().data(),true).Replace_n("\n","") + "</div>";
  } else
    return tweet;
}

CString TwStatus::getSource(bool urls)
{
  if (urls)
    return CString::ConvertUnicode(source,false).Replace_n("\\u003C","<").Replace_n("\\u003E",">")
           .Replace_n("&lt;","<").Replace_n("&gt;",">").Replace_n("&quot;","\"")
           .Replace_n("&#60;","<").Replace_n("&#62;",">");
  else {
    if (source.WildCmp("\\u003C*\\u003E")) {
      CString tmp = source;
      return tmp.Token(1,false,"\\u003E").Token(0,false,"\\u003C");
    } else
      return source;
  }
}

CString TwStatus::getInReplyToUser()
{
  for (map<unsigned long,CString>::const_iterator iter = users.begin();
      iter != users.end(); ++iter) {
    if (iter->first == in_reply_to_user)
      return iter->second;
  }
  return "";
}
