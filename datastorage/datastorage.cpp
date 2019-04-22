#include "datastorage.h"
#include "moc_datastorage.h"
#include "global.h"

dataStorage::dataStorage()
{
  replyIcon = new QIcon();
  replyIcon->addFile(":/buttons/img/reply.png", QSize(), QIcon::Normal, QIcon::Off);
  replyIcon->addFile(":/buttons/img/reply_hover.png", QSize(), QIcon::Active, QIcon::On);
  retweetIcon = new QIcon();
  retweetIcon->addFile(":/buttons/img/retweet.png", QSize(), QIcon::Normal, QIcon::Off);
  retweetIcon->addFile(":/buttons/img/retweet_hover.png", QSize(), QIcon::Active, QIcon::On);
  retweetOnIcon = new QIcon();
  retweetOnIcon->addFile(":/buttons/img/retweet_on.png", QSize(), QIcon::Normal, QIcon::Off);
  favoriteIcon = new QIcon();
  favoriteIcon->addFile(":/buttons/img/favorite.png", QSize(), QIcon::Normal, QIcon::Off);
  favoriteIcon->addFile(":/buttons/img/favorite_hover.png", QSize(), QIcon::Active, QIcon::On);
  favoriteOnIcon = new QIcon();
  favoriteOnIcon->addFile(":/buttons/img/favorite_on.png", QSize(), QIcon::Normal, QIcon::Off);
  deleteIcon = new QIcon();
  deleteIcon->addFile(":/buttons/img/delete.png", QSize(), QIcon::Normal, QIcon::Off);
  blockIcon = new QIcon();
  blockIcon->addFile(":/buttons/img/block.png", QSize(), QIcon::Normal, QIcon::Off);
  spamIcon = new QIcon();
  spamIcon->addFile(":/buttons/img/spam.png", QSize(), QIcon::Normal, QIcon::Off);
  network = new QNetworkAccessManager();
  profilePictureNetwork = new QNetworkAccessManager();
  QObject::connect(network, SIGNAL(finished(QNetworkReply*))
                  ,this, SLOT(info(QNetworkReply*)));
  QObject::connect(profilePictureNetwork, SIGNAL(finished(QNetworkReply*))
                  ,this, SLOT(onRequestCompleted(QNetworkReply*)));
  user = new OAuth();
  infoRequestTimer = new QTimer(this);
  QObject::connect(infoRequestTimer, SIGNAL(timeout())
                  ,this, SLOT(requestInfo()));
  lastRequest = 0;
  currentUploadingService = 0;
}

QIcon* dataStorage::getIcon(Icons i)
{
  switch (i) {
    case Reply:
      return replyIcon;
    case Retweet:
      return retweetIcon;
    case Retweeted:
      return retweetOnIcon;
    case Favorite:
      return favoriteIcon;
    case Favorited:
      return favoriteOnIcon;
    case Delete:
      return deleteIcon;
    case Block:
      return blockIcon;
    case Report:
      return spamIcon;
    default:
      return new QIcon();
  }
}

void dataStorage::info(QNetworkReply *reply)
{
  reply->deleteLater();
  stringstream input;
  Array a;
  if (reply == rBlocked) {
    input << reply->readAll().data();
    if (Array::parse(input, a)) {
      blocked.clear();
      for (unsigned int i = 0; i < a.size(); i++) {
        if (a.has<double>(i))
          blocked.push_back(a.get<double>(i));
      }
    }
  } else if (reply == rNo_retweets) {
    input << reply->readAll().data();
    if (Array::parse(input, a)) {
      no_retweets.clear();
      for (unsigned int i = 0; i < a.size(); i++) {
        if (a.has<double>(i))
          no_retweets.push_back(a.get<double>(i));
      }
    }
  }
  lastRequest = time(NULL);
  if (reply->error() == QNetworkReply::NoError)
    infoRequestTimer->setInterval(6 * 3600 * 1000);
  else
    infoRequestTimer->setInterval(300 * 1000);
  infoRequestTimer->start();
  emit saveSettings();
}

