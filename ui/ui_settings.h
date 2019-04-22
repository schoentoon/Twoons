#ifndef UI_SETTINGS_H
#define UI_SETTINGS_H

#include "CString.h"
#include "datastorage.h"
#include "stream_thread.h"
#include "ui_column.h"
#include "ui_errorbox.h"
#include "ui_tweetfield.h"
#include "ui_filtersettings.h"
/* Qt includes */
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QWidget>
#include <QtGui/QTabWidget>
#include <QtGui/QListWidget>
#include <QtGui/QCheckBox>
#include <QtGui/QScrollArea>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QGroupBox>
#include <QtGui/QLineEdit>
#include <QtGui/QKeyEvent>
#include <QtGui/QComboBox>
#include <QNetworkReply>
#include <QDesktopWidget>
#include <iostream>
#include <vector>

using namespace std;
/** @brief The gui used to authorize
 * @section description This class is a part of the autorization */
class Ui_Settings: public QTabWidget
{
  Q_OBJECT
public:
  /** @brief Constructor */
  Ui_Settings();
  /** @brief Hand over the stream thread */
  void setStreamThread(StreamThread *stream_thread);
  /** @brief Hand over the columns */
  void setRealColumns(vector<Ui_Column*> columns);
  /** @brief Hand over the main tweet field */
  void setTweetField(TweetField*);
  /** @brief Used to get the /new/ real columns back. */
  vector<Ui_Column*> getRealColumns() { return realColumns; };
  bool eventFilter(QObject *object, QEvent *event);
  void setSslCheckbox(bool newState) { httpsOnly->setChecked(newState); };
  void setStreamState(bool newState) { toggleStream(newState); };
protected:
  void hideEvent(QHideEvent *event);
  void getColumnSettings(unsigned int);
  void mousePressEvent(QMouseEvent *event) { dragStart = event->pos(); raise(); };
  void mouseMoveEvent(QMouseEvent *event) { setGeometry(QRect(geometry().topLeft() + event->pos() - dragStart, rect().size())); };
  QPoint dragStart;
  StreamThread *stream_thread;
  TweetField *tweet_field;
  vector<Ui_Column*> realColumns;
  /* Column settings */
  QWidget *columnsWidget;
  QGridLayout *columnsLayout;
  QScrollArea *columnsScrollArea;
  QWidget *columnsScrollAreaWidgetContents;
  QHBoxLayout *columnsHorizontalLayout;
  QPushButton *columnsNew;
  QGroupBox *columnSettingsBox;
  QGridLayout *gridLayout;
  QLabel *columnsNameLabel;
  QLineEdit *columnsNameEdit;
  QLabel *columnsIntervalLabel;
  QSpinBox *columnsIntervalBox;
  QComboBox *alertOnNewTweets;
  QCheckBox *showRetweets;
  QLabel *columnsWidthLabel;
  QSlider *columnsWidthSlider;
  QPushButton *closeColumn;
  QPushButton *moveColumnLeft;
  QPushButton *moveColumnRight;
  vector<QPushButton*> columns;
  QPushButton* activeColumn;
  QPushButton* filterButton;
  FilterSettings* filters;
  Ui_Column* realActiveColumn;
  /* These are for the new column part */
  QTabWidget *newColumnTabWidget;
  QWidget *newColumnWidget;
  QGridLayout *newColumnLayout;
  QPushButton *newMentionsColumn;
  QPushButton *newAllFriendsColumn;
  QPushButton *newTrendingsColumn;
  QPushButton *newFollowersColumn;
  QWidget *newColumnListWidget;
  QGridLayout *newColumnListLayout;
  QListWidget *listsView;
  QNetworkReply *listsCall;
  QLineEdit *userNameList;
  /* !Column settings */
  /* Streaming settings */
  QWidget *streamingWidget;
  QGridLayout *streamingSettings;
  QCheckBox *streamingEnabled;
  QCheckBox *streamingReplies;
  /* !Streaming settings */
  /* UI settings */
  QWidget *uiWidget;
  QGridLayout *uiLayout;
  QPushButton *refreshTheme;
  QLabel *fieldHeightLabel;
  QSlider *fieldHeight;
  QLabel *pictureLabel;
  QComboBox *picturePicker;
  /* !UI settings */
  /* Global filters */
  FilterSettings *globalFilters;
  /* !Global filters */
  /* Security settings */
  QWidget *secWidget;
  QGridLayout *secLayout;
  QCheckBox *httpsOnly;
  QLabel *secLabel;
  QLabel *errorLabel;
  errorBoxT *errorT;
  QLineEdit *passwordBox0, *passwordBox1;
  QPushButton *confPassword;
  /* !Security settings */
private:
  void disconnectSlots();
  void connect(Ui_Column*);
private slots:
  void toggleStream(bool);
  void clickedColumn();
  void newColumn();
  void newMentionsColumnClicked();
  void newAllFriendsColumnClicked();
  void newTrendingsColumnClicked();
  void newFollowersColumnClicked();
  void doubleClickedList(QListWidgetItem* item);
  void onRequestCompleted(QNetworkReply* reply);
  void closeColumnClicked();
  void moveLeftClicked();
  void moveRightClicked();
  void applyPassword();
  void setSsl(bool);
  void setUploadingService(int);
  void refreshUI();
signals:
  void saveSettings();
  void goGetNewColumns();
  void setPassword(QString);
};

#endif // UI_SETTINGS_H
