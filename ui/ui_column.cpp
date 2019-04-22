#include "ui_column.h"
#include "moc_ui_column.h"
#include "global.h"

Ui_Column::Ui_Column(QString columnName, QWidget *parent)
: QGroupBox(parent)
{
  setObjectName("column");
  limit = 200;
  alert = false;
  allow_retweets = true;
  QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
  setSizePolicy(sizePolicy);
  setContentsMargins(0,0,0,0);
  setMinimumWidth(290);
  setMaximumWidth(290);
  titleLabel = new QLabel(columnName,this);
  columnLayout = new QGridLayout(this);
  columnLayout->addWidget(titleLabel, 0, 0);
  scrollArea_Column = new QScrollArea(this);
  scrollArea_Column->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  scrollArea_Column->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollArea_Column->setWidgetResizable(true);
  scrollArea_Column->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
  scrollAreaWidgetContents_Column = new QWidget();
  scrollAreaWidgetContents_Column->setMinimumWidth(250);
  scrollAreaWidgetContents_Column->setLayoutDirection(Qt::LeftToRight);
  scrollArea_Column->setWidget(scrollAreaWidgetContents_Column);
  tweetsLayout = new QVBoxLayout(scrollAreaWidgetContents_Column);
  tweetsLayout->setContentsMargins(0,0,0,0);
  tweetsLayout->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
  columnLayout->addWidget(scrollArea_Column, 1, 0);

  timer = new QTimer(this);
  network = new QNetworkAccessManager();
  networkHandle = NULL;
  QObject::connect(timer, SIGNAL(timeout())
                  ,this, SLOT(sendRequest()));
  QObject::connect(network, SIGNAL(finished(QNetworkReply*))
                  ,this, SLOT(onRequestCompleted(QNetworkReply*)));
  QObject::connect(network, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>))
                  ,Global::dataStore(), SLOT(onIgnoreSSLErrors(QNetworkReply*,QList<QSslError>)));
}

Ui_Column::~Ui_Column()
{
  delete tweetsLayout;
  delete scrollArea_Column;
  delete columnLayout;
  delete titleLabel;
  network->deleteLater();
  delete timer;
}

void Ui_Column::Tweet(TwStatus *tweet)
{
  if (tweet->isRetweet() and !allow_retweets)
    return;
  if (tweet->isRetweet()) {
    if (!Global::dataStore()->wantsRetweetsFrom(tweet->getUser()->getId()))
      return;
    if (Global::dataStore()->isBlocked(tweet->getRetweet()->getUser()->getId()))
      return;
  }
  if (Global::dataStore()->isBlocked(tweet->getUser()->getId()))
    return;
  TweetBox* tmp;
  for (unsigned int i = 0; i < (unsigned) tweetsLayout->count(); i++) {
    QLayoutItem* item = tweetsLayout->itemAt(i);
    if (item->widget()->objectName() == "tweet") {
      tmp = static_cast<TweetBox*>(item->widget());
      if (tmp->getTweet()->getId() == tweet->getId())
        return;
    }
  }
  if ((unsigned) tweetsLayout->count() >= limit) {
    QLayoutItem* item = tweetsLayout->takeAt(limit - 1);
    delete item->widget();
    delete item;
  }
  tmp = new TweetBox(tweet);
  if (isFiltered(userFilter, tweet->getUser()->getName(), false)
   or isFiltered(wordFilter, tweet->getText())
   or isFiltered(sourceFilter, tweet->getSource(false), false)
   or isFiltered(Global::dataStore()->getUserFilters(), tweet->getUser()->getName(), false)
   or isFiltered(Global::dataStore()->getWordFilters(), tweet->getText())
   or isFiltered(Global::dataStore()->getSourceFilters(), tweet->getSource(false), false))
    tmp->setVisible(false);
  else if (tweet->isRetweet()) {
    if (isFiltered(userFilter, tweet->getRetweet()->getUser()->getName(), false)
     or isFiltered(wordFilter, tweet->getRetweet()->getText())
     or isFiltered(sourceFilter, tweet->getRetweet()->getSource(false), false)
     or isFiltered(Global::dataStore()->getUserFilters(), tweet->getRetweet()->getUser()->getName(), false)
     or isFiltered(Global::dataStore()->getWordFilters(), tweet->getRetweet()->getText())
     or isFiltered(Global::dataStore()->getSourceFilters(), tweet->getRetweet()->getSource(false), false))
      tmp->setVisible(false);
  } else if (alert > 0) {
    switch (alert) {
    case Blink:
      QApplication::alert(this, 0);
      break;
    case LibNotify:
      tmp->alert();
      break;
    }
  }
  tweetsLayout->insertWidget(0, tmp);
}