void dataStorage::addBlock(unsigned long long id)
{
  blocked.push_back(id);
}

void dataStorage::onRequestCompleted(QNetworkReply *reply)
{
  reply->deleteLater();
  for (vector<QNetworkReply*>::size_type i = 0; i < profilePictureReplies.size(); i++) {
    if (reply == profilePictureReplies[i]) {
      if (reply->error() == QNetworkReply::NoError) {
        QPixmap pixmap;
        if (pixmap.loadFromData(reply->readAll())) {
          pixmap=pixmap.scaled(48,48,Qt::KeepAspectRatio);
          if (!pixmap.isNull()) {
            QPixmapCache::insert(reply->url().toString(), pixmap);
            emit sendProfilePicture(CString(reply->url().toString().toLatin1().data()),pixmap);
          }
        }
      }
      profilePictureReplies.erase(profilePictureReplies.begin() + i);
      break;
    }
  }
}

/* User store related methods */

void dataStorage::addUser(StoreUser* user)
{
  for (vector<StoreUser*>::size_type i = 0; i < users.size(); i++) {
    if (users[i]->getId() == user->getId()) {
      delete users[i];
      users.erase(users.begin() + i);
      break;
    }
  }
  users.insert(users.begin(), user);
}

VCString dataStorage::queryUserDB(CString query)
{
  VCString tmp;
  for (vector<StoreUser*>::size_type i = 0; i < users.size(); i++) {
    if (users[i]->getName().AsLower().WildCmp("*" + query.AsLower() + "*"))
      tmp.push_back(users[i]->getName());
  }
  return tmp;
}

bool dataStorage::isFriend(unsigned long long user_id)
{
  for (vector<unsigned long long>::size_type i = 0; i < friends.size(); i++) {
    if (user_id == friends[i])
      return true;
  }
  return false;
}

void dataStorage::removeFriend(unsigned long long user_id)
{
  for (vector<unsigned long long>::iterator i = friends.begin(); i != friends.end(); i++) {
    if (user_id == *i)
      friends.erase(i);
  }
}

bool dataStorage::isBlocked(unsigned long long user_id)
{
  for (vector<unsigned long long>::size_type i = 0; i < blocked.size(); i++) {
    if (blocked[i] == user_id)
      return true;
  }
  return false;
}

bool dataStorage::wantsRetweetsFrom(unsigned long long user_id)
{
  for (vector<unsigned long long>::size_type i = 0; i < no_retweets.size(); i++) {
    if (no_retweets[i] == user_id)
      return false;
  }
  return true;
}

void dataStorage::clearFriends()
{
  friends.clear();
}

void dataStorage::setOAuth(OAuth *user)
{
  delete this->user;
  this->user = user;
}

void dataStorage::requestInfo()
{
  rBlocked = network->get(user->APICall("http://api.twitter.com/1.1/blocks/blocking/ids.json","GET"));
  rNo_retweets = network->get(user->APICall("http://api.twitter.com/1.1/friendships/no_retweet_ids.json","GET"));
}


void dataStorage::onIgnoreSSLErrors(QNetworkReply* reply, QList<QSslError> errors)
{
  DEBUG("onIgnoreSSLErrors()");
  for (int i = 0; i < errors.size(); ++i) {
    DEBUG(errors[i].errorString().toStdString());
  };
  reply->ignoreSslErrors(errors);
}

void dataStorage::getProfilePicture(CString url)
{
  QPixmap pixmap;
  if (QPixmapCache::find(url.c_str(), &pixmap))
    emit sendProfilePicture(url, pixmap);
  else
  for (vector<QNetworkReply*>::size_type i = 0; i < profilePictureReplies.size(); i++) {
    if (profilePictureReplies[i]->url().toString().compare(url.c_str()) == 0)
      return;
  }
  QNetworkRequest request(QUrl(url.c_str()));
  request.setRawHeader("User-Agent", "Twoons for " OS " ~ " VERSION);
  profilePictureReplies.push_back(profilePictureNetwork->get(request));
}

