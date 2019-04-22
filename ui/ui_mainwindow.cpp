#include "ui_mainwindow.h"
#include "moc_ui_mainwindow.h"
#include "global.h"

Ui_MainWindow::Ui_MainWindow(QWidget *parent)
: QWidget(parent)
{ /* Do the config reading at the beginning of everything */
  if (!QFile(".git").exists()) {
    QDir work(QDir::homePath() + "/.twoons");
    if (!work.exists())
      QDir().mkdir(work.path());
    QFile legacy("config");
    if (legacy.exists()) {
      QDir().mkdir(QDir::homePath() + "/.twoons");
      legacy.rename(QDir::homePath() + "/.twoons/config");
    }
    config = new QFile(QDir::homePath() + "/.twoons/config");
  } else
    config = new QFile("config");
  stringstream input;
  if (!config->open(QIODevice::ReadOnly|QIODevice::Text)) {
    input << "{\"columns\":[{\"filters\":{\"username\":[]"
             ",\"source\":[]"
             ",\"text\":[]}"
             ",\"limit\":200,\"alert\":false,\"interval\":30"
             ",\"type\":0,\"streaming\":true,\"name\":\"All friends\"}"
             ",{\"filters\":{\"username\":[]"
             ",\"source\":[]"
             ",\"text\":[]}"
             ",\"limit\":200,\"alert\":true,\"interval\":30"
             ",\"type\":1,\"streaming\":true,\"name\":\"Mentions\"}]"
             ",\"replies\":false}";
  } else {
    QTextStream textStream(config);
    QString settings;
    while (!textStream.atEnd())
      input << textStream.readLine().toStdString();
    config->close();
  }
  Object o;
  if (!(Object::parse(input, o) or o.has<string>("key") or o.has<string>("secret"))) {
    auth = new Ui_Auth();
    QObject::connect(auth, SIGNAL(sendKeys(CString,CString))
                    ,this, SLOT(getKeys(CString,CString)));
  } else
    auth = NULL;
  Global::dataStore()->setParent(this);
  if (o.has<Object>("datastore"))
    Global::dataStore()->importSettings(o.get<Object>("datastore"));

  QFile stylesheet("stylesheet.qss");
  if (stylesheet.open(QIODevice::ReadOnly|QIODevice::Text))
    qApp->setStyleSheet(stylesheet.readAll());
  if (o.has<string>("key") and o.has<string>("secret")) {
    aKey=o.get<string>("key");
    aSec=o.get<string>("secret");
    Global::dataStore()->setOAuth(new OAuth(aKey, aSec));
  }

  resize(640, 505);
  setWindowTitle("Twoons");
  setWindowIcon(QIcon(":/icons/32x32.png"));
  setContentsMargins(0,0,0,0);
  QTimer::singleShot(30000, this, SLOT(initNotif()));
  mainLayout = new QGridLayout(this);
  mainLayout->setContentsMargins(0,0,0,0);
  setLayout(mainLayout);
  horizontalLayout = new QHBoxLayout();
  mainLayout->addLayout(horizontalLayout, 1, 0, 1, -1);

  tweetButton = new QPushButton(this);
  tweetButton->setMaximumSize(QSize(50, 32));
  tweetButton->setText("Tweet");
  tweetButton->setCheckable(true);
  horizontalLayout->addWidget(tweetButton);

  errorBox = new QLabel(this);
  horizontalLayout->addWidget(errorBox);
  errorTimer = new QTimer();
  errorTimer->setInterval(5000);
  QObject::connect(errorTimer, SIGNAL(timeout())
                  ,errorBox, SLOT(clear()));

  lookupLabel = new QLabel(this);
  lookupLabel->setText("User lookup: ");
  horizontalLayout->addWidget(lookupLabel);

  lookupField = new QTextEdit(this);
  lookupField->installEventFilter(this);
  lookupField->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  lookupField->setMaximumHeight(24);
  horizontalLayout->addWidget(lookupField);

  versionLabel = new QLabel(this);
  versionLabel->setText(VERSION);
  horizontalLayout->addWidget(versionLabel);

  settingsWindow = new Ui_Settings();
  passwordBox = new QInputDialog(this);
  passwordBox->setLabelText("Password:");
  passwordBox->setTextEchoMode(QLineEdit::Password);
  if (o.has<string>("password"))
    password = o.get<string>("password").c_str();
  QObject::connect(passwordBox, SIGNAL(accepted())
                  ,this, SLOT(passwordAccepted()));

  showDMs = new QPushButton(this);
  showDMs->setText("DM");
  horizontalLayout->addWidget(showDMs);

  settingsButton = new QPushButton(this);
  settingsButton->setCheckable(true);
  settingsButton->setText("Settings");
  settingsButton->setMaximumSize(100, 50);
  horizontalLayout->addWidget(settingsButton);
  QObject::connect(settingsButton, SIGNAL(clicked(bool))
                  ,settingsWindow, SLOT(setVisible(bool)));

  tweetField = new TweetField(this);
  tweetField->setVisible(false);
  if (o.has<double>("tweetfield_size"))
    tweetField->setMaximumHeight(o.get<double>("tweetfield_size"));
  else
    tweetField->setMaximumHeight(100);
  settingsWindow->setTweetField(tweetField);
  mainLayout->addWidget(tweetField, 2, 0, 1, -1);
  QObject::connect(tweetButton, SIGNAL(toggled(bool))
                  ,tweetField, SLOT(tweetButtonClicked(bool)));

  tweetFieldWindow = new TweetField(this);
  tweetFieldWindow->setWindowTitle("Tweet");
  tweetFieldWindow->setWindowIcon(QIcon(":/icons/32x32.png"));
  tweetFieldWindow->resize(400, 100);
  tweetFieldWindow->hideAfterTweet(true);
  tweetFieldWindow->move((QApplication::desktop()->width()/2)-(tweetFieldWindow->size().width()/2), 100);

  directMessages = new Ui_DirectMessage();
  QObject::connect(showDMs, SIGNAL(clicked())
                  ,this, SLOT(toggleDMs()));
  QObject::connect(directMessages, SIGNAL(notifyDM())
                  ,this, SLOT(newDM()));

  trayIcon = new QSystemTrayIcon(QIcon(":/icons/32x32.png"));
  trayIcon->setContextMenu(new QMenu(this));
  trayIcon->contextMenu()->addAction("Hide", this, SLOT(trayIconClicked()));
  trayIcon->contextMenu()->addSeparator();
  trayIcon->contextMenu()->addAction("Tweet", tweetFieldWindow, SLOT(show()));
  trayIcon->contextMenu()->addAction("Quit", qApp, SLOT(quit()));
  QObject::connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason))
                  ,this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
  trayIcon->show();

  scrollArea_Columns = new QScrollArea(this);
  scrollArea_Columns->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollArea_Columns->setWidgetResizable(true);
  scrollArea_Columns->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
  scrollArea_Columns->setContentsMargins(0,0,0,0);
  scrollAreaWidgetContents = new QWidget();
  scrollAreaWidgetContents->setContentsMargins(0,0,0,0);
  columnViewLayout = new QHBoxLayout(scrollAreaWidgetContents);
  columnViewLayout->setContentsMargins(0,0,0,0);
  columnViewLayout->setAlignment(Qt::AlignLeft);

  if (o.has<Array>("columns")) {
    for (unsigned int i = 0; i < o.get<Array>("columns").size(); i++) {
      if (o.get<Array>("columns").has<Object>(i)) {
        if (o.get<Array>("columns").get<Object>(i).has<string>("name")) {
          columns.push_back(new Ui_Column(o.get<Array>("columns").get<Object>(i).get<string>("name").c_str(),this));
          columns[columns.size() - 1]->importSettings(o.get<Array>("columns").get<Object>(i));
          columnViewLayout->addWidget(columns[columns.size() - 1]);
        }
      }
    }
  }

  scrollArea_Columns->setWidget(scrollAreaWidgetContents);
  mainLayout->addWidget(scrollArea_Columns, 3, 1);
  userProfile = new Ui_UserProfile(this);
  openweb = false;
  mainLayout->addWidget(userProfile, 3, 2);
  QObject::connect(this, SIGNAL(openProfile(unsigned long))
                  ,userProfile, SLOT(openProfile(unsigned long)));
  QObject::connect(this, SIGNAL(openProfile(CString))
                  ,userProfile, SLOT(openProfile(CString)));

  autoComplete = new Ui_Autocomplete();
  QObject::connect(tweetField, SIGNAL(callAutocomplete())
                  ,autoComplete, SLOT(appear()));
  QObject::connect(autoComplete, SIGNAL(sendNickname(CString))
                  ,tweetField, SLOT(getNickname(CString)));
  QObject::connect(autoComplete, SIGNAL(focus())
                  ,tweetField, SLOT(focus()));

  qRegisterMetaType<CString>("CString");
  qRegisterMetaType<APItypes>("APItypes");
  qRegisterMetaType<DM>("DM");

  if (o.has<bool>("replies"))
    stream_thread = new StreamThread(o.get<bool>("replies"));
  else
    stream_thread = new StreamThread();
  if (o.has<bool>("stream"))
    stream = o.get<bool>("stream");
  else if (o.has<Array>("columns")) { /* This is really just for legacy */
    if (o.get<Array>("columns").has<Object>(0)) { /* Aka it'll use the old method of getting the streaming bool */
      if (o.get<Array>("columns").get<Object>(0).has<bool>("streaming")) /* And set this to the new one */
        stream = o.get<Array>("columns").get<Object>(0).get<bool>("streaming");
      else
        stream = true;
    }
  } else
    stream = true;
  settingsWindow->setStreamThread(stream_thread);
  settingsWindow->setStreamState(stream);
  settingsWindow->setRealColumns(columns);
  QObject::connect(settingsWindow, SIGNAL(setPassword(QString))
                  ,this, SLOT(setPassword(QString)));
  if (o.has<string>("key") and o.has<string>("secret")) {
    if (columns.size() > 0) {
      QObject::connect(stream_thread, SIGNAL(sendData(CString))
                      ,this, SLOT(receiveData(CString)));
      QObject::connect(stream_thread, SIGNAL(needBackfill())
                      ,this, SLOT(needBackfill()));
      if (stream)
        stream_thread->start();
      Global::dataStore()->getOAuth()->setStream(stream);
    }
    for (unsigned int i = 0; i < columns.size(); i++) {
      columns[i]->setUserId(aKey.Token(0,false,"-").ToUInt());
    }
    userProfile->setUserId(aKey.Token(0,false,"-").ToUInt());
    Global::dataStore()->addFriend(aKey.Token(0,false,"-").ToUInt());
    if (o.has<bool>("ssl"))
      Global::dataStore()->getOAuth()->SetSsl(o.get<bool>("ssl"));
    settingsWindow->setSslCheckbox(Global::dataStore()->getOAuth()->isSsl());;
  }
  mainLayout->addWidget(settingsWindow, 0, 0, -1, -1, Qt::AlignCenter);
  mainLayout->addWidget(autoComplete, 0, 0, -1, -1, Qt::AlignCenter);
  mainLayout->addWidget(directMessages, 0, 0, -1, -1, Qt::AlignCenter);
  if (auth)
    mainLayout->addWidget(auth, 0, 0, -1, -1, Qt::AlignCenter);

  imageView = new Ui_ImageView(this);
  pictureHandlers = PictureFramework::init_pictures(imageView);
  mainLayout->addWidget(imageView, 0, 0, -1, -1, Qt::AlignCenter);
  QObject::connect(tweetField, SIGNAL(sendError(CString))
                  ,this, SLOT(sendError(CString)));
  QObject::connect(tweetField, SIGNAL(retweeted(unsigned long long))
                  ,this, SLOT(markRetweeted(unsigned long long)));
  QObject::connect(settingsWindow, SIGNAL(saveSettings())
                  ,this, SLOT(writeSettings()));
  QObject::connect(settingsWindow, SIGNAL(goGetNewColumns())
                  ,this, SLOT(getNewColumns()));
  QObject::connect(Global::dataStore(), SIGNAL(saveSettings())
                  ,this, SLOT(writeSettings()));
}

