#ifndef UI_FILTERSETTINGS_H
#define UI_FILTERSETTINGS_H

#include "CString.h"
/* Qt includes */
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QTabWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QWidget>

/** @brief The class is purely for the dialog to configure filters with */
class FilterSettings : public QTabWidget
{
  Q_OBJECT
public:
  FilterSettings(QWidget *parent = 0);
  /** @brief Used to set a field from the outside world
   * Will be used to set the field with config shit later */
  void setField(CString field, VCString info);
  void clear();
private slots:
  /** @brief Called when the apply button is hit */
  void sendUserFilter();
  /** @brief Called when the apply button is hit */
  void sendSourceFilter();
  /** @brief Called when the apply button is hit */
  void sendTextFilter();
signals:
  /** @brief Used to send the vector with rules to the column */
  void sendFilters(CString, VCString);
private:
  QWidget     *Username      ,*Source          ,*Text;
  QGridLayout *userLayout    ,*sourceLayout    ,*textLayout;
  QTextEdit   *userTextField ,*sourceTextField ,*textTextField;
};

#endif // UI_FILTERSETTINGS_H