void Ui_Column::User(TwUser *user)
{
  UserBox* tmp;
  for (unsigned int i = 0; i < (unsigned) tweetsLayout->count(); i++) {
    QLayoutItem* item = tweetsLayout->itemAt(i);
    if (item->widget()->objectName() == "user") {
      tmp = dynamic_cast<UserBox*>(item->widget());
      if (tmp and tmp->getUser()->getId() == user->getId())
        return;
    }
  }
  tmp = new UserBox(user);
  tweetsLayout->insertWidget(0, tmp);
  if (alert)
    QApplication::alert(this, 0);
}

void Ui_Column::deleteTweet(unsigned long long id)
{
  TweetBox* tmp;
  for (int i = 0; i < tweetsLayout->count(); i++) {
    QLayoutItem* item = tweetsLayout->itemAt(i);
    if (item->widget()->objectName() == "tweet") {
      tmp = dynamic_cast<TweetBox*>(item->widget());
      if (tmp and tmp->getTweet()->getId() == id) {
        delete item->widget();
        i--;
      }
    }
  }
}

void Ui_Column::deleteTweetsUser(unsigned long id)
{
  TweetBox* tmp;
  for (int i = 0; i < tweetsLayout->count(); i++) {
    QLayoutItem* item = tweetsLayout->itemAt(i);
    if (item->widget()->objectName() == "tweet") {
      tmp = dynamic_cast<TweetBox*>(item->widget());
      if (tmp and tmp->getTweet()->getUser()->getId() == id) {
        delete item->widget();
        i--;
      }
    }
  }
}

void Ui_Column::deleteUser(unsigned long id)
{
  UserBox* tmp;
  for (int i = 0; i < tweetsLayout->count(); i++) {
    QLayoutItem* item = tweetsLayout->itemAt(i);
    if (item->widget()->objectName() == "user") {
      tmp = dynamic_cast<UserBox*>(item->widget());
      if (tmp and tmp->getUser()->getId() == id) {
        delete item->widget();
        i--;
      }
    }
  }
}

bool Ui_Column::isFiltered(VCString filter, CString rule, bool contains)
{
  for (VCString::size_type i = 0; i < filter.size(); i++) {
    if (contains) {
      if (rule.AsLower().WildCmp("*" + filter[i].AsLower() + "*"))
        return true;
    } else if (filter[i].find("*") != string::npos) {
      if (rule.AsLower().WildCmp(filter[i].AsLower()))
        return true;
    } else if (filter[i].Equals(rule))
        return true;
  }
  return false;
}

void Ui_Column::setFilters(CString field, VCString info)
{
  if (field.Equals("user"))
    userFilter = info;
  else if (field.Equals("source"))
    sourceFilter = info;
  else if (field.Equals("text"))
    wordFilter = info;
  TweetBox* tmp;
  for (int i = 0; i < tweetsLayout->count(); i++) {
    QLayoutItem* item = tweetsLayout->itemAt(i);
    if (item->widget()->objectName() == "tweet") {
      tmp = static_cast<TweetBox*>(item->widget());
      if (isFiltered(userFilter, tmp->getTweet()->getUser()->getName(), false)
       or isFiltered(wordFilter, tmp->getTweet()->getText())
       or isFiltered(sourceFilter, tmp->getTweet()->getSource(false), false)
       or isFiltered(Global::dataStore()->getUserFilters(), tmp->getTweet()->getUser()->getName(), false)
       or isFiltered(Global::dataStore()->getWordFilters(), tmp->getTweet()->getText())
       or isFiltered(Global::dataStore()->getSourceFilters(), tmp->getTweet()->getSource(false), false))
        tmp->setVisible(false);
      else if (tmp->getTweet()->isRetweet()) {
        if (isFiltered(userFilter, tmp->getTweet()->getRetweet()->getUser()->getName(), false)
         or isFiltered(wordFilter, tmp->getTweet()->getRetweet()->getText())
         or isFiltered(sourceFilter, tmp->getTweet()->getRetweet()->getSource(false), false)
         or isFiltered(Global::dataStore()->getUserFilters(), tmp->getTweet()->getRetweet()->getUser()->getName(), false)
         or isFiltered(Global::dataStore()->getWordFilters(), tmp->getTweet()->getRetweet()->getText())
         or isFiltered(Global::dataStore()->getSourceFilters(), tmp->getTweet()->getRetweet()->getSource(false), false))
          tmp->setVisible(false);
      } else
        tmp->setVisible(true);
    }
  }
}