void Ui_MainWindow::openConversationView(unsigned long long id, TweetBox *ptr)
{
  Ui_Conversation *tmp;
  for (unsigned int i = 0; i < (unsigned) columnViewLayout->count(); i++) {
    if (columnViewLayout->itemAt(i)->widget()->objectName() == "conversation") {
      tmp = static_cast<Ui_Conversation*>(columnViewLayout->itemAt(i)->widget());
      if (tmp->getStartId() == id)
        return;
    }
  }
  for (unsigned int i = 0; i < columns.size(); i++) {
    if (columns[i]->contains(ptr)) {
      columnViewLayout->insertWidget(columnViewLayout->indexOf(columns[i])+1,new Ui_Conversation(id,this));
      return;
    }
  }
  /* If we're here we didn't come from a column, probably user profile */
  QLayoutItem *item = mainLayout->itemAtPosition(3,3);
  if (item) {
    tmp = static_cast<Ui_Conversation*>(mainLayout->itemAtPosition(3,3)->widget());
    if (tmp->getStartId() == id)
      return;
  }
  if (userProfile->contains(ptr))
    mainLayout->addWidget(new Ui_Conversation(id,this),3,3);
}

void Ui_MainWindow::closeEvent(QCloseEvent*)
{
  writeSettings();
  qApp->quit();
}

