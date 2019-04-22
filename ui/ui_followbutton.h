#ifndef UI_FOLLOWBUTTON_H
#define UI_FOLLOWBUTTON_H

#include "ui_tweetbox.h"

#include <QApplication>
#include <QGridLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QVariant>
#include <QStyle>

#include "jsonxx.h"

using namespace std;
using namespace jsonxx;

class Ui_FollowButton : public QGroupBox
{
  Q_OBJECT
public:
  Ui_FollowButton(QWidget *parent = 0);
  void setFriendShip(Object& json);
protected:
  /** @brief overriden from QGroupBox
   * this will simply make the reply button visible when hovering over it */
  void enterEvent(QEvent *);
  /** @brief overriden from QGroupBox
   * this will simply make the reply button invisible again when mouse is gone */
  void leaveEvent(QEvent *);
  QGridLayout *mainLayout;
  QPushButton *followButton;
  QPushButton *dmButton;
  QString followInfo;
  unsigned long user_id;
signals:
  void sendAPI(unsigned long long,APItypes);
  void openDM();
public slots:
  void followButtonClicked();
  void dmButtonClicked();
};

#endif // UI_FOLLOWBUTTON_H