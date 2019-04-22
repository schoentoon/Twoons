#include "ui_userprofile.h"
#include "moc_ui_userprofile.h"
#include "global.h"

Ui_UserProfile::Ui_UserProfile(QWidget *parent)
: Ui_Column("",parent)
{
  setVisible(false);
  setMinimumSize(QSize(293, 0));
  setMaximumSize(QSize(293, 16777215));
  setContentsMargins(0,0,0,0);
  closeButton = new QPushButton(this);
  closeButton->setText("X");
  closeButton->setMinimumSize(QSize(26, 26));
  closeButton->setMaximumSize(QSize(26, 26));
  columnLayout->addWidget(closeButton, 0, 2, Qt::AlignRight);

  realName = new QLabel(this);
  realName->setTextFormat(Qt::RichText);
  columnLayout->addWidget(realName, 0, 0, 1, 2);

  description = new QLabel(this);
  description->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
  description->setWordWrap(true);
  description->setTextFormat(Qt::RichText);
  columnLayout->addWidget(description, 1, 0, 1, -1);

  extraInfo = new QLabel(this);
  extraInfo->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
  extraInfo->setWordWrap(true);
  columnLayout->addWidget(extraInfo, 2, 0, 1, -1);

  tweetCount = new QPushButton(this);
  tweetCount->setMaximumSize(QSize(84, 42));
  tweetCount->setMinimumSize(QSize(84, 42));
  tweetCount->setCheckable(true);
  columnLayout->addWidget(tweetCount, 3, 0);

  favoriteCount = new QPushButton(this);
  favoriteCount->setMaximumSize(QSize(84, 42));
  favoriteCount->setMinimumSize(QSize(84, 42));
  favoriteCount->setCheckable(true);
  columnLayout->addWidget(favoriteCount, 3, 1);

  listCount = new QPushButton(this);
  listCount->setMaximumSize(QSize(84, 42));
  listCount->setMinimumSize(QSize(84, 42));
  listCount->setCheckable(true);
  columnLayout->addWidget(listCount, 3, 2);

  followersCount = new QPushButton(this);
  followersCount->setMinimumSize(QSize(84, 42));
  followersCount->setMaximumSize(QSize(84, 42));
  followersCount->setCheckable(true);
  columnLayout->addWidget(followersCount, 4, 0);

  followingCount = new QPushButton(this);
  followingCount->setMinimumSize(QSize(84, 42));
  followingCount->setMaximumSize(QSize(84, 42));
  followingCount->setCheckable(true);
  columnLayout->addWidget(followingCount, 4, 1);

  mentionsButton = new QPushButton(this);
  mentionsButton->setMinimumSize(QSize(84, 42));
  mentionsButton->setMaximumSize(QSize(84, 42));
  mentionsButton->setCheckable(true);
  mentionsButton->setText("Mentions");
  columnLayout->addWidget(mentionsButton, 4, 2);
  columnLayout->addWidget(scrollArea_Column, 5, 0, 1, -1);

  followButton = new Ui_FollowButton(this);
  followButton->setVisible(false);
  columnLayout->addWidget(followButton, 6, 0, 1, -1);

  QObject::connect(closeButton, SIGNAL(clicked())
                  ,this, SLOT(cleanUp()));
  QObject::connect(favoriteCount, SIGNAL(clicked(bool))
                  ,this, SLOT(getFavorites(bool)));
  QObject::connect(tweetCount, SIGNAL(clicked(bool))
                  ,this, SLOT(getTweets(bool)));
  QObject::connect(listCount, SIGNAL(clicked(bool))
                  ,this, SLOT(getLists(bool)));
  QObject::connect(mentionsButton, SIGNAL(clicked(bool))
                  ,this, SLOT(getMentions(bool)));
  QObject::connect(followersCount, SIGNAL(clicked(bool))
                  ,this, SLOT(getFollowers(bool)));
  QObject::connect(followingCount, SIGNAL(clicked(bool))
                  ,this, SLOT(getFollowing(bool)));
  QObject::connect(followButton, SIGNAL(openDM())
                  ,this, SLOT(openDM()));
}