void Ui_MainWindow::inReplyTo(unsigned long long replyId, CString screen_name)
{
  if (!tweetField->isSending()) {
    tweetField->setVisible(true);
    tweetField->setInReplyTo(replyId);
    tweetField->getField()->setPlainText(screen_name.c_str());
    tweetField->getField()->setEnabled(true);
    tweetButton->setChecked(true);
    tweetField->getRetweetButton()->setVisible(false);
    tweetField->getEditRetweetButton()->setVisible(false);
    tweetField->getSendButton()->setVisible(true);
    QTextCursor cursor = tweetField->getField()->textCursor();
    cursor.setPosition(screen_name.Token(0).size() + 1);
    cursor.setPosition(tweetField->getField()->toPlainText().toLatin1().count(), QTextCursor::KeepAnchor);
    tweetField->getField()->setTextCursor(cursor);
    tweetField->getField()->setFocus();
  }
}

void Ui_MainWindow::sendRetweet(unsigned long long retweetId, CString tmp)
{
  if (!tweetField->isSending()) {
    tweetField->setVisible(true);
    tweetField->setRetweet(retweetId);
    tweetField->getField()->setPlainText(tmp.c_str());
    tweetField->getField()->setEnabled(false);
    tweetField->getRetweetButton()->setVisible(true);
    tweetField->getEditRetweetButton()->setVisible(true);
    tweetField->getSendButton()->setVisible(false);
    tweetButton->setChecked(true);
    tweetField->getRetweetButton()->setFocus();
  }
}

