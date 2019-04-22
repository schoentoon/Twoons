#ifndef PICTUREUP_FRAMEWORK_H
#define PICTUREUP_FRAMEWORK_H

#include <vector>

class TweetField;

/* Qt includes */
#include <QUrl>
#include <QFile>
#include <QObject>
#include <QFileInfo>
#include <QNetworkReply>

using namespace std;

class PictureUpFramework : public QObject
{
  Q_OBJECT
public:
  PictureUpFramework();
  virtual void postUpload(QNetworkReply *reply);
  virtual QString url();
  virtual QNetworkReply* upload(QFile *file);
  virtual unsigned int getUniqueId();
  virtual QString getName();
  static vector<PictureUpFramework*> init(TweetField*);
private slots:
  void finished(QNetworkReply *reply);
signals:
  void sendUrl(QString);
  /** @brief This signal is used to show an error
   * to the user
   */
  void showError(QString);
protected:
  QNetworkAccessManager *network;
};

#endif // PICTUREUP_FRAMEWORK_H
