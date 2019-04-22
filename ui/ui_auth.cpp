#include "ui_auth.h"
#include "moc_ui_auth.h"
#include "global.h"

Ui_Auth::Ui_Auth()
{
  user = new OAuth();
  setWindowTitle("Twoons (authorize with twitter)");
  setVisible(true);
  setAutoFillBackground(true);
  layout = new QGridLayout(this);

  url = new QLabel(this);
  url->setTextFormat(Qt::RichText);
  url->setOpenExternalLinks(true);
  layout->addWidget(url, 0, 0, Qt::AlignTop);

  pin = new QTextEdit(this);
  pin->setMaximumSize(QSize(96, 32));
  pin->installEventFilter(this);
  layout->addWidget(pin, 1, 0, Qt::AlignCenter);
  errorBox = new QLabel(this);
  layout->addWidget(errorBox, 2, 0);
  QObject::connect(Global::dataStore()->getNetworkManager(), SIGNAL(finished(QNetworkReply*))
                  ,this, SLOT(onRequestCompleted(QNetworkReply *)));
  map<CString,CString> params;
  params["oauth_callback"] = "oob";
  networkHandle = Global::dataStore()->getNetworkManager()->post(user->APICall("https://api.twitter.com/oauth/request_token", params, "POST"),"");
}

bool Ui_Auth::eventFilter(QObject *, QEvent * event)
{
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent* eventKey = static_cast<QKeyEvent*>(event);
    if (eventKey->key() == Qt::Key_Return or eventKey->key() == Qt::Key_Enter) {
      if (pin->toPlainText().toLatin1().count() < 1)
        return true;
      else {
        sPin = pin->toPlainText().toLatin1().data();
        map<CString,CString> meh;
        meh["oauth_verifier"] = sPin;
        QObject::connect(Global::dataStore()->getNetworkManager(), SIGNAL(finished(QNetworkReply*))
                        ,this, SLOT(onRequestCompleted(QNetworkReply*)));
        networkHandle = Global::dataStore()->getNetworkManager()->post(user->APICall("https://api.twitter.com/oauth/access_token", meh), OAuthUtils::JoinPostParams(meh).c_str());
      }
      return true;
    }
  }
  return false;
}

void Ui_Auth::onRequestCompleted(QNetworkReply *reply)
{
  reply->deleteLater();
  if (reply == networkHandle) {
    if (user->isEmpty()) {
      CString data = reply->readAll().data();
      user->SetKeys(data.Token(1,false,"=").Token(0,false,"&"),data.Token(1,false,"&").Token(1,false,"="));
      url->setText(CString(CString("Please got to the following url and enter the PIN below<br>") +
        CString("<a href=\"https://twitter.com/oauth/authorize?oauth_token=" + data.Token(1,false,"=").Token(0,false,"&")) +
        CString("\">https://twitter.com/oauth/authorize?oauth_token=<br>" + data.Token(1,false,"=").Token(0,false,"&")) + "</a>").c_str());
    } else {
      CString data = reply->readAll().data();
      emit sendKeys(data.Token(1,false,"=").Token(0,false,"&"),data.Token(1,false,"&").Token(1,false,"="));
    }
  }
}
