/**
 * @mainpage Twoons reference Manual
 *
 * @section description Description
 *
 * This will be a advanced Qt twitter client
 * It'll obviously feature the streaming api
 * and A LOT client side features like filtering
 */
#include <QApplication>
#include "global.h"
#include "debug.h"

int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  QStringList arguments = QCoreApplication::arguments();
  if (arguments.size() > 1) {
    if (arguments.contains("-v")) {
      cout << VERSION << endl;
      return 0;
    } else if (arguments.contains("-t")) {
      QFile* config = NULL;
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
      if (config->open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream textStream(config);
        QString settings;
        while (!textStream.atEnd())
          input << textStream.readLine().toStdString();
        config->close();
        config->deleteLater();
      }
      Object o;
      if (Object::parse(input, o) and o.has<string>("key") and o.has<string>("secret")) {
        Global::dataStore()->setOAuth(new OAuth(o.get<string>("key"), o.get<string>("secret")));
        Global::dataStore()->setParent(&app);
        if (o.has<Object>("datastore"))
          Global::dataStore()->importSettings(o.get<Object>("datastore"));
        TweetField* tweetFieldWindow = new TweetField(NULL);
        tweetFieldWindow->setWindowTitle("Tweet");
        tweetFieldWindow->setWindowIcon(QIcon(":/icons/32x32.png"));
        tweetFieldWindow->resize(400, 100);
        tweetFieldWindow->quitAfterTweet(true);
        tweetFieldWindow->move((QApplication::desktop()->width()/2)-(tweetFieldWindow->size().width()/2), 100);
        app.setQuitOnLastWindowClosed(true);
        tweetFieldWindow->show();
        return app.exec();
      }
      return 1;
    }
    Debug::SetDebug(arguments.contains("-d"));
  }
  app.setQuitOnLastWindowClosed(false);
  Ui_MainWindow* win = Global::Get();
  win->show();
  return app.exec();
}
