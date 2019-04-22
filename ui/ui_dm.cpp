#include "ui_dm.h"
#include "moc_ui_dm.h"
#include "global.h"

Ui_DirectMessage::Ui_DirectMessage()
{
  setWindowTitle("Twoons [direct messages]");
  setMinimumSize(QSize(450, 300));
  setContentsMargins(0,0,0,0);
  setAutoFillBackground(true);
  hide();

  mainLayout = new QGridLayout(this);
  scrollArea = new QScrollArea(this);
  mainLayout->addWidget(scrollArea, 0, 0, 3, 1);
  closeButton = new QPushButton(this);
  closeButton->setText("X");
  closeButton->setMinimumSize(16,16);
  QObject::connect(closeButton, SIGNAL(clicked())
                  ,this, SLOT(hide()));
  mainLayout->addWidget(closeButton, 0, 1, Qt::AlignRight);
  scrollArea->setWidgetResizable(true);
  scrollArea->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollArea->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
  scrollArea->setMinimumWidth(225);
  scrollAreaWidgetContents = new QWidget();
  scrollArea->setWidget(scrollAreaWidgetContents);
  scrollLayout = new QVBoxLayout(scrollAreaWidgetContents);
  scrollLayout->setAlignment(Qt::AlignTop);
  scrollLayout->setContentsMargins(0,0,0,0);

  network = new QNetworkAccessManager();
  QObject::connect(network, SIGNAL(finished(QNetworkReply*))
                  ,this, SLOT(onRequestCompleted(QNetworkReply*)));
  QObject::connect(network, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>))
                  ,Global::dataStore(), SLOT(onIgnoreSSLErrors(QNetworkReply*,QList<QSslError>)));
  timer = new QTimer(this);
  timer->setInterval(1000);
  timer->setSingleShot(true);
  QObject::connect(timer, SIGNAL(timeout())
                  ,this, SLOT(makeRequests()));
  timer->start();

  dmField = new QPlainTextEdit(this);
  dmField->setMaximumHeight(75);
  dmField->installEventFilter(this);
  mainLayout->addWidget(dmField, 1, 1);
  characterCounter = new QLabel(this);
  characterCounter->setText("140");
  characterCounter->setContentsMargins(5, 5, 5, 5);
  mainLayout->addWidget(characterCounter, 1, 1, Qt::AlignLeft|Qt::AlignBottom);
  QObject::connect(dmField, SIGNAL(textChanged())
                  ,this, SLOT(textChanged()));

  scrollDm = new QScrollArea(this);
  scrollDm->setWidgetResizable(true);
  scrollDm->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  scrollDm->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollDm->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
  scrollDmWidget = new QWidget();
  scrollDm->setWidget(scrollDmWidget);
  dmLayout = new QVBoxLayout(scrollDmWidget);
  dmLayout->setAlignment(Qt::AlignTop);
  dmLayout->setContentsMargins(0,0,0,0);
  mainLayout->addWidget(scrollDm, 2, 1);
}

void Ui_DirectMessage::makeRequests()
{
  map<CString,CString> params;
  params["count"] = "50";
  params["include_entities"] = "t";
  params["skip_status"] = "t";
  if (last_id > 0)
    params["since_id"] = CString(last_id,0);
  sent = network->get(Global::dataStore()->getOAuth()->APICall("https://api.twitter.com/1.1/direct_messages/sent.json",params));
  received = network->get(Global::dataStore()->getOAuth()->APICall("https://api.twitter.com/1.1/direct_messages.json",params));
}

