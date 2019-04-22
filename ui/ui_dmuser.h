#ifndef UI_DMUSER_H
#define UI_DMUSER_H

#include "CString.h"
#include "jsonxx.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QDateTime>
#include <QLabel>

#include <algorithm>
#include <vector>

using namespace jsonxx;

struct DM {
  double id;
  bool sender;
  CString text;
  CString profile_picture;
  QDateTime timestamp;
  bool operator()(DM first, DM second)
  {
    return first.id > second.id;
  }
};

class DmUser: public QGroupBox
{
  Q_OBJECT
public:
  DmUser(Object& json, bool sender);
  DmUser(CString screen_name, unsigned int user_id);
  unsigned int getId() { return user_id; };
  void insertDM(Object& json);
  void deleteDM(double id);
  void sendSince(double id);
  double getLastId();
  void focus();
protected:
  void mouseReleaseEvent(QMouseEvent*);
  QGridLayout *layout;
  QLabel      *name;
  QLabel      *counter;
  unsigned int user_id;
  vector<DM>  dms;
signals:
  void sendDM(DM);
  void clearDMs(unsigned int);
  void deleteDMfromView(double);
};

#endif //UI_DMUSER_H