void Ui_UserProfile::openProfile(unsigned long user_id)
{
  followButton->setVisible(false);
  this->user_id = user_id;
  getPage(Tweets);
  QLayoutItem* item;
  while ((item = tweetsLayout->takeAt(0)) != 0)
    item->widget()->deleteLater();
}

void Ui_UserProfile::openProfile(CString user_name)
{
  followButton->setVisible(false);
  screen_name = user_name;
  user_id = 0;
  getPage(Tweets);
  QLayoutItem* item;
  while ((item = tweetsLayout->takeAt(0)) != 0)
    item->widget()->deleteLater();
}

void Ui_UserProfile::cleanUp()
{
  setVisible(false);
  QLayoutItem* item;
  while ((item = tweetsLayout->takeAt(0)) != 0) {
    delete item->widget();
    delete item;
  }
  realName->clear();
  description->clear();
  tweetCount->setText("");
  favoriteCount->setText("");
  listCount->setText("");
  followButton->setVisible(false);
}

void Ui_UserProfile::renderProfile(CString tmp, CallType type)
{
  istringstream input(tmp);
  Object o;
  Array a;
  switch (type) {
    case Tweets:
      Array::parse(input, a);
      if (a.has<Object>(0)) {
        for (int i = a.size() - 1; i >= 0; i--)
          tweetsLayout->insertWidget(0, new TweetBox(new TwStatus(a.get<Object>(i))));
        if (a.get<Object>(0).has<Object>("user")) {
          Global::dataStore()->addUser(new StoreUser(new TwUser(a.get<Object>(0).get<Object>("user"), me)));
          if (a.get<Object>(0).get<Object>("user").has<double>("id"))
            user_id = (unsigned int) a.get<Object>(0).get<Object>("user").get<double>("id");
          if (a.get<Object>(0).get<Object>("user").has<string>("name"))
            realName->setText(CString("<span style=\" font-size:14pt;\">" +
                              CString::ConvertUnicode(a.get<Object>(0).get<Object>("user").get<string>("name")) + "</span>").c_str());
          else
            realName->setText("");
          if (a.get<Object>(0).get<Object>("user").has<string>("description"))
            description->setText(CString::ConvertUnicode(a.get<Object>(0).get<Object>("user").get<string>("description")).c_str());
          else
            description->setText("");
          if (a.get<Object>(0).get<Object>("user").has<string>("screen_name")) {
            setTitle(a.get<Object>(0).get<Object>("user").get<string>("screen_name").c_str());
            screen_name = a.get<Object>(0).get<Object>("user").get<string>("screen_name");
          }
          if (a.get<Object>(0).get<Object>("user").has<double>("statuses_count"))
            tweetCount->setText(CString(CString(a.get<Object>(0).get<Object>("user").get<double>("statuses_count"),0) +
                                "\nTweets").c_str());
          if (a.get<Object>(0).get<Object>("user").has<double>("favourites_count"))
            favoriteCount->setText(CString(CString(a.get<Object>(0).get<Object>("user").get<double>("favourites_count"),0) +
                                  "\nFavorites").c_str());
          if (a.get<Object>(0).get<Object>("user").has<double>("listed_count"))
            listCount->setText(CString(CString(a.get<Object>(0).get<Object>("user").get<double>("listed_count"),0) +
                              "\nListed").c_str());
          if (a.get<Object>(0).get<Object>("user").has<double>("followers_count"))
            followersCount->setText(CString(CString(a.get<Object>(0).get<Object>("user").get<double>("followers_count"),0) +
                                    "\nFollowers").c_str());
          if (a.get<Object>(0).get<Object>("user").has<double>("friends_count"))
            followingCount->setText(CString(CString(a.get<Object>(0).get<Object>("user").get<double>("friends_count"),0) +
                                    "\nFollowing").c_str());
          CString tmp;
          if (a.get<Object>(0).get<Object>("user").has<bool>("protected")) {
            if (a.get<Object>(0).get<Object>("user").get<bool>("protected"))
              tmp = "This account is protected.\n";
          }
          if (a.get<Object>(0).get<Object>("user").has<bool>("verified")) {
            if (a.get<Object>(0).get<Object>("user").get<bool>("verified"))
              tmp = tmp + "This account is verified.\n";
          }
          extraInfo->setText(tmp.Trim_n().c_str());
        }
        setVisible(true);
        getPage(Friendship);
      } else
        getPage(UserProfile);
      tweetCount->setChecked(true);
      favoriteCount->setChecked(false);
      listCount->setChecked(false);
      mentionsButton->setChecked(false);
      followersCount->setChecked(false);
      followingCount->setChecked(false);
      break;
    case Favorites:
      Array::parse(input, a);
      for (int i = a.size() - 1; i >= 0; i--)
        tweetsLayout->insertWidget(0, new TweetBox(new TwStatus(a.get<Object>(i))));
      tweetCount->setChecked(false);
      favoriteCount->setChecked(true);
      listCount->setChecked(false);
      mentionsButton->setChecked(false);
      followersCount->setChecked(false);
      followingCount->setChecked(false);
      setVisible(true);
      break;
    case Lists:
      Object::parse(input, o);
      if (o.has<Array>("lists")) {
        if (o.get<Array>("lists").has<Object>(0)) {
          for (int i = o.get<Array>("lists").size() - 1; i >= 0; i--)
            tweetsLayout->insertWidget(0, new ListBox(TwList(o.get<Array>("lists").get<Object>(i), me)));
        }
      }
      tweetCount->setChecked(false);
      favoriteCount->setChecked(false);
      listCount->setChecked(true);
      mentionsButton->setChecked(false);
      followersCount->setChecked(false);
      followingCount->setChecked(false);
      setVisible(true);
      break;
    case Followers:
      Object::parse(input, o);
      tweetCount->setChecked(false);
      favoriteCount->setChecked(false);
      listCount->setChecked(false);
      mentionsButton->setChecked(false);
      followersCount->setChecked(true);
      followingCount->setChecked(false);
      if (o.has<Array>("users")) {
        for (int i = o.get<Array>("users").size() -1; i >= 0; i--)
          tweetsLayout->insertWidget(0, new UserBox(new TwUser(o.get<Array>("users").get<Object>(i), me)));
      }
      break;
    case Following:
      Object::parse(input, o);
      tweetCount->setChecked(false);
      favoriteCount->setChecked(false);
      listCount->setChecked(false);
      mentionsButton->setChecked(false);
      followersCount->setChecked(false);
      followingCount->setChecked(true);
      if (o.has<Array>("users")) {
        for (int i = o.get<Array>("users").size() -1; i >= 0; i--)
          tweetsLayout->insertWidget(0, new UserBox(new TwUser(o.get<Array>("users").get<Object>(i), me)));
      }
      break;
    case Mentions:
      Object::parse(input, o);
      if (o.has<Array>("statuses")) {
        for (int i = o.get<Array>("statuses").size() - 1; i >= 0; i--)
          tweetsLayout->insertWidget(0, new TweetBox(new TwStatus(o.get<Array>("statuses").get<Object>(i))));
      }
      tweetCount->setChecked(false);
      favoriteCount->setChecked(false);
      listCount->setChecked(false);
      mentionsButton->setChecked(true);
      followersCount->setChecked(false);
      followingCount->setChecked(false);
      setVisible(true);
      break;
    case UserProfile:
      Object::parse(input, o);
      if (o.has<string>("error")) {
        QObject::connect(this, SIGNAL(sendError(CString))
                        ,Global::Get(), SLOT(sendError(CString)));
        emit sendError(o.get<string>("error"));
      } else {
        TwUser *user = new TwUser(o, me);
        Global::dataStore()->addUser(new StoreUser(user));
        delete user;
        if (o.has<double>("id"))
          user_id = (unsigned int) o.get<double>("id");
        if (o.has<string>("name"))
          realName->setText(CString("<span style=\" font-size:14pt;\">" +
                            CString::ConvertUnicode(o.get<string>("name")) + "</span>").c_str());
        if (o.has<string>("description"))
          description->setText(CString::ConvertUnicode(o.get<string>("description")).c_str());
        if (o.has<string>("screen_name")) {
          setTitle(o.get<string>("screen_name").c_str());
          screen_name = o.get<string>("screen_name");
        }
        if (o.has<double>("statuses_count"))
          tweetCount->setText(CString(CString(o.get<double>("statuses_count"),0) +
                              "\nTweets").c_str());
        if (o.has<double>("favourites_count"))
          favoriteCount->setText(CString(CString(o.get<double>("favourites_count"),0) +
                                "\nFavorites").c_str());
        if (o.has<double>("listed_count"))
          listCount->setText(CString(CString(o.get<double>("listed_count"),0) +
                            "\nListed").c_str());
        if (o.has<double>("followers_count"))
          followersCount->setText(CString(CString(o.get<double>("followers_count"),0) +
                                  "\nFollowers").c_str());
        if (o.has<double>("friends_count"))
          followingCount->setText(CString(CString(o.get<double>("friends_count"),0) +
                                  "\nFollowing").c_str());
        CString tmp;
        if (o.has<bool>("protected")) {
          if (o.get<bool>("protected"))
            tmp = "This account is protected.\n";
        }
        if (o.has<bool>("verified")) {
          if (o.get<bool>("verified"))
            tmp = tmp + "This account is verified.\n";
        }
        extraInfo->setText(tmp.Trim_n().c_str());
        setVisible(true);
        getPage(Friendship);
      }
      break;
    case Friendship:
      if (Object::parse(input, o))
        followButton->setFriendShip(o);
      break;
  }
}

void Ui_UserProfile::getTweets(bool tmp)
{
  if (!tmp)
    tweetCount->setChecked(true);
  favoriteCount->setChecked(false);
  listCount->setChecked(false);
  mentionsButton->setChecked(false);
  followersCount->setChecked(false);
  followingCount->setChecked(false);
  getPage(Tweets);
  QLayoutItem* item;
  while ((item = tweetsLayout->takeAt(0)) != 0) {
    delete item->widget();
    delete item;
  }
}

void Ui_UserProfile::getFavorites(bool tmp)
{
  if (!tmp)
    favoriteCount->setChecked(true);
  tweetCount->setChecked(false);
  listCount->setChecked(false);
  mentionsButton->setChecked(false);
  followersCount->setChecked(false);
  followingCount->setChecked(false);
  getPage(Favorites);
  QLayoutItem* item;
  while ((item = tweetsLayout->takeAt(0)) != 0) {
    delete item->widget();
    delete item;
  }
}

void Ui_UserProfile::getLists(bool tmp)
{
  if (!tmp)
    listCount->setChecked(true);
  tweetCount->setChecked(false);
  favoriteCount->setChecked(false);
  mentionsButton->setChecked(false);
  followersCount->setChecked(false);
  followingCount->setChecked(false);
  getPage(Lists);
  QLayoutItem* item;
  while ((item = tweetsLayout->takeAt(0)) != 0) {
    delete item->widget();
    delete item;
  }
}