void dataStorage::killAllRequests()
{
}

void dataStorage::setFilters(CString field, VCString info)
{
  if (field.Equals("user"))
    userFilter = info;
  else if (field.Equals("source"))
    sourceFilter = info;
  else if (field.Equals("text"))
    wordFilter = info;
  emit newGlobalFilters();
}

CString dataStorage::printSettings()
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
  tmp = tmp + "]},\"blocks\":[";
  for (vector<unsigned long long>::size_type i = 0; i < blocked.size(); i++)
    tmp = tmp + CString(blocked[i]) + ",";
  if (blocked.size() > 0)
    tmp.RightChomp(1);
  tmp = tmp + "],\"no_retweets\":[";
  for (vector<unsigned long long>::size_type i = 0; i < no_retweets.size(); i++)
    tmp = tmp + CString(no_retweets[i]) + ",";
  if (no_retweets.size() > 0)
    tmp.RightChomp(1);
  tmp = tmp + "],\"last_request\":" + CString(lastRequest);
  tmp = tmp + ",\"uploading_service\":" + CString(currentUploadingService);
  tmp = tmp + "}";
  return tmp;
}

void dataStorage::importSettings(Object& settings)
{
  if (settings.has<Object>("filters")) {
    if (settings.get<Object>("filters").has<Array>("username")) {
      for (unsigned int i = 0; i < settings.get<Object>("filters").get<Array>("username").size(); i++)
        userFilter.push_back(settings.get<Object>("filters").get<Array>("username").get<string>(i));
    }
    if (settings.get<Object>("filters").has<Array>("source")) {
      for (unsigned int i = 0; i < settings.get<Object>("filters").get<Array>("source").size(); i++)
        sourceFilter.push_back(settings.get<Object>("filters").get<Array>("source").get<string>(i));
    }
    if (settings.get<Object>("filters").has<Array>("text")) {
      for (unsigned int i = 0; i < settings.get<Object>("filters").get<Array>("text").size(); i++)
        wordFilter.push_back(settings.get<Object>("filters").get<Array>("text").get<string>(i));
    }
  }
  if (settings.has<Array>("blocks")) {
    for (unsigned int i = 0 ; i < settings.get<Array>("blocks").size(); i++) {
      if (settings.get<Array>("blocks").has<double>(i)) {
        if (!isBlocked(settings.get<Array>("blocks").get<double>(i)))
          blocked.push_back(settings.get<Array>("blocks").get<double>(i));
      }
    }
  }
  if (settings.has<Array>("no_retweets")) {
    for (unsigned int i = 0 ; i < settings.get<Array>("no_retweets").size(); i++) {
      if (settings.get<Array>("no_retweets").has<double>(i)) {
        if (wantsRetweetsFrom(settings.get<Array>("no_retweets").get<double>(i)))
          no_retweets.push_back(settings.get<Array>("no_retweets").get<double>(i));
      }
    }
  }
  if (settings.has<double>("last_request")) {
    lastRequest = settings.get<double>("last_request");
    if ((lastRequest > (unsigned int) time(NULL)
     or lastRequest < ((unsigned int) time(NULL) - (3600 * 6)))
     and (lastRequest - (3600 * 6) - (unsigned int) time(NULL) > 0))
      requestInfo();
    else {
      infoRequestTimer->setInterval((lastRequest + (3600 * 6) - (unsigned int) time(NULL)) * 1000);
      infoRequestTimer->start();
    }
  } else
    requestInfo();
  if (settings.has<double>("uploading_service"))
    currentUploadingService = settings.get<double>("uploading_service");
}
