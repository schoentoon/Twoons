#ifndef UI_ERRORBOX_H
#define UI_ERRORBOX_H

#include "CString.h"
/* Qt includes */
#include <QObject>
#include <QTimer>

class errorBoxT: public QObject
{
  Q_OBJECT
public:
  errorBoxT();
public slots:
  /**
   * @brief Used to display an error in the error box
   * @param errormsg The message to display
   */
  void Error(QString errormsg);
private slots:
  void clear();
private:
  QTimer* timer;
signals:
  void getError(QString);
};

#endif // UI_ERRORBOX_H
