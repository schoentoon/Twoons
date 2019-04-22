#ifndef UI_TRENDINGTOPIC_H
#define UI_TRENDINGTOPIC_H

/* Qt includes */
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <iostream>

using namespace std;

class TrendingTopic: public QGroupBox
{
  Q_OBJECT
public:
  TrendingTopic(QString text);
  ~TrendingTopic();
protected:
  QGridLayout *layout;
  QLabel *text;
};

#endif // UI_TRENDINGTOPIC_H