bool Ui_MainWindow::eventFilter(QObject * object, QEvent * event)
{
  if (object == lookupField) {
    if (event->type() == QEvent::KeyPress) {
      QKeyEvent* eventKey = static_cast<QKeyEvent*>(event);
      if (eventKey->key() == Qt::Key_Return) {
        emit openProfile(CString(lookupField->toPlainText().toLatin1().data()));
        lookupField->clear();
        return true;
      }
    }
  }
  return false;
}

void Ui_MainWindow::sendError(CString error)
{
  errorBox->setText(error.c_str());
  errorTimer->setSingleShot(true);
  errorTimer->start();
}

void Ui_MainWindow::writeSettings()
{
  if (config->open(QIODevice::Truncate|QIODevice::Text|QIODevice::ReadWrite)) {
    CString tmp = "{\"columns\":[";
    for (unsigned int i = 0;
        i < columns.size(); i++)
      tmp = tmp + columns[i]->printSettings().c_str() + ",";
    if (columns.size() > 0)
      tmp = tmp.RightChomp_n(1);
    tmp = tmp + "],";
    tmp = tmp + "\"datastore\":";
    tmp = tmp + Global::dataStore()->printSettings();
    tmp = tmp + ",";
    if (!password.isEmpty())
      tmp = tmp + "\"password\":\"" + password.toStdString() + "\",";
    tmp = tmp + "\"ssl\":" + (Global::dataStore()->getOAuth()->isSsl() ? "true" : "false") + ",";
    tmp = tmp + "\"key\":\"" + aKey + "\",\"secret\":\"" + aSec + "\",";
    tmp = tmp + "\"replies\":" + (stream_thread->ifReplies() ? "true" : "false") + ",";
    tmp = tmp + "\"tweetfield_size\":" + CString(tweetField->maximumHeight()) + ",";
    tmp = tmp + "\"stream\":" + (Global::dataStore()->getOAuth()->isStreaming() ? "true" : "false") + "}";
    config->write(tmp.c_str(), qstrlen(tmp.c_str()));
    config->close();
  } else
    sendError("Can't write to config file.");
}

