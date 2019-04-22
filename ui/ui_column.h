#ifndef UI_COLUMN_H
#define UI_COLUMN_H

#include "ui_tweetbox.h"
#include "ui_filtersettings.h"
#include "ui_trendingtopic.h"
#include "jsonxx.h"
#include "datastorage.h"
#include "OAuth.h"
#include <map>
#include <vector>
#include <iostream>
/* Qt includes */
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QScrollArea>
#include <QtGui/QStatusBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>

using namespace std;
using namespace jsonxx;
/** @brief Column class
 * @section description This class is the base of every column */
class Ui_Column: public QGroupBox
{
  Q_OBJECT
public:
  /** @brief Constructor
    * @param columnName The name of the column
    * @param parent This is just required to pass to the underlying QGroupBox
    * @return The column */
  Ui_Column(QString columnName, QWidget *parent = 0);
  /** @brief Destructor */
  ~Ui_Column();
  /** @brief Used to add usernames to the user filter
   * @param user The user to filter */
  void addUserFilter(CString user) { userFilter.push_back(user); };
  /** @brief Used to add rules to the word filter
   * @param word The word to filter */
  void addWordFilter(CString word) { wordFilter.push_back(word); };
  /** @brief Used to add rules to the source filter
   * @param source The source to filter */
  void addSourceFilter(CString source) { sourceFilter.push_back(source); };
  /** @brief Are we streaming? */
  bool isStreaming() { return stream; };
  /** @brief IMPORT ALL THE SETTINGS! */
  void importSettings(Object& settings);
  /** @brief IMPORT ALL THE SETTINGS! For lazy people. */
  void importSettings(CString plainText);
  /** @brief Enable streaming on this column anyway
   * It may disable streaming again on a list though, this fully
   * depends on if you're following everyone on the list or not. */
  void setStream(bool streaming = false);
  /** @brief Used to set a list id 
   * @param id The id of the list
   * Yes a CString, I don't care tbh.. compatiblity..*/
  void setList(CString id);
  /** @brief This column requires backfilling */
  void needBackfill();
  void setUserId(unsigned int i) { user_id = i; };
  void setColumnType(unsigned int i);
  /** @brief Used to remove tweets dynamically from columns (only needed for streaming) */
  void deleteTweet(unsigned long long id);
  /** @brief Used to remove all the tweets from a user in the column */
  void deleteTweetsUser(unsigned long id);
  void deleteUser(unsigned long id);
  void fromMainWindow(Object& data, APItypes type);
  CString printSettings();
  void receiveData(Object&);
  QString getTitle() { return titleLabel->text(); };
  unsigned int getInterval() { return interval; }
  unsigned int getAlert() { return alert; };
  bool getAllowRetweets() { return allow_retweets; };
  unsigned int getType() { return type; };
  void setFilter(FilterSettings *filter);
  bool contains(QWidget *widget);
  TwStatus* getTweet(unsigned long long id);
  typedef enum {
    None         = 0,
    Blink        = 1,
    LibNotify    = 2
  } AlertType;
  typedef enum {
    HomeTimeline = 0,
    Mentions     = 1,
    List         = 2,
    Trending     = 3,
    Followers    = 4
  } ColumnType;
protected:
  /** @brief Used internally to check if something should be filtered or not
   * @param filter The vector to use for rules
   * @param rule The text to check against
   * @param contains If it should exactly match the string or not
   * @return True if it should be filtered */
  bool isFiltered(vector<CString> filter, CString rule, bool contains = true);
  /** @brief Used to add tweets */
  void Tweet(TwStatus *tweet);
  /** @brief Used to add a user */
  void User(TwUser *user);
  QWidget *centralwidget, *scrollAreaWidgetContents, *scrollAreaWidgetContents_Column;
  QVBoxLayout *tweetsLayout;
  QGridLayout *columnLayout;
  QGroupBox *groupBox_Column;
  QScrollArea *scrollArea_Column;
  QLabel *titleLabel;
  unsigned int limit;
  unsigned int user_id;
  unsigned int interval;
  unsigned int alert;
  bool stream, allow_retweets;
  VCString userFilter, wordFilter, sourceFilter;
  CString listId;
  CString since_id;
  ColumnType type;
  QNetworkReply *networkHandle;
  QNetworkReply *listMembersCall;
  QNetworkAccessManager *network;
  vector<unsigned long> listMembers;
  QTimer *timer;
private slots:
  void sendRequest();
public slots:
  /** @brief Used to set the filters from the filter setting dialog */
  void setFilters(CString field, VCString info);
  void markRetweeted(unsigned long long);
  void onRequestCompleted(QNetworkReply *);
  void newTitle(QString);
  void setInterval(int);
  void setAlert(int);
  void setAllowRetweets(bool);
  void resize(int);
  void refilter();
};

#endif // UI_COLUMN_H