void Ui_Column::importSettings(CString plainText)
{
  istringstream input(plainText);
  Object o;
  Object::parse(input, o);
  importSettings(o);
}

void Ui_Column::importSettings(Object& settings)
{
  if (settings.has<Object>("filters")) {
    if (settings.get<Object>("filters").has<Array>("username")) {
      for (unsigned int i = 0; i < settings.get<Object>("filters").get<Array>("username").size(); i++)
        addUserFilter(settings.get<Object>("filters").get<Array>("username").get<string>(i));
    }
    if (settings.get<Object>("filters").has<Array>("source")) {
      for (unsigned int i = 0; i < settings.get<Object>("filters").get<Array>("source").size(); i++)
        addSourceFilter(settings.get<Object>("filters").get<Array>("source").get<string>(i));
    }
    if (settings.get<Object>("filters").has<Array>("text")) {
      for (unsigned int i = 0; i < settings.get<Object>("filters").get<Array>("text").size(); i++)
        addWordFilter(settings.get<Object>("filters").get<Array>("text").get<string>(i));
    }
  }
  if (settings.has<double>("width"))
    resize(settings.get<double>("width"));
  if (settings.has<double>("limit"))
    limit = settings.get<double>("limit");
  if (settings.has<bool>("alert"))
    alert = settings.get<bool>("alert") ? 1 : 0;
  else if (settings.has<double>("alert"))
    alert = (unsigned int) settings.get<double>("alert");
  if (settings.has<double>("interval")) {
    if (settings.get<double>("interval") >= 10) {
      timer->setInterval(settings.get<double>("interval")*1000);
      interval = settings.get<double>("interval");
    } else {
      timer->setInterval(30000);
      interval = 30;
    }
  } else {
    timer->setInterval(30000);
    interval = 30;
  }
  if (settings.has<bool>("allow_retweets"))
    allow_retweets = settings.get<bool>("allow_retweets");
  if (settings.has<double>("type"))
    setColumnType(settings.get<double>("type"));
  if (settings.has<double>("id"))
    setList(CString(settings.get<double>("id"),0));
  else if (settings.has<string>("id"))
    setList(settings.get<string>("id"));
  if (type != Trending)
    setStream(Global::dataStore()->getOAuth()->isStreaming());
  else
    setStream(false);
  sendRequest();
}

void Ui_Column::setStream(bool streaming)
{
  if (streaming and type == Trending)
    stream = false;
  else
    stream = streaming;
  if (!stream)
    timer->start();
  else
    timer->stop();
}

void Ui_Column::setList(CString id)
{
  listId = id;
  map<CString,CString> params;
  params["list_id"] = listId;
  params["cursor"] = "-1";
  params["skip_status"] = "t";
  params["include_entities"] = "f";
  listMembersCall = network->get(Global::dataStore()->getOAuth()->APICall("http://api.twitter.com/1.1/lists/members.json", params, "GET"));
}

void Ui_Column::setColumnType(unsigned int i)
{
  switch(i) {
    case 0:
      type = HomeTimeline;
      break;
    case 1:
      type = Mentions;
      break;
    case 2:
      type = List;
      break;
    case 3:
      type = Trending;
      scrollArea_Column->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      tweetsLayout->setAlignment(0);
      break;
    case 4:
      type = Followers;
      break;
  }
}

