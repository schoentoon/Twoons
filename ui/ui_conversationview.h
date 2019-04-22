#ifndef UI_CONVERSATIONVIEW_H
#define UI_CONVERSATIONVIEW_H

#include "CString.h"
#include "OAuth.h"
#include "ui_tweetbox.h"
#include "ui_column.h"
#include "datastorage.h"
#include "actions.h"
/* Qt includes */
#include <QtGui/QGroupBox>
#include <QtGui/QGridLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QScrollArea>
#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QNetworkAccessManager>
#include <QNetworkReply>

using namespace std;
using namespace jsonxx;
/**
 * @brief User profile viewer class
 * @section description This class is used to show someone's profile
 */
class Ui_Conversation: public Ui_Column
{
  Q_OBJECT
public:
  Ui_Conversation(unsigned long long start_id, QWidget *parent = 0);
  unsigned long long getStartId() { return start_id; };
  ~Ui_Conversation();
protected:
  QPushButton *closeButton;
  vector<TwStatus*> tweetBuffer;
  unsigned long long start_id;
  unsigned long long next_id;
  void fill(bool request = false);
  QNetworkAccessManager *network;
  TwStatus* getTweet(unsigned long long id);
private slots:
  void cleanUp();
public slots:
  void onRequestCompleted(QNetworkReply *reply);
};

#endif // UI_CONVERSATIONVIEW_H