void Ui_UserProfile::getMentions(bool tmp)
{
  if (!tmp)
    mentionsButton->setChecked(true);
  tweetCount->setChecked(false);
  favoriteCount->setChecked(false);
  listCount->setChecked(false);
  followersCount->setChecked(false);
  followingCount->setChecked(false);
  getPage(Mentions);
  QLayoutItem* item;
  while ((item = tweetsLayout->takeAt(0)) != 0) {
    delete item->widget();
    delete item;
  }
}

void Ui_UserProfile::getFollowers(bool tmp)
{
  if (!tmp)
    followersCount->setChecked(true);
  tweetCount->setChecked(false);
  favoriteCount->setChecked(false);
  listCount->setChecked(false);
  mentionsButton->setChecked(false);
  followingCount->setChecked(false);
  getPage(Followers);
  QLayoutItem* item;
  while ((item = tweetsLayout->takeAt(0)) != 0) {
    delete item->widget();
    delete item;
  }
}

void Ui_UserProfile::getFollowing(bool tmp)
{
  if (!tmp)
    followButton->setChecked(true);
  tweetCount->setChecked(false);
  favoriteCount->setChecked(false);
  listCount->setChecked(false);
  mentionsButton->setChecked(false);
  followersCount->setChecked(false);
  getPage(Following);
  QLayoutItem* item;
  while ((item = tweetsLayout->takeAt(0)) != 0) {
    delete item->widget();
    delete item;
  }
}