void Ui_MainWindow::openUrl(QUrl url)
{
  bool displayed = false;
  if (url.path().endsWith(".png",Qt::CaseInsensitive)
   or url.path().endsWith(".jpg",Qt::CaseInsensitive)
   or url.path().endsWith(".jpeg",Qt::CaseInsensitive)
   or url.path().endsWith(".gif",Qt::CaseInsensitive)) {
    imageView->openImage(url);
    imageView->displayLink(url);
    displayed = true;
  }
  for (unsigned int i = 0; i < pictureHandlers.size(); i++) {
    if (pictureHandlers[i]->checkLink(url)) {
      pictureHandlers[i]->openImage(url);
      imageView->displayLink(url);
      displayed = true;
      break;
    }
  }
  if (!displayed)
    QDesktopServices::openUrl(url);
}

void Ui_MainWindow::getUserProfile(QString link)
{
  CString tmp = CString(link.toStdString());
  if (tmp.Left(4).Equals("http"))
    openUrl(QUrl(link));
  else if (tmp.Left(7).Equals("user_id")) {
    if (openweb)
      QDesktopServices::openUrl(QUrl(
                        CString("http://twitter.com/" +
                        tmp.Token(1,false,";").Token(1,false,":")).c_str()));
    else
      emit openProfile(tmp.Token(0,false,";").Token(1,false,":").ToULong());
  }
}

void Ui_MainWindow::markRetweeted(unsigned long long tmp)
{
  for (unsigned int i = 0; i < columns.size(); i++)
    QObject::connect(this, SIGNAL(markRetweetedCol(unsigned long long))
                    ,columns[i], SLOT(markRetweeted(unsigned long long)));
  emit markRetweetedCol(tmp);
}

void Ui_MainWindow::sendAPI(unsigned long long id, APItypes type)
{
  this->type = type;
  map<CString,CString> meh;
  QObject::connect(Global::dataStore()->getNetworkManager(), SIGNAL(finished(QNetworkReply*))
                  ,this, SLOT(onRequestCompleted(QNetworkReply*)));
  switch (type) {
    case Favorite:
      meh["id"] = CString(id);
      networkHandle = Global::dataStore()->getNetworkManager()->post(Global::dataStore()->getOAuth()->APICall("http://api.twitter.com/1.1/favorites/create.json", meh, "POST"),OAuthUtils::JoinPostParams(meh).c_str());
      break;
    case UnFavorite:
      meh["id"] = CString(id);
      networkHandle = Global::dataStore()->getNetworkManager()->post(Global::dataStore()->getOAuth()->APICall("http://api.twitter.com/1.1/favorites/destroy.json", meh, "POST"),OAuthUtils::JoinPostParams(meh).c_str());
      break;
    case Delete:
      networkHandle = Global::dataStore()->getNetworkManager()->post(Global::dataStore()->getOAuth()->APICall("http://api.twitter.com/1.1/statuses/destroy/" + CString(id) + ".json", "POST"),"");
      break;
    case Block:
      meh["id"] = CString(id);
      meh["skip_status"] = "t";
      networkHandle = Global::dataStore()->getNetworkManager()->post(Global::dataStore()->getOAuth()->APICall("http://api.twitter.com/1.1/blocks/create.json", meh, "POST"),OAuthUtils::JoinPostParams(meh).c_str());
      break;
    case Report:
      meh["id"] = CString(id);
      meh["skip_status"] = "t";
      networkHandle = Global::dataStore()->getNetworkManager()->post(Global::dataStore()->getOAuth()->APICall("http://api.twitter.com/1.1/report_spam.json", meh, "POST"),OAuthUtils::JoinPostParams(meh).c_str());
      break;
    case Follow:
      meh["user_id"] = CString(id);
      networkHandle = Global::dataStore()->getNetworkManager()->post(Global::dataStore()->getOAuth()->APICall("http://api.twitter.com/1.1/friendships/create.json", meh, "POST"),OAuthUtils::JoinPostParams(meh).c_str());
      break;
    case UnFollow:
      meh["user_id"] = CString(id);
      networkHandle = Global::dataStore()->getNetworkManager()->post(Global::dataStore()->getOAuth()->APICall("http://api.twitter.com/1.1/friendships/destroy.json", meh, "POST"),OAuthUtils::JoinPostParams(meh).c_str());
      break;
    case UnBlock:
      meh["user_id"] = CString(id);
      networkHandle = Global::dataStore()->getNetworkManager()->post(Global::dataStore()->getOAuth()->APICall("http://api.twitter.com/1.1/blocks/destroy.json", meh, "POST"),OAuthUtils::JoinPostParams(meh).c_str());
      break;
  }
}

