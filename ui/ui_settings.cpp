#include "ui_settings.h"
#include "moc_ui_settings.h"
#include "global.h"

Ui_Settings::Ui_Settings()
{
  setWindowTitle("Twoons [settings]");
  setMinimumSize(QSize(400, 300));
  hide();
  setMovable(true);
  stream_thread = NULL;
  setAutoFillBackground(true);

  /* Column settings */
  activeColumn = NULL;
  columnsWidget = new QWidget();
  columnsLayout = new QGridLayout(columnsWidget);
  addTab(columnsWidget, "Column settings");

  columnsScrollArea = new QScrollArea(this);
  QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(columnsScrollArea->sizePolicy().hasHeightForWidth());
  columnsScrollArea->setSizePolicy(sizePolicy);
  columnsScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  columnsScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  columnsScrollArea->setWidgetResizable(true);
  columnsScrollAreaWidgetContents = new QWidget();
  columnsScrollAreaWidgetContents->setGeometry(QRect(0, 0, 410, 60));
  columnsHorizontalLayout = new QHBoxLayout(columnsScrollAreaWidgetContents);
  columnsHorizontalLayout->setAlignment(Qt::AlignLeft);
  columnsLayout->addWidget(columnsScrollArea, 0, 0, 1, 1);
  columnsScrollArea->setWidget(columnsScrollAreaWidgetContents);

  columnsNew = new QPushButton(this);
  QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Minimum);
  sizePolicy2.setHorizontalStretch(0);
  sizePolicy2.setVerticalStretch(0);
  sizePolicy2.setHeightForWidth(columnsNew->sizePolicy().hasHeightForWidth());
  columnsNew->setSizePolicy(sizePolicy2);
  columnsNew->setText("+");
  columnsNew->setCheckable(true);
  columnsNew->setMinimumSize(QSize(30, 40));
  columnsHorizontalLayout->addWidget(columnsNew);
  QObject::connect(columnsNew, SIGNAL(clicked())
                  ,this, SLOT(newColumn()));

  columnSettingsBox = new QGroupBox(this);
  QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Expanding);
  sizePolicy3.setHorizontalStretch(0);
  sizePolicy3.setVerticalStretch(0);
  sizePolicy3.setHeightForWidth(columnSettingsBox->sizePolicy().hasHeightForWidth());
  columnSettingsBox->setSizePolicy(sizePolicy3);
  gridLayout = new QGridLayout(columnSettingsBox);

  closeColumn = new QPushButton(this);
  closeColumn->setText("X");
  closeColumn->setMaximumSize(24,24);
  gridLayout->addWidget(closeColumn, 0, 1, 1, 1, Qt::AlignRight);
  QObject::connect(closeColumn, SIGNAL(clicked())
                  ,this, SLOT(closeColumnClicked()));

  moveColumnLeft = new QPushButton(this);
  moveColumnLeft->setText("<-");
  moveColumnLeft->setMaximumSize(24,24);
  gridLayout->addWidget(moveColumnLeft, 3, 0, 1, 1);
  QObject::connect(moveColumnLeft, SIGNAL(clicked())
                  ,this, SLOT(moveLeftClicked()));

  moveColumnRight = new QPushButton(this);
  moveColumnRight->setText("->");
  moveColumnRight->setMaximumSize(24,24);
  gridLayout->addWidget(moveColumnRight, 3, 0, 1, 1, Qt::AlignRight);
  QObject::connect(moveColumnRight, SIGNAL(clicked())
                  ,this, SLOT(moveRightClicked()));

  columnsNameLabel = new QLabel(columnSettingsBox);
  columnsNameLabel->setText("Name:");
  gridLayout->addWidget(columnsNameLabel, 1, 0, 1, 1);

  columnsNameEdit = new QLineEdit(columnSettingsBox);
  columnsNameEdit->setMaxLength(128);
  columnsNameEdit->setDragEnabled(true);
  gridLayout->addWidget(columnsNameEdit, 1, 1, 1, 1);

  columnsIntervalLabel = new QLabel(columnSettingsBox);
  columnsIntervalLabel->setText("Interval:");
  gridLayout->addWidget(columnsIntervalLabel, 2, 0, 1, 1);

  columnsIntervalBox = new QSpinBox(columnSettingsBox);
  columnsIntervalBox->setMinimum(10);
  columnsIntervalBox->setMaximum(300);
  columnsIntervalBox->setValue(30);
  gridLayout->addWidget(columnsIntervalBox, 2, 1, 1, 1);

  alertOnNewTweets = new QComboBox(columnSettingsBox);
  alertOnNewTweets->addItem("None");
  alertOnNewTweets->addItem("Blink taskbar");
  alertOnNewTweets->addItem("Popup notification");
  gridLayout->addWidget(alertOnNewTweets, 3, 1, 1, 1);

  showRetweets = new QCheckBox(columnSettingsBox);
  showRetweets->setLayoutDirection(Qt::RightToLeft);
  showRetweets->setText("Show retweets in this column:");
  gridLayout->addWidget(showRetweets, 4, 1, 1, 1);

  columnsWidthLabel = new QLabel(columnSettingsBox);
  columnsWidthLabel->setText("Width:");
  gridLayout->addWidget(columnsWidthLabel, 5, 0, 1, 1);

  columnsWidthSlider = new QSlider();
  columnsWidthSlider->setMinimum(290);
  columnsWidthSlider->setMaximum(QApplication::desktop()->screenGeometry().width());
  columnsWidthSlider->setOrientation(Qt::Horizontal);
  gridLayout->addWidget(columnsWidthSlider, 5, 1, 1, 1);

  filterButton = new QPushButton(this);
  filterButton->setText("Filters");
  filterButton->setCheckable(true);
  gridLayout->addWidget(filterButton, 6, 0, 1, 1);

  filters = new FilterSettings(this);
  filters->setVisible(false);
  QObject::connect(filterButton, SIGNAL(clicked(bool))
                  ,filters, SLOT(setVisible(bool)));
  gridLayout->addWidget(filters, 6, 1, 2, 1);

  columnsLayout->addWidget(columnSettingsBox, 1, 0, 1, 1);

  newColumnTabWidget = new QTabWidget(this);
  newColumnTabWidget->setVisible(false);
  columnsLayout->addWidget(newColumnTabWidget, 1, 0, 1, 1);

  newColumnWidget = new QWidget();
  newColumnLayout = new QGridLayout(newColumnWidget);
  newAllFriendsColumn = new QPushButton("All friends");
  QObject::connect(newAllFriendsColumn, SIGNAL(clicked())
                  ,this, SLOT(newAllFriendsColumnClicked()));
  newColumnLayout->addWidget(newAllFriendsColumn, 0, 0, 1, 1);
  newMentionsColumn = new QPushButton("Mentions");
  QObject::connect(newMentionsColumn, SIGNAL(clicked())
                  ,this, SLOT(newMentionsColumnClicked()));
  newColumnLayout->addWidget(newMentionsColumn, 1, 0, 1, 1);
  newTrendingsColumn = new QPushButton("Trending Topics");
  QObject::connect(newTrendingsColumn, SIGNAL(clicked())
                  ,this, SLOT(newTrendingsColumnClicked()));
  newColumnLayout->addWidget(newTrendingsColumn, 2, 0, 1, 1);
  newFollowersColumn = new QPushButton("New followers");
  QObject::connect(newFollowersColumn, SIGNAL(clicked())
                  ,this, SLOT(newFollowersColumnClicked()));
  newColumnLayout->addWidget(newFollowersColumn, 3, 0, 1, 1);
  newColumnTabWidget->addTab(newColumnWidget, "General");

  newColumnListWidget = new QWidget();
  newColumnListLayout = new QGridLayout(newColumnListWidget);
  userNameList = new QLineEdit(this);
  userNameList->installEventFilter(this);
  newColumnListLayout->addWidget(userNameList, 0, 0, 1, 1);
  listsView = new QListWidget(newColumnListWidget);
  newColumnListLayout->addWidget(listsView, 1, 0, 1, 1);
  newColumnTabWidget->addTab(newColumnListWidget, "Lists");
  QObject::connect(listsView, SIGNAL(itemDoubleClicked(QListWidgetItem*))
                  ,this, SLOT(doubleClickedList(QListWidgetItem*)));
  /* !Column settings */

  /* Streaming settings */
  streamingWidget = new QWidget();
  streamingSettings = new QGridLayout(streamingWidget);
  addTab(streamingWidget, "Streaming settings");
  streamingEnabled = new QCheckBox("Enable streaming",this);
  streamingSettings->addWidget(streamingEnabled, 0, 0);
  QObject::connect(streamingEnabled, SIGNAL(toggled(bool))
                  ,this, SLOT(toggleStream(bool)));
  streamingReplies = new QCheckBox("Include @replies from your friends to others.",this);
  streamingSettings->addWidget(streamingReplies, 1, 0);
  /* !Streaming settings */

  /* UI settings */
  uiWidget = new QWidget();
  uiLayout = new QGridLayout(uiWidget);
  addTab(uiWidget, "UI settings");
  fieldHeightLabel = new QLabel();
  fieldHeightLabel->setText("Tweet field height");
  uiLayout->addWidget(fieldHeightLabel, 0, 0);
  fieldHeight = new QSlider();
  fieldHeight->setMinimum(65);
  fieldHeight->setMaximum(150);
  fieldHeight->setOrientation(Qt::Horizontal);
  uiLayout->addWidget(fieldHeight, 0, 1);

  pictureLabel = new QLabel();
  pictureLabel->setText("Uploading service");
  uiLayout->addWidget(pictureLabel, 1, 0);
  picturePicker = new QComboBox(this);
  vector<PictureUpFramework*> uploaders = PictureUpFramework::init(0);
  picturePicker->addItem("pic.twitter.com",0);
  for (unsigned int i = 0; i < uploaders.size(); i++) {
    picturePicker->addItem(uploaders[i]->getName(),uploaders[i]->getUniqueId());
    delete uploaders[i];
  }
  uploaders.clear();
  QObject::connect(picturePicker, SIGNAL(currentIndexChanged(int))
                  ,this, SLOT(setUploadingService(int)));
  picturePicker->setCurrentIndex(Global::dataStore()->getUploadingService());
  uiLayout->addWidget(picturePicker, 1, 1);
  refreshTheme = new QPushButton(this);
  refreshTheme->setText("Refresh theme");
  QObject::connect(refreshTheme, SIGNAL(clicked())
                  ,this, SLOT(refreshUI()));
  uiLayout->addWidget(refreshTheme, 2, 1);
  /* !UI settings */

  /* Global filters */
  globalFilters = new FilterSettings(this);
  QObject::connect(globalFilters, SIGNAL(sendFilters(CString,VCString))
                  ,Global::dataStore(), SLOT(setFilters(CString,VCString)));
  globalFilters->setField("user",Global::dataStore()->getUserFilters());
  globalFilters->setField("source",Global::dataStore()->getSourceFilters());
  globalFilters->setField("text",Global::dataStore()->getWordFilters());
  addTab(globalFilters, "Global Filters");
  /* !Global filters */

  /* Security settings */
  secWidget = new QWidget();
  secLayout = new QGridLayout(secWidget);
  addTab(secWidget, "Security settings");
  httpsOnly = new QCheckBox(this);
  httpsOnly->setText("Make all requests over https");
  QObject::connect(httpsOnly, SIGNAL(toggled(bool))
                  ,this, SLOT(setSsl(bool)));
  secLayout->addWidget(httpsOnly, 0, 0);
  secLabel = new QLabel("Password:");
  secLayout->addWidget(secLabel, 1, 0);
  passwordBox0 = new QLineEdit(this);
  passwordBox1 = new QLineEdit(this);
  passwordBox0->setEchoMode(QLineEdit::Password);
  passwordBox1->setEchoMode(QLineEdit::Password);
  secLayout->addWidget(passwordBox0, 2, 0);
  secLayout->addWidget(passwordBox1, 3, 0);
  confPassword = new QPushButton(this);
  confPassword->setText("Apply");
  secLayout->addWidget(confPassword, 4, 0, Qt::AlignRight);
  QObject::connect(confPassword, SIGNAL(clicked())
                  ,this, SLOT(applyPassword()));
  errorLabel = new QLabel(this);
  secLayout->addWidget(errorLabel, 5, 0, Qt::AlignLeft);
  errorT = new errorBoxT();
  QObject::connect(errorT, SIGNAL(getError(QString))
                  ,errorLabel, SLOT(setText(QString)));
  /* !Security settings */
}