void Ui_UserProfile::openDM()
{
  QObject::connect(this, SIGNAL(openDM(CString,unsigned int))
                  ,Global::Get(), SLOT(openDM(CString,unsigned int)));
  emit openDM(screen_name,user_id);
}

void Ui_UserProfile::fromMainWindow(Object&, APItypes)
{
  followButton->setVisible(false);
  getPage(Friendship);
}

void Ui_UserProfile::getPage(CallType type)
{
  map<CString, CString> meh;
  this->type = type;
  QObject::connect(network, SIGNAL(finished(QNetworkReply*))
                  ,this, SLOT(onRequestCompleted(QNetworkReply*)));
  switch (type) {
    case Tweets:
      if (user_id != 0)
        meh["user_id"] = CString(user_id);
      else
        meh["screen_name"] = screen_name;
      meh["count"] = "20";
      meh["include_entities"] = "t";
      meh["include_rts"] = "t";
      networkHandle = network->get(Global::dataStore()->getOAuth()->APICall("http://api.twitter.com/1.1/statuses/user_timeline.json", meh, "GET"));
      break;
    case Favorites:
      if (user_id != 0)
        meh["id"] = CString(user_id);
      else
        meh["screen_name"] = screen_name;
      meh["include_entities"] = "t";
      networkHandle = network->get(Global::dataStore()->getOAuth()->APICall("http://api.twitter.com/1.1/favorites/list.json", meh, "GET"));
      break;
    case Lists:
      if (user_id != 0)
        meh["user_id"] = CString(user_id);
      else
        meh["screen_name"] = screen_name;
      networkHandle = network->get(Global::dataStore()->getOAuth()->APICall("http://api.twitter.com/1.1/lists/memberships.json", meh, "GET"));
      break;
    case Followers:
      if (user_id != 0)
        meh["user_id"] = CString(user_id);
      networkHandle = network->get(Global::dataStore()->getOAuth()->APICall("http://api.twitter.com/1.1/followers/list.json", meh, "GET"));
      break;
    case Following:
      if (user_id != 0)
        meh["user_id"] = CString(user_id);
      networkHandle = network->get(Global::dataStore()->getOAuth()->APICall("http://api.twitter.com/1.1/friends/list.json", meh, "GET"));
      break;
    case Mentions:
      meh["q"] = CString("@" + screen_name);
      meh["result_type"] = "recent";
      meh["include_entities"] = "t";
      networkHandle = network->get(Global::dataStore()->getOAuth()->APICall("http://api.twitter.com/1.1/search/tweets.json", meh, "GET"));
      break;
    case UserProfile: /* Fallback to get just the user object (protected and not following) */
      if (user_id != 0)
        meh["user_id"] = CString(user_id);
      else
        meh["screen_name"] = screen_name;
      networkHandle = network->get(Global::dataStore()->getOAuth()->APICall("http://api.twitter.com/1.1/users/show.json", meh, "GET"));
      break;
    case Friendship:
      if (user_id == me)
        renderProfile("{\"mhmm\":\"It\'s you.\"}",Friendship);
      else {
        if (user_id != 0)
          meh["target_id"] = CString(user_id);
        meh["source_id"] = CString(me);
        networkHandle = network->get(Global::dataStore()->getOAuth()->APICall("http://api.twitter.com/1.1/friendships/show.json", meh, "GET"));
      }
      break;
  }
}

void Ui_UserProfile::onRequestCompleted(QNetworkReply *reply)
{
  if (networkHandle == NULL)
    return;
  if (reply == networkHandle) {
    reply->deleteLater();
    networkHandle = NULL;
    QObject::disconnect(network, SIGNAL(finished(QNetworkReply*))
                       ,this, SLOT(onRequestCompleted(QNetworkReply*)));
    renderProfile(reply->readAll().data(),type);
  }
}