CString Ui_Column::printSettings()
{
  CString tmp = "{\"filters\":{\"username\":[";
  for (VCString::size_type i = 0; i < userFilter.size(); i++)
    tmp = tmp + "\"" + userFilter[i] + "\",";
  if (userFilter.size() > 0)
    tmp.RightChomp(1);
  tmp = tmp + "],\"source\":[";
  for (VCString::size_type i = 0; i < sourceFilter.size(); i++)
    tmp = tmp + "\"" + sourceFilter[i] + "\",";
  if (sourceFilter.size() > 0)
    tmp.RightChomp(1);
  tmp = tmp + "],\"text\":[";
  for (VCString::size_type i = 0; i < wordFilter.size(); i++)
    tmp = tmp + "\"" + wordFilter[i] + "\",";
  if (wordFilter.size() > 0)
    tmp.RightChomp(1);
  tmp = tmp + "]}";
  tmp = tmp + ",\"limit\":" + CString(limit);
  tmp = tmp + ",\"alert\":" + CString(alert);
  tmp = tmp + ",\"interval\":" + CString(interval);
  tmp = tmp + ",\"type\":" + CString(type);
  if (listId.size() > 0)
    tmp = tmp + ",\"id\":" + listId;
  tmp = tmp + ",\"width\":" + CString(minimumWidth());
  tmp = tmp + ",\"name\":\"" + CString(titleLabel->text().toStdString()).Escape_n(CString::ESQL) + "\"";
  tmp = tmp + ",\"allow_retweets\":" + (allow_retweets ? "true" : "false") + "}";
  return tmp;
}

void Ui_Column::markRetweeted(unsigned long long tmp)
{
  TweetBox* twet;
  for (int i = 0; i < tweetsLayout->count(); i++) {
    QLayoutItem* item = tweetsLayout->itemAt(i);
    if (item->widget()->objectName() == "tweet") {
      twet = static_cast<TweetBox*>(item->widget());
      if (twet->getTweet()->getId() == tmp)
        twet->markRetweeted();
    }
  }
}

void Ui_Column::fromMainWindow(Object& data, APItypes APItype)
{
  if (type == Trending)
    return;
  TweetBox* tmp;
  switch (APItype) {
    case Favorite:
      if (data.has<string>("id_str")) {
        unsigned long long id = CString(data.get<string>("id_str")).ToULongLong();
        for (int i = 0; i < tweetsLayout->count(); i++) {
          QLayoutItem* item = tweetsLayout->itemAt(i);
          tmp = dynamic_cast<TweetBox*>(item->widget());
          if (tmp and tmp->getTweet()->getId() == id) {
            tmp->markFavorited();
            break;
          }
        }
      }
      break;
    case UnFavorite:
      if (data.has<string>("id_str")) {
        unsigned long long id = CString(data.get<string>("id_str")).ToULongLong();
        for (int i = 0; i < tweetsLayout->count(); i++) {
          QLayoutItem* item = tweetsLayout->itemAt(i);
          tmp = dynamic_cast<TweetBox*>(item->widget());
          if (tmp and tmp->getTweet()->getId() == id) {
            tmp->markUnFavorited();
            break;
          }
        }
      }
      break;
    case Delete:
      if (data.has<string>("id_str")) {
        unsigned long long id = CString(data.get<string>("id_str")).ToULong();
        deleteTweet(id);
      }
      break;
    case Block:
      if (data.has<string>("id_str")) {
        unsigned long id = CString(data.get<string>("id_str")).ToULong();
        deleteTweetsUser(id);
        deleteUser(id);
      }
      break;
    case Report:
      if (data.has<string>("id_str")) {
        unsigned long id = CString(data.get<string>("id_str")).ToULong();
        deleteTweetsUser(id);
        deleteUser(id);
      }
      break;
    case UnFollow:
      if (data.has<string>("id_str")) {
        unsigned long id = CString(data.get<string>("id_str")).ToULong();
        deleteTweetsUser(id);
        deleteUser(id);
      }
      break;
    default:
      break;
  }
}