void Ui_Settings::toggleStream(bool newState)
{
  streamingEnabled->setChecked(newState);
  streamingReplies->setEnabled(newState);
}

void Ui_Settings::connect(Ui_Column *column)
{
  QObject::connect(columnsNameEdit, SIGNAL(textChanged(QString))
                  ,column, SLOT(newTitle(QString)));
  QObject::connect(columnsIntervalBox, SIGNAL(valueChanged(int))
                  ,column, SLOT(setInterval(int)));
  QObject::connect(alertOnNewTweets, SIGNAL(activated(int))
                  ,column, SLOT(setAlert(int)));
  QObject::connect(showRetweets, SIGNAL(toggled(bool))
                  ,column, SLOT(setAllowRetweets(bool)));
  QObject::connect(columnsWidthSlider, SIGNAL(sliderMoved(int))
                  ,column, SLOT(resize(int)));
  QObject::connect(filters, SIGNAL(sendFilters(CString,VCString))
                  ,column, SLOT(setFilters(CString,VCString)));
}

void Ui_Settings::disconnectSlots()
{
  columnsNameEdit->disconnect();
  columnsIntervalBox->disconnect();
  alertOnNewTweets->disconnect();
  showRetweets->disconnect();
  columnsWidthSlider->disconnect();
  filters->disconnect();
  filters->clear();
}

