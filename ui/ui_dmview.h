#ifndef UI_DMVIEW_H
#define UI_DMVIEW_H

#include "CString.h"
#include "ui_dmuser.h"

#include <QTextBrowser>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>

class DmView: public QGroupBox
{
  Q_OBJECT
public:
  DmView(DM dm);
  ~DmView();
  double getId() { return id; };
protected:
  QGridLayout *layout;
  QTextBrowser *text;
  QLabel      *timestamp;
  QLabel      *profilePicture;
  CString     url;
  double      id;
protected slots:
  void getProfilePicture(CString,QPixmap);
  void adjustTextField(QSizeF);
  void urlClicked(QUrl);
};

#endif // UI_DMVIEW_H