void Ui_DirectMessage::onRequestCompleted(QNetworkReply *reply)
{
  reply->deleteLater();
  DmUser *tmp;
  bool sender = false;
  bool added = false;
  bool send = false;
  if (reply == received)
    sender = false;
  else if (reply == sent)
    sender = true;
  else if (reply == sending)
    send = true;
  stringstream input;
  input << reply->readAll().data();
  if (!send) {
    Array a;
    unsigned int user_id = 0;
    if (Array::parse(input, a)) {
      for (int i = a.size() - 1; i > -1; i--) {
        if (sender) {
          if (a.get<Object>(i).has<Object>("recipient")) {
            if (a.get<Object>(i).get<Object>("recipient").has<double>("id"))
              user_id = a.get<Object>(i).get<Object>("recipient").get<double>("id");
          }
        } else {
          if (a.get<Object>(i).has<Object>("sender")) {
            if (a.get<Object>(i).get<Object>("sender").has<double>("id"))
              user_id = a.get<Object>(i).get<Object>("sender").get<double>("id");
          }
        }
        if (a.get<Object>(i).has<double>("id")) {
          double tmp_id = a.get<Object>(i).get<double>("id");
          if (tmp_id > last_id)
            last_id = tmp_id;
        }
        for (unsigned int j = 0; j < people.size(); j++) {
          if (people[j]->getId() == user_id) {
            people[j]->insertDM(a.get<Object>(i));
            scrollLayout->insertWidget(0, people[j]);
            added = true;
            emit notifyDM();
            if (people[j]->getId() == user) {
              if (dmLayout->count() > 0) {
                QLayoutItem *item = dmLayout->itemAt(0);
                if (item) {
                  if (item->widget()->objectName() == "DmView") {
                    DmView *dm = static_cast<DmView*>(item->widget());
                    people[j]->sendSince(dm->getId());
                  }
                }
              }
            }
            break;
          }
        }
        if (!added) {
          tmp = new DmUser(a.get<Object>(i), sender);
          if (tmp->getId() != Global::dataStore()->getOAuth()->getUserId()) {
            QObject::connect(tmp, SIGNAL(clearDMs(unsigned int))
                            ,this, SLOT(clearDMs(unsigned int)));
            QObject::connect(tmp, SIGNAL(sendDM(DM))
                            ,this, SLOT(getDM(DM)));
            QObject::connect(tmp, SIGNAL(deleteDMfromView(double))
                            ,this, SLOT(deleteDM(double)));
            scrollLayout->insertWidget(0, tmp);
            people.push_back(tmp);
            emit notifyDM();
          } else
            delete tmp;
        }
        added = false;
      }
    }
  } else {
    if (reply->error() == QNetworkReply::NoError) {
      Object o;
      if (Object::parse(input, o)) {
        if (o.has<Object>("recipient")) {
          if (o.get<Object>("recipient").has<double>("id")) {
            for (unsigned int i = 0; i < people.size(); i++) {
              if (people[i]->getId() == (unsigned int) o.get<Object>("recipient").get<double>("id")) {
                people[i]->insertDM(o);
                if (people[i]->getId() == user) {
                  if (dmLayout->count() > 0) {
                    QLayoutItem *item = dmLayout->itemAt(0);
                    if (item) {
                      if (item->widget()->objectName() == "DmView") {
                        DmView *dm = static_cast<DmView*>(item->widget());
                        people[i]->sendSince(dm->getId());
                      }
                    }
                  }
                }
                added = true;
                break;
              }
            }
          }
        }
      }
      if (added) {
        dmField->clear();
        dmField->setEnabled(true);
      } else {
        tmp = new DmUser(o, true);
        QObject::connect(tmp, SIGNAL(clearDMs(unsigned int))
                        ,this, SLOT(clearDMs(unsigned int)));
        QObject::connect(tmp, SIGNAL(sendDM(DM))
                        ,this, SLOT(getDM(DM)));
        QObject::connect(tmp, SIGNAL(deleteDMfromView(double))
                        ,this, SLOT(deleteDM(double)));
        scrollLayout->insertWidget(0, tmp);
        people.push_back(tmp);
      }
    } else
      dmField->setEnabled(true);
    dmField->setFocus();
  }
}

void Ui_DirectMessage::getDM(DM dm)
{
  dmLayout->insertWidget(0, new DmView(dm));
}

void Ui_DirectMessage::clearDMs(unsigned int newUser)
{
  user = newUser;
  QLayoutItem* item;
  while ((item = dmLayout->takeAt(0)) != 0) {
    delete item->widget();
    delete item;
  }
}

bool Ui_DirectMessage::eventFilter(QObject *, QEvent *event)
{
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent* eventKey = static_cast<QKeyEvent*>(event);
    if (eventKey->key() == Qt::Key_Return or eventKey->key() == Qt::Key_Enter) {
      if (eventKey->modifiers() == Qt::ShiftModifier)
        return false;
      else if (dmField->toPlainText().toLatin1().count() <= 140) {
        sendDM(dmField->toPlainText().toLatin1().data());
        return true;
      }
    }
  }
  return false;
}

void Ui_DirectMessage::sendDM(CString msg)
{
  map<CString,CString> meh;
  meh["text"] = msg;
  meh["user_id"] = CString(user);
  dmField->setEnabled(false);
  sending = network->post(Global::dataStore()->getOAuth()->APICall("https://api.twitter.com/1.1/direct_messages/new.json",meh,"POST"),OAuthUtils::JoinPostParams(meh,true).c_str());
}

