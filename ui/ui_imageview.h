#ifndef UI_IMAGEVIEW_H
#define UI_IMAGEVIEW_H

/* Qt includes */
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProgressBar>
#include <QGridLayout>
#include <QMouseEvent>
#include <QPushButton>
#include <QGroupBox>
#include <QWidget>
#include <QThread>
#include <QLabel>
#include <QUrl>

class Resizer;

using namespace std;
/** @brief Auto Complete class
 * @section description This class is used for the autocompletion of user names */
class Ui_ImageView: public QGroupBox
{
  Q_OBJECT
public:
  /** @brief Constructor */
  Ui_ImageView(QWidget *parent = 0);
  void displayLink(QUrl);
protected:
  void mousePressEvent(QMouseEvent *event) { dragStart = event->pos(); };
  void mouseMoveEvent(QMouseEvent *event) { setGeometry(QRect(geometry().topLeft() + event->pos() - dragStart, rect().size())); };
  QPoint dragStart;
  QGridLayout *layout;
  QLabel *header;
  QPushButton *closeButton;
  QLabel *image;
  QLabel *link;
  QNetworkAccessManager *network;
  QProgressBar *loader;
  Resizer *resizer;
public slots:
  void openImage(QUrl);
  void showError(QString);
  void doneLoading(QImage);
private slots:
  void finished(QNetworkReply*);
  void close();
  void progress(qint64,qint64);
};

class Resizer: public QThread
{
  Q_OBJECT
public:
  void startResizing(QPixmap pixmap);
  void run();
protected:
  QImage toResize;
signals:
  void doneLoading(QImage);
};

#endif // UI_AUTOCOMPLETE_H