bool Ui_Settings::eventFilter(QObject*, QEvent *event)
{
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent* eventKey = static_cast<QKeyEvent*>(event);
    if (eventKey->key() == Qt::Key_Return or eventKey->key() == Qt::Key_Enter) {
      map<CString,CString> params;
      params["screen_name"] = userNameList->text().toLatin1().data();
      listsCall = Global::dataStore()->getNetworkManager()->get(Global::dataStore()->getOAuth()->APICall("http://api.twitter.com/1.1/lists/list.json",params,"GET"));
      listsView->clear();
      userNameList->clear();
    }
  }
  return false;
}

void Ui_Settings::setStreamThread(StreamThread *stream_thread)
{
  this->stream_thread = stream_thread;
  streamingReplies->setChecked(stream_thread->ifReplies());
}

void Ui_Settings::setTweetField(TweetField *tweet_field)
{
  this->tweet_field = tweet_field;
  fieldHeight->setSliderPosition(tweet_field->maximumHeight());
  QObject::connect(fieldHeight, SIGNAL(sliderMoved(int))
                  ,tweet_field, SLOT(resizeField(int)));
}

void Ui_Settings::hideEvent(QHideEvent*)
{
  if (stream_thread != NULL) {
    stream_thread->setReplies(streamingReplies->isChecked());
    Global::dataStore()->getOAuth()->setStream(streamingEnabled->isChecked());
    if (streamingEnabled->isChecked())
      stream_thread->start();
    else
      stream_thread->stop();
    for (unsigned int i = 0; i < realColumns.size(); i++)
      realColumns[i]->setStream(streamingEnabled->isChecked());
  }
  emit saveSettings();
}