void Ui_DirectMessage::receiveData(Object& json)
{
  bool added = false;
  bool sender = false;
  if (json.has<Object>("delete")) {
    if (json.get<Object>("delete").has<Object>("direct_message")) {
      for (unsigned int i = 0; i < people.size(); i++) {
        if (json.get<Object>("delete").get<Object>("direct_message").has<double>("id"))
          people[i]->deleteDM(json.get<Object>("delete").get<Object>("direct_message").get<double>("id"));
        break;
      }
    }
    return;
  }
  if (json.has<double>("id")) {
    double tmp_id = json.get<double>("id");
    if (tmp_id > last_id)
      last_id = tmp_id;
  }
  if (json.has<Object>("recipient")) {
    if (json.get<Object>("recipient").has<double>("id")) {
      for (unsigned int i = 0; i < people.size(); i++) {
        if (people[i]->getId() == (unsigned int) json.get<Object>("recipient").get<double>("id")) {
          people[i]->insertDM(json);
          added = true;
          scrollLayout->insertWidget(0, people[i]);
          if (people[i]->getId() == user) {
            if (dmLayout->count() > 0) {
              QLayoutItem *item = dmLayout->itemAt(0);
              if (item) {
                if (item->widget()->objectName() == "DmView") {
                  DmView *dm = static_cast<DmView*>(item->widget());
                  people[i]->sendSince(dm->getId());
                }
              }
            }
          }
          break;
        }
      }
    }
  }
  if (json.has<Object>("sender")) {
    if (json.get<Object>("sender").has<double>("id")) {
      for (unsigned int i = 0; i < people.size(); i++) {
        if (people[i]->getId() == (unsigned int) json.get<Object>("sender").get<double>("id")) {
          people[i]->insertDM(json);
          emit notifyDM();
          added = true;
          scrollLayout->insertWidget(0, people[i]);
          if (people[i]->getId() == user) {
            if (dmLayout->count() > 0) {
              QLayoutItem *item = dmLayout->itemAt(0);
              if (item) {
                if (item->widget()->objectName() == "DmView") {
                  DmView *dm = static_cast<DmView*>(item->widget());
                  people[i]->sendSince(dm->getId());
                }
              }
            }
          }
          break;
        }
      }
    }
  }
  if (!added) {
    DmUser *tmp;
    if (json.has<Object>("sender")) {
      if (json.get<Object>("sender").has<double>("id")) {
        if ((unsigned int) json.get<Object>("sender").get<double>("id") == Global::dataStore()->getOAuth()->getUserId())
          sender = true;
        else
          sender = false;
      }
    }
    tmp = new DmUser(json, sender);
    QObject::connect(tmp, SIGNAL(clearDMs(unsigned int))
                    ,this, SLOT(clearDMs(unsigned int)));
    QObject::connect(tmp, SIGNAL(sendDM(DM))
                    ,this, SLOT(getDM(DM)));
    QObject::connect(tmp, SIGNAL(deleteDMfromView(double))
                    ,this, SLOT(deleteDM(double)));
    scrollLayout->insertWidget(0, tmp);
    people.push_back(tmp);
    if (!sender)
      emit notifyDM();
  }
}

void Ui_DirectMessage::textChanged()
{
  if (dmField->toPlainText().toLatin1().count() > 140)
    characterCounter->setText(CString("<span style=\"color:#ff0000;font-weight:600;\">" + CString(140 - dmField->toPlainText().toLatin1().count()) + "</span>").c_str());
  else
    characterCounter->setText(CString(140 - dmField->toPlainText().toLatin1().count()).c_str());
}

void Ui_DirectMessage::needBackfill()
{
  makeRequests();
}

void Ui_DirectMessage::deleteDM(double id)
{
  if (dmLayout->count() > 0) {
    for (unsigned int i = 0; i < (unsigned) dmLayout->count(); i++) {
      QLayoutItem *item = dmLayout->itemAt(i);
      if (item->widget()->objectName() == "DmView") {
        DmView *dm = static_cast<DmView*>(item->widget());
        if (dm->getId() == id)
          item->widget()->deleteLater();
      }
    }
  }
}

void Ui_DirectMessage::openDM(CString screen_name, unsigned int user_id)
{
  bool gotit = false;
  for (unsigned int i = 0; i < people.size(); i++) {
    if (people[i]->getId() == user_id) {
      people[i]->focus();
      gotit = true;
    }
  }
  if (!gotit) {
    DmUser *tmp = new DmUser(screen_name, user_id);
    QObject::connect(tmp, SIGNAL(clearDMs(unsigned int))
                    ,this, SLOT(clearDMs(unsigned int)));
    QObject::connect(tmp, SIGNAL(sendDM(DM))
                    ,this, SLOT(getDM(DM)));
    QObject::connect(tmp, SIGNAL(deleteDMfromView(double))
                    ,this, SLOT(deleteDM(double)));
    scrollLayout->insertWidget(0, tmp);
    people.push_back(tmp);
    tmp->focus();
  } 
}