void Ui_Column::receiveData(Object& data)
{
  if (type == Trending)
    return;
  if (data.has<string>("text")) {
    if (!stream or (!allow_retweets and !data.has<Object>("retweeted_status")))
      return;
    switch (type) {
      case HomeTimeline:
        if (data.has<Object>("user")) {
          if (data.get<Object>("user").has<double>("id")) {
            if (Global::dataStore()->isFriend(data.get<Object>("user").get<double>("id"))) {
              Tweet(new TwStatus(data));
              if (CString(data.get<string>("id_str")).ToDouble() > since_id.ToDouble())
                since_id = CString(data.get<string>("id_str"));
            }
          }
        }
        break;
      case Mentions:
        if (data.has<Object>("entities")) {
          if (data.get<Object>("entities").has<Array>("user_mentions")) {
            for (unsigned int i = 0;
            i < data.get<Object>("entities").get<Array>("user_mentions").size();
            i++) {
              if (data.get<Object>("entities").get<Array>("user_mentions").has<Object>(i)) {
                if (data.get<Object>("entities").get<Array>("user_mentions").get<Object>(i).has<string>("id_str")) {
                  if (CString(data.get<Object>("entities").get<Array>("user_mentions").get<Object>(i).get<string>("id_str")).
                    Equals(CString(user_id))) {
                    Tweet(new TwStatus(data));
                    if (CString(data.get<string>("id_str")).ToDouble() > since_id.ToDouble())
                      since_id = CString(data.get<string>("id_str"));
                  }
                }
              }
            }
          }
        }
        break;
      case List:
        if (data.has<Object>("user")) {
          if (data.get<Object>("user").has<double>("id")) {
            for (vector<double>::size_type i = 0; i < listMembers.size(); i++) {
              if (((unsigned long) data.get<Object>("user").get<double>("id")) == listMembers[i]) {
                if (data.has<double>("in_reply_to_user_id")) {
                  for (vector<double>::size_type j = 0; j < listMembers.size(); j++) {
                    if (((unsigned long) data.get<double>("in_reply_to_user_id")) == listMembers[j])
                      Tweet(new TwStatus(data));
                  }
                } else
                  Tweet(new TwStatus(data));
              }
            }
          }
        }
        break;
      default:
        break;
    }
  } else if (data.has<string>("event")) {
    if (CString(data.get<string>("event")).Equals("follow")) {
      if (data.has<Object>("source")) {
        if (data.get<Object>("source").has<double>("id")) {
          if (Global::dataStore()->getOAuth()->getUserId() == (unsigned int) data.get<Object>("source").get<double>("id")) {
            if (data.has<Object>("target")) {
              if (data.get<Object>("target").has<double>("id"))
                Global::dataStore()->addFriend(data.get<Object>("target").get<double>("id"));
            }
          } else {
            if (type == Followers)
              User(new TwUser(data.get<Object>("source"),0));
          }
        }
      }
    } else if (CString(data.get<string>("event")).Equals("block")) {
      if (data.has<Object>("target")) {
        if (data.get<Object>("target").has<double>("id")) {
          deleteTweetsUser((unsigned int) data.get<Object>("target").get<double>("id"));
          deleteUser((unsigned int) data.get<Object>("target").get<double>("id"));
          Global::dataStore()->addBlock(data.get<Object>("target").get<double>("id"));
        }
      }
    }
  } else if (data.has<Object>("delete")) {
    if (data.get<Object>("delete").has<Object>("status")) {
      if (data.get<Object>("delete").get<Object>("status").has<double>("id"))
        deleteTweet((unsigned long) data.get<Object>("delete").get<Object>("status").get<double>("id"));
    }
  } else if (data.has<Array>("friends")) {
    if (listMembers.size() > 0) {
      setStream(true);
      for (vector<double>::size_type i = 0; i < listMembers.size(); i++) {
        if (!Global::dataStore()->isFriend(listMembers[i])) {
          setStream(false);
          break;
        }
      }
    }
  }
}