void Ui_Settings::setRealColumns(vector<Ui_Column*> columns)
{
  QPushButton *tmp;
  realColumns = columns;
  for (unsigned int i = 0; i < columns.size(); i++) {
    tmp = new QPushButton(this);
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    sizePolicy.setHeightForWidth(tmp->sizePolicy().hasHeightForWidth());
    tmp->setSizePolicy(sizePolicy);
    tmp->setText(" ");
    tmp->setMinimumSize(QSize(30, 40));
    tmp->setCheckable(true);
    this->columns.push_back(tmp);
    QObject::connect(this->columns[i], SIGNAL(clicked())
                    ,this, SLOT(clickedColumn()));
    columnsHorizontalLayout->insertWidget(columnsHorizontalLayout->count() - 1,this->columns[i]);
  }
  this->columns[0]->setChecked(true);
  clickedColumn();
  connect(realActiveColumn);
}

void Ui_Settings::clickedColumn()
{
  newColumnTabWidget->setVisible(false);
  columnSettingsBox->setVisible(true);
  if (activeColumn != NULL) {
    if (!activeColumn->isChecked()) {
      activeColumn->setChecked(true);
      return;
    }
    activeColumn->setChecked(false);
    disconnectSlots();
  }
  for (unsigned int i = 0; i < columns.size(); i++) {
    if (columns[i]->isChecked()) {
      if (i == 0)
        moveColumnLeft->setVisible(false);
      else
        moveColumnLeft->setVisible(true);
      if (i == columns.size() - 1)
        moveColumnRight->setVisible(false);
      else
        moveColumnRight->setVisible(true);
      getColumnSettings(i);
      activeColumn = columns[i];
      realActiveColumn = realColumns[i];
      connect(realActiveColumn);
      realActiveColumn->setFilter(filters);
      if (realActiveColumn->getType() == 3
       or realActiveColumn->getType() == 4) {
        filterButton->setChecked(false);
        filterButton->setEnabled(false);
        filters->setVisible(false);
        if (realActiveColumn->getType() == 3)
          alertOnNewTweets->setEnabled(false);
        showRetweets->setEnabled(false);
      } else {
        filterButton->setEnabled(true);
        alertOnNewTweets->setEnabled(true);
        showRetweets->setEnabled(true);
      }
      break;
    }
  }
}