void Ui_MainWindow::getKeys(CString key, CString secret)
{
  delete auth;
  aKey=key;
  aSec=secret;
  writeSettings();
  Global::dataStore()->setOAuth(new OAuth(aKey, aSec));
  if (columns.size() > 0) {
    QObject::connect(stream_thread, SIGNAL(sendData(CString))
                    ,this, SLOT(receiveData(CString)));
    QObject::connect(stream_thread, SIGNAL(needBackfill())
                    ,this, SLOT(needBackfill()));
    if (stream)
      stream_thread->start();
    Global::dataStore()->getOAuth()->setStream(stream);
  }
  for (unsigned int i = 0; i < columns.size(); i++)
    columns[i]->setUserId(aKey.Token(0,false,"-").ToUInt());
  userProfile->setUserId(aKey.Token(0,false,"-").ToUInt());
  Global::dataStore()->addFriend(aKey.Token(0,false,"-").ToUInt());
  settingsWindow->setSslCheckbox(Global::dataStore()->getOAuth()->isSsl());;
  needBackfill();
  Global::dataStore()->requestInfo();
}

void Ui_MainWindow::receiveData(CString data)
{
  stringstream input;
  if (buffer.size() > 0)
    input << buffer << data;
  else
    input << data;
  Object o;
  if (Object::parse(input, o)) {
    buffer = "";
    if (o.has<Array>("friends")) {
      Global::dataStore()->clearFriends();
      if (o.get<Array>("friends").has<double>(0)) {
        for (unsigned int i = 0; i < o.get<Array>("friends").size(); i++) {
          if (o.get<Array>("friends").has<double>(i))
            Global::dataStore()->addFriend((unsigned long long) o.get<Array>("friends").get<double>(i));
        }
        Global::dataStore()->addFriend(aKey.Token(0,false,"-").ToUInt());
      }
    } else if (o.has<Object>("direct_message"))
      directMessages->receiveData(o.get<Object>("direct_message"));
    else if (o.has<Object>("delete")) {
      if (o.get<Object>("delete").has<Object>("direct_message"))
        directMessages->receiveData(o);
    } else if (o.has<string>("event")) {
      if (CString(o.get<string>("event")).Equals("follow")) {
        if (o.has<Object>("source")) {
          if (o.get<Object>("source").has<double>("id")) {
            if (CString(o.get<Object>("source").get<double>("id")).Equals(aKey.Token(0,false,"-"))) {
              if (o.has<Object>("target")) {
                if (o.get<Object>("target").has<double>("id"))
                  Global::dataStore()->addFriend((unsigned long) o.get<Object>("target").get<double>("id"));
              }
            }
          }
        }
      } else if (CString(o.get<string>("event")).Equals("unfollow")) {
        if (o.has<Object>("source")) {
          if (o.get<Object>("source").has<double>("id")) {
            if (CString(o.get<Object>("source").get<double>("id")).Equals(aKey.Token(0,false,"-"))) {
              if (o.has<Object>("target")) {
                if (o.get<Object>("target").has<double>("id"))
                  Global::dataStore()->removeFriend((unsigned long) o.get<Object>("target").get<double>("id"));
              }
            }
          }
        }
      }
    }
    if (!o.has<Object>("direct_message")) {
      for (unsigned int i = 0; i < columns.size(); i++)
        columns[i]->receiveData(o);
    }
  } else
    buffer += data;
}