void Ui_Column::sendRequest()
{
  if (networkHandle != NULL)
    networkHandle->abort();
  map<CString,CString> params;
  switch (type) {
    case HomeTimeline:
      params["include_entities"] = "t";
      params["count"] = "50";
      if (since_id.size() > 0)
        params["since_id"] = since_id;
      networkHandle = network->get(Global::dataStore()->getOAuth()->APICall("http://api.twitter.com/1.1/statuses/home_timeline.json", params, "GET"));
      break;
    case Mentions:
      params["include_entities"] = "t";
      params["count"] = "50";
      if (since_id.size() > 0)
        params["since_id"] = since_id;
      networkHandle = network->get(Global::dataStore()->getOAuth()->APICall("http://api.twitter.com/1.1/statuses/mentions_timeline.json", params, "GET"));
      break;
    case List:
      params["include_entities"] = "t";
      params["list_id"] = listId;
      params["per_page"] = "50";
      if (since_id.size() > 0)
        params["since_id"] = since_id;
      params["include_rts"] = (allow_retweets ? "t" : "f");
      networkHandle = network->get(Global::dataStore()->getOAuth()->APICall("http://api.twitter.com/1.1/lists/statuses.json", params, "GET"));
      break;
    case Trending:
      params["id"] = "1";
      networkHandle = network->get(Global::dataStore()->getOAuth()->APICall("http://api.twitter.com/1.1/trends/place.json",params, "GET"));
      break;
    case Followers:
      params["skip_status"] = "t";
      networkHandle = network->get(Global::dataStore()->getOAuth()->APICall("http://api.twitter.com/1.1/followers/list.json",params, "GET"));
      break;
  }
}

void Ui_Column::onRequestCompleted(QNetworkReply *reply)
{
  reply->deleteLater();
  if (reply == networkHandle) {
    networkHandle = NULL;
    if (reply->error() == QNetworkReply::NoError) {
      istringstream input(reply->readAll().data());
      Array a;
      Object o;
      switch (type) {
        case HomeTimeline:
          if (Array::parse(input, a) and a.has<Object>(0)) {
            since_id = a.get<Object>(0).get<string>("id_str");
            for (int i = a.size() - 1; i >= 0; i--)
              Tweet(new TwStatus(a.get<Object>(i)));
          }
          break;
        case Mentions:
          if (Array::parse(input, a) and a.has<Object>(0)) {
            since_id = a.get<Object>(0).get<string>("id_str");
            for (int i = a.size() - 1; i >= 0; i--)
              Tweet(new TwStatus(a.get<Object>(i)));
          }
          break;
        case List:
          if (Array::parse(input, a) and a.has<Object>(0)) {
            since_id = a.get<Object>(0).get<string>("id_str");
            for (int i = a.size() - 1; i >= 0; i--)
              Tweet(new TwStatus(a.get<Object>(i)));
          }
          break;
        case Trending:
          if (Array::parse(input, a) and a.has<Object>(0)) {
            if (a.get<Object>(0).has<Array>("trends")) {
              QLayoutItem *child;
              while ((child  = tweetsLayout->takeAt(0)) != 0) {
                delete child->widget();
                delete child;
              }
              for (int i = 0; i < 10; i++) {
                if (a.get<Object>(0).get<Array>("trends").has<Object>(i)) {
                  if (a.get<Object>(0).get<Array>("trends").get<Object>(i).has<string>("name"))
                    tweetsLayout->addWidget(new TrendingTopic(CString::ConvertUnicode(a.get<Object>(0).get<Array>("trends").get<Object>(i).get<string>("name")).c_str()));
                }
              }
            }
          }
          break;
        case Followers:
          if (Object::parse(input, o) and o.has<Array>("users")) {
            QLayoutItem *child;
            while ((child = tweetsLayout->takeAt(0)) != 0) {
              delete child->widget();
              delete child;
            }
            for (unsigned int i = 0; i < o.get<Array>("users").size(); i++) {
              if (o.get<Array>("users").has<Object>(i))
                tweetsLayout->addWidget(new UserBox(new TwUser(o.get<Array>("users").get<Object>(i),(unsigned) 0)));
            }
          }
          break;
      }
    }
  } else if (reply == listMembersCall) {
    if (reply->error() == QNetworkReply::NoError) {
      istringstream input(reply->readAll().data());
      Object o;
      if (Object::parse(input, o) and o.has<Array>("users")) {
        for (unsigned int i = 0; i < o.get<Array>("users").size(); i++) {
          if (o.get<Array>("users").has<Object>(i)) {
            if (o.get<Array>("users").get<Object>(i).has<double>("id"))
              listMembers.push_back((unsigned long) o.get<Array>("users").get<Object>(i).get<double>("id"));
          }
        }
        if (type == List and stream) {
          for (vector<double>::size_type i = 0; i < listMembers.size(); i++) {
            if (!Global::dataStore()->isFriend(listMembers[i])) {
              setStream(false);
              break;
            }
          }
        }
        if (o.has<double>("next_cursor") and stream) {
          if (o.get<double>("next_cursor") != 0) {
            map<CString,CString> params;
            params["cursor"] = CString(o.get<double>("next_cursor"),0);
            params["list_id"] = listId;
            params["skip_status"] = "t";
            params["include_entities"] = "f";
            listMembersCall = network->get(Global::dataStore()->getOAuth()->APICall("http://api.twitter.com/1.1/lists/members.json", params, "GET"));
          }
        }
      }
    } else
      setStream(false);
  }
}