void Ui_Settings::getColumnSettings(unsigned int column)
{
  columnsNameEdit->setText(realColumns[column]->getTitle());
  columnsIntervalBox->setValue(realColumns[column]->getInterval());
  alertOnNewTweets->setCurrentIndex(realColumns[column]->getAlert());
  showRetweets->setChecked(realColumns[column]->getAllowRetweets());
  columnsWidthSlider->setSliderPosition(realColumns[column]->minimumWidth());
}

void Ui_Settings::newColumn()
{
  if (!columnsNew->isChecked()) {
    columns[0]->setChecked(true);
    activeColumn = this->columns[0];
    getColumnSettings(0);
    realActiveColumn = realColumns[0];
    connect(realActiveColumn);
    newColumnTabWidget->setVisible(false);
    columnSettingsBox->setVisible(true);
  } else {
    newColumnTabWidget->setVisible(true);
    columnSettingsBox->setVisible(false);
    disconnectSlots();
    if (activeColumn != NULL)
      activeColumn->setChecked(false);
    listsCall = Global::dataStore()->getNetworkManager()->get(Global::dataStore()->getOAuth()->APICall("http://api.twitter.com/1.1/lists/list.json","GET"));
    listsView->clear();
    QObject::connect(Global::dataStore()->getNetworkManager(), SIGNAL(finished(QNetworkReply*))
                    ,this, SLOT(onRequestCompleted(QNetworkReply*)));
  }
}