void Ui_MainWindow::onRequestCompleted(QNetworkReply *reply)
{
  if (reply == networkHandle) {
    reply->deleteLater();
    istringstream input(reply->readAll().data());
    Object o;
    if (Object::parse(input, o)) {
      for (unsigned int i = 0; i < columns.size(); i++)
        columns[i]->fromMainWindow(o, type);
      userProfile->fromMainWindow(o, type);
    }
  }
}

void Ui_MainWindow::needBackfill()
{
  for (unsigned int i = 0; i < columns.size(); i++)
    columns[i]->needBackfill();
  directMessages->needBackfill();
}

void Ui_MainWindow::getNewColumns()
{
  vector<Ui_Column*> newColumns;
  newColumns=settingsWindow->getRealColumns();
  for (unsigned int i = 0; i < columns.size(); i++)
    columnViewLayout->removeWidget(columns[i]);
  columns.clear();
  for (unsigned int i = 0; i < newColumns.size(); i++) {
    columns.push_back(newColumns[i]);
    columnViewLayout->addWidget(columns[i]);
  }
}

void Ui_MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
  switch (reason) {
    case QSystemTrayIcon::Trigger:
      trayIconClicked();
      break;
    default:
      break;
  }
}

void Ui_MainWindow::setPassword(QString passwd)
{
  if (passwd.isEmpty())
    this->password = "";
  else {
    CString password = passwd.toStdString(); /* So how do we create this password hash? */
    CString key = aKey + "&" + aSec;         /* Well we start off by combining the access keys */
    key = key.SHA256();                      /* We create a simple SHA256 hash out of this */
    this->password = OAuthUtils::HMACSHA1(password, key).c_str(); /* And we use that hash as key in the HMACSHA1 method */
  }                                /* This means there's a unique hash for every user, even if the password is the same */
}

void Ui_MainWindow::trayIconClicked()
{
  if (isVisible()) {
    hide();
    trayIcon->contextMenu()->actions().at(0)->setText("Restore");
  } else {
    if (password.isEmpty()) {
      show();
      trayIcon->contextMenu()->actions().at(0)->setText("Hide");
    } else
      passwordBox->show();
  }
}

void Ui_MainWindow::passwordAccepted()
{
  CString input = passwordBox->textValue().toStdString();
  CString key = aKey + "&" + aSec;
  key = key.SHA256();
  if (CString(password.toStdString()).Equals(OAuthUtils::HMACSHA1(input, key))) {
    show();
    trayIcon->contextMenu()->actions().at(0)->setText("Hide");
  } else
    passwordBox->hide();
  passwordBox->setTextValue("");
}

void Ui_MainWindow::newDM()
{
  if (!directMessages->isVisible())
    showDMs->setIcon(QIcon(":icons/dm.png"));
}

void Ui_MainWindow::toggleDMs()
{
  directMessages->show();
  showDMs->setIcon(QIcon());
}

TwStatus* Ui_MainWindow::getTweet(unsigned long long id)
{
  TwStatus *tmp = NULL;
  for (unsigned int i = 0; i < columns.size(); i++) {
    tmp = columns[i]->getTweet(id);
    if (tmp)
      return tmp;
  }
  tmp = userProfile->getTweet(id);
  return tmp;
}

void Ui_MainWindow::refreshTheme()
{
  QFile stylesheet("stylesheet.qss");
  if (stylesheet.open(QIODevice::ReadOnly|QIODevice::Text))
    qApp->setStyleSheet(stylesheet.readAll());
  style()->unpolish(qApp);
  style()->polish(qApp);
}

void Ui_MainWindow::initNotif()
{
  Notification::init("Twoons");
}

void Ui_MainWindow::openDM(CString screen_name, unsigned int user_id)
{
  directMessages->setVisible(true);
  directMessages->openDM(screen_name, user_id);
}