void Ui_Column::needBackfill()
{
  sendRequest();
}

void Ui_Column::newTitle(QString newTitle)
{
  titleLabel->setText(newTitle);
}

void Ui_Column::setInterval(int newInterval)
{
  interval = (unsigned) newInterval;
  timer->setInterval(newInterval*1000);
}

void Ui_Column::setAlert(int newAlert)
{
  alert = (unsigned) newAlert;
}

void Ui_Column::setAllowRetweets(bool newRetweets)
{
  allow_retweets = newRetweets;
}

void Ui_Column::resize(int i)
{
  setMinimumWidth(i);
  setMaximumWidth(i);
  scrollAreaWidgetContents_Column->setMinimumWidth(i - 40);
}

void Ui_Column::refilter()
{
  if (type == Trending or type == Followers)
    return;
  TweetBox* tmp;
  for (int i = 0; i < tweetsLayout->count(); i++) {
    QLayoutItem* item = tweetsLayout->itemAt(i);
    if (item->widget()->objectName() == "tweet") {
      tmp = static_cast<TweetBox*>(item->widget());
      if (isFiltered(userFilter, tmp->getTweet()->getUser()->getName(), false)
       or isFiltered(wordFilter, tmp->getTweet()->getText())
       or isFiltered(sourceFilter, tmp->getTweet()->getSource(false), false)
       or isFiltered(Global::dataStore()->getUserFilters(), tmp->getTweet()->getUser()->getName(), false)
       or isFiltered(Global::dataStore()->getWordFilters(), tmp->getTweet()->getText())
       or isFiltered(Global::dataStore()->getSourceFilters(), tmp->getTweet()->getSource(false), false))
        tmp->setVisible(false);
      else if (tmp->getTweet()->isRetweet()) {
        if (isFiltered(userFilter, tmp->getTweet()->getRetweet()->getUser()->getName(), false)
         or isFiltered(wordFilter, tmp->getTweet()->getRetweet()->getText())
         or isFiltered(sourceFilter, tmp->getTweet()->getRetweet()->getSource(false), false)
         or isFiltered(Global::dataStore()->getUserFilters(), tmp->getTweet()->getRetweet()->getUser()->getName(), false)
         or isFiltered(Global::dataStore()->getWordFilters(), tmp->getTweet()->getRetweet()->getText())
         or isFiltered(Global::dataStore()->getSourceFilters(), tmp->getTweet()->getRetweet()->getSource(false), false))
          tmp->setVisible(false);
      } else
        tmp->setVisible(true);
    }
  }
}

void Ui_Column::setFilter(FilterSettings *filter)
{
  filter->setField("user", userFilter);
  filter->setField("source", sourceFilter);
  filter->setField("text", wordFilter);
}

bool Ui_Column::contains(QWidget *widget)
{
  return (tweetsLayout->indexOf(widget) != -1);
}

TwStatus* Ui_Column::getTweet(unsigned long long id)
{
  TweetBox* box;
  for (int i = 0; i < tweetsLayout->count(); i++) {
    QLayoutItem* item = tweetsLayout->itemAt(i);
    if (item->widget()->objectName() == "tweet") {
      box = dynamic_cast<TweetBox*>(item->widget());
      if (box and box->getTweet()->getId() == id)
        return new TwStatus(*box->getTweet());
      if (box->getTweet()->isRetweet()) {
        if (box->getTweet()->getRetweet()->getId() == id)
          return new TwStatus(*box->getTweet()->getRetweet());
      }
    }
  }
  return NULL;
}