void Ui_Settings::newMentionsColumnClicked()
{
  disconnectSlots();
  activeColumn->setChecked(false);
  columnsNew->setChecked(false);
  QPushButton *tmp;
  tmp = new QPushButton(this);
  QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
  sizePolicy.setHeightForWidth(tmp->sizePolicy().hasHeightForWidth());
  tmp->setSizePolicy(sizePolicy);
  tmp->setText(" ");
  tmp->setMinimumSize(QSize(30, 40));
  tmp->setCheckable(true);
  columns.push_back(tmp);
  QObject::connect(tmp, SIGNAL(clicked())
                  ,this, SLOT(clickedColumn()));
  columnsHorizontalLayout->insertWidget(columnsHorizontalLayout->count() - 1,tmp);
  realColumns.push_back(new Ui_Column("Mentions"));
  realColumns[realColumns.size() - 1]->importSettings(
    "{\"type\":1,\"interval\":30}");
  tmp->setChecked(true);
  activeColumn = tmp;
  getColumnSettings(columns.size() - 1);
  realActiveColumn = realColumns[columns.size() - 1];
  newColumnTabWidget->setVisible(false);
  columnSettingsBox->setVisible(true);
  moveColumnLeft->setVisible(true);
  moveColumnRight->setVisible(false);
  connect(realActiveColumn);
  if (columns.size() > 1)
    closeColumn->setVisible(true);
  emit goGetNewColumns();
}

void Ui_Settings::newTrendingsColumnClicked()
{
  disconnectSlots();
  activeColumn->setChecked(false);
  columnsNew->setChecked(false);
  QPushButton *tmp;
  tmp = new QPushButton(this);
  QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
  sizePolicy.setHeightForWidth(tmp->sizePolicy().hasHeightForWidth());
  tmp->setSizePolicy(sizePolicy);
  tmp->setText(" ");
  tmp->setMinimumSize(QSize(30, 40));
  tmp->setCheckable(true);
  columns.push_back(tmp);
  QObject::connect(tmp, SIGNAL(clicked())
                  ,this, SLOT(clickedColumn()));
  columnsHorizontalLayout->insertWidget(columnsHorizontalLayout->count() - 1, tmp);
  realColumns.push_back(new Ui_Column("Trending topics"));
  realColumns[realColumns.size() - 1]->importSettings(
    "{\"type\":3,\"interval\":60}");
  tmp->setChecked(true);
  activeColumn = tmp;
  getColumnSettings(columns.size() - 1);
  realActiveColumn = realColumns[columns.size() - 1];
  newColumnTabWidget->setVisible(false);
  columnSettingsBox->setVisible(true);
  moveColumnLeft->setVisible(true);
  moveColumnRight->setVisible(false);
  filterButton->setChecked(false);
  filterButton->setEnabled(false);
  filters->setVisible(false);
  alertOnNewTweets->setEnabled(false);
  showRetweets->setEnabled(false);
  connect(realActiveColumn);
  if (columns.size() > 1)
    closeColumn->setVisible(true);
  emit goGetNewColumns();
}

