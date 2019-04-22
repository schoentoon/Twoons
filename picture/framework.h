#ifndef PICTURE_FRAMEWORK_H
#define PICTURE_FRAMEWORK_H

#include "ui_imageview.h"
/* Qt includes */
#include <QUrl>
#include <QObject>

class PictureFramework : public QObject
{
  Q_OBJECT
public:
  /** @brief This will be used to check the url
   * You should check if it's from the right site
   * over here.
   */
  virtual bool checkLink(QUrl);
  /** @brief This will be called if checkLink(QUrl)
   * returned true, you will have to end by emitting
   * openPicture(QUrl) in here.
   */
  virtual void openImage(QUrl);
  static vector<PictureFramework*> init_pictures(Ui_ImageView*);
signals:
  /** @brief This signal is used to return the url
   * to the actual picture.
   */
  void openPicture(QUrl);
  /** @brief This signal is used to show an error
   * to the user
   */
  void showError(QString);
};

#endif // PICTURE_FRAMEWORK_H