void Ui_Settings::newAllFriendsColumnClicked()
{
  disconnectSlots();
  activeColumn->setChecked(false);
  columnsNew->setChecked(false);
  QPushButton *tmp;
  tmp = new QPushButton(this);
  QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
  sizePolicy.setHeightForWidth(tmp->sizePolicy().hasHeightForWidth());
  tmp->setSizePolicy(sizePolicy);
  tmp->setText(" ");
  tmp->setMinimumSize(QSize(30, 40));
  tmp->setCheckable(true);
  columns.push_back(tmp);
  QObject::connect(tmp, SIGNAL(clicked())
                  ,this, SLOT(clickedColumn()));
  columnsHorizontalLayout->insertWidget(columnsHorizontalLayout->count() - 1,tmp);
  realColumns.push_back(new Ui_Column("All friends"));
  realColumns[realColumns.size() - 1]->importSettings(
    "{\"type\":0,\"interval\":30}");
  tmp->setChecked(true);
  activeColumn = tmp;
  getColumnSettings(columns.size() - 1);
  realActiveColumn = realColumns[columns.size() - 1];
  newColumnTabWidget->setVisible(false);
  columnSettingsBox->setVisible(true);
  moveColumnLeft->setVisible(true);
  moveColumnRight->setVisible(false);
  connect(realActiveColumn);
  if (columns.size() > 1)
    closeColumn->setVisible(true);
  emit goGetNewColumns();
}

void Ui_Settings::newFollowersColumnClicked()
{
  disconnectSlots();
  activeColumn->setChecked(false);
  columnsNew->setChecked(false);
  QPushButton *tmp;
  tmp = new QPushButton(this);
  QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
  sizePolicy.setHeightForWidth(tmp->sizePolicy().hasHeightForWidth());
  tmp->setSizePolicy(sizePolicy);
  tmp->setText(" ");
  tmp->setMinimumSize(QSize(30, 40));
  tmp->setCheckable(true);
  columns.push_back(tmp);
  QObject::connect(tmp, SIGNAL(clicked())
                  ,this, SLOT(clickedColumn()));
  columnsHorizontalLayout->insertWidget(columnsHorizontalLayout->count() - 1,tmp);
  realColumns.push_back(new Ui_Column("New Followers"));
  realColumns[realColumns.size() - 1]->importSettings(
    "{\"type\":4,\"interval\":30}");
  tmp->setChecked(true);
  activeColumn = tmp;
  getColumnSettings(columns.size() - 1);
  realActiveColumn = realColumns[columns.size() - 1];
  newColumnTabWidget->setVisible(false);
  columnSettingsBox->setVisible(true);
  moveColumnLeft->setVisible(true);
  moveColumnRight->setVisible(false);
  filterButton->setChecked(false);
  filterButton->setEnabled(false);
  filters->setVisible(false);
  showRetweets->setEnabled(false);
  connect(realActiveColumn);
  if (columns.size() > 1)
    closeColumn->setVisible(true);
  emit goGetNewColumns();
}

void Ui_Settings::doubleClickedList(QListWidgetItem* item)
{
  disconnectSlots();
  activeColumn->setChecked(false);
  columnsNew->setChecked(false);
  QPushButton *tmp;
  tmp = new QPushButton(this);
  QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
  sizePolicy.setHeightForWidth(tmp->sizePolicy().hasHeightForWidth());
  tmp->setSizePolicy(sizePolicy);
  tmp->setText(" ");
  tmp->setMinimumSize(QSize(30, 40));
  tmp->setCheckable(true);
  columns.push_back(tmp);
  QObject::connect(tmp, SIGNAL(clicked())
                  ,this, SLOT(clickedColumn()));
  columnsHorizontalLayout->insertWidget(columnsHorizontalLayout->count() - 1,tmp);
  realColumns.push_back(new Ui_Column(item->text()));
  realColumns[realColumns.size() - 1]->importSettings(
    "{\"type\":2,\"interval\":30,\"id\":" +
    CString(item->data(32).toDouble(),0) + "}");
  tmp->setChecked(true);
  activeColumn = tmp;
  getColumnSettings(columns.size() - 1);
  realActiveColumn = realColumns[columns.size() - 1];
  newColumnTabWidget->setVisible(false);
  columnSettingsBox->setVisible(true);
  moveColumnLeft->setVisible(true);
  moveColumnRight->setVisible(false);
  connect(realActiveColumn);
  if (columns.size() > 1)
    closeColumn->setVisible(true);
  emit goGetNewColumns();
}

void Ui_Settings::onRequestCompleted(QNetworkReply* reply)
{
  reply->deleteLater();
  if (reply == listsCall) {
    QListWidgetItem *list;
    Array a;
    istringstream input(reply->readAll().data());
    Array::parse(input, a);
    if (a.has<Object>(0)) {
      for (unsigned int i = 0; i < a.size(); i++) {
        if (a.get<Object>(i).has<string>("full_name"))
          list = new QListWidgetItem(a.get<Object>(i).get<string>("full_name").c_str());
        else
          break;
        if (a.get<Object>(i).has<double>("id"))
          list->setData(32,a.get<Object>(i).get<double>("id"));
        listsView->addItem(list);
      }
    }
  }
}

void Ui_Settings::closeColumnClicked()
{
  int number = -1;
  for (unsigned int i = 0; i < columns.size(); i++) {
    if (columns[i]->isChecked()) {
      number = i;
      break;
    }
  }
  if (number >= 0) {
    QLayoutItem* item = columnsHorizontalLayout->takeAt(number);
    delete item->widget();
    delete item;
    columns.erase(columns.begin() + number);
    delete realColumns[number];
    realColumns.erase(realColumns.begin() + number);
    emit goGetNewColumns();
    if (columns.size() > 1) {
      columns[0]->setChecked(true);
      activeColumn = columns[0];
      getColumnSettings(0);
      realActiveColumn = realColumns[0];
      connect(realActiveColumn);
    }
  }
  if (columns.size() == 1)
    closeColumn->setVisible(false);
}

void Ui_Settings::moveLeftClicked()
{
  int number = -1;
  for (unsigned int i = 0; i < columns.size(); i++) {
    if (columns[i]->isChecked()) {
      number = i;
      break;
    }
  }
  if (number >= 0) {
    realColumns.insert(realColumns.begin() + number - 1, realColumns[number]);
    realColumns.erase(realColumns.begin() + number + 1);
    columns[number]->setChecked(false);
    columns[number-1]->setChecked(true);
    activeColumn=columns[number-1];
    if ((unsigned) number - 1 == 0)
      moveColumnLeft->setVisible(false);
    else
      moveColumnLeft->setVisible(true);
    moveColumnRight->setVisible(true);
    emit goGetNewColumns();
  }
}
void Ui_Settings::moveRightClicked()
{
  int number = -1;
  for (unsigned int i = 0; i < columns.size() ; i++) {
    if (columns[i]->isChecked()) {
      number = i;
      break;
    }
  }
  if (number >= 0) {
    realColumns.insert(realColumns.begin() + number + 2, realColumns[number]);
    realColumns.erase(realColumns.begin() + number);
    columns[number]->setChecked(false);
    columns[number+1]->setChecked(true);
    activeColumn=columns[number+1];
    if ((unsigned) number + 1 == columns.size() - 1)
      moveColumnRight->setVisible(false);
    else
      moveColumnRight->setVisible(true);
    moveColumnLeft->setVisible(true);
    emit goGetNewColumns();
  }
}

void Ui_Settings::applyPassword()
{
  if (passwordBox0->text() == passwordBox1->text()) {
    if (passwordBox0->text().isEmpty())
      errorT->Error("Password is empty, so it's unset.");
    else
      errorT->Error("Password set.");
    emit setPassword(passwordBox0->text());
    if (!passwordBox0->text().isEmpty()) {
      passwordBox0->clear();
      passwordBox1->clear();
    }
  } else
    errorT->Error("Passwords aren't equal, doing nothing.");
}

void Ui_Settings::setSsl(bool newState)
{
  Global::dataStore()->getOAuth()->SetSsl(newState);
}

void Ui_Settings::setUploadingService(int newService)
{
  Global::dataStore()->setUploadingService((unsigned) newService);
}

void Ui_Settings::refreshUI()
{
  Global::Get()->refreshTheme();
}
