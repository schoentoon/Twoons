#include "ui_imageview.h"
#include "moc_ui_imageview.h"
#include "global.h"

Ui_ImageView::Ui_ImageView(QWidget *parent)
: QGroupBox(parent)
{
  setVisible(false);
  setAutoFillBackground(true);
  resizer = new Resizer();
  layout = new QGridLayout(this);

  header = new QLabel("ImageView",this);
  layout->addWidget(header, 0, 0, Qt::AlignTop);

  closeButton = new QPushButton("X",this);
  layout->addWidget(closeButton, 0, 0, 2, 0, Qt::AlignTop|Qt::AlignRight);
  QObject::connect(closeButton, SIGNAL(clicked())
                  ,this, SLOT(close()));

  link = new QLabel(this);
  link->setOpenExternalLinks(true);
  link->setTextFormat(Qt::RichText);
  layout->addWidget(link, 1, 0);

  loader = new QProgressBar(this);
  layout->addWidget(loader, 2, 0);

  image = new QLabel(this);
  layout->addWidget(image, 2, 0);
  network = new QNetworkAccessManager();
  QObject::connect(network, SIGNAL(finished(QNetworkReply*))
                  ,this, SLOT(finished(QNetworkReply*)));
  QObject::connect(network, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>))
                  ,Global::dataStore(), SLOT(onIgnoreSSLErrors(QNetworkReply*,QList<QSslError>)));
  QObject::connect(resizer, SIGNAL(doneLoading(QImage))
                  ,this, SLOT(doneLoading(QImage)));
}

void Ui_ImageView::openImage(QUrl url)
{
  QNetworkRequest tmp = QNetworkRequest(url);
  tmp.setRawHeader("User-Agent", "Twoons for " OS " ~ " VERSION);
  QNetworkReply *request = network->get(tmp);
  QObject::connect(request, SIGNAL(downloadProgress(qint64,qint64))
                  ,this, SLOT(progress(qint64,qint64)));
  setVisible(true);
  loader->setVisible(true);
}

void Ui_ImageView::displayLink(QUrl url)
{
  link->setText("<a href=\"" + url.toString() + "\">Open in browser</a>");
}

void Ui_ImageView::progress(qint64 incomingBytes, qint64 totalBytes)
{
  loader->setMaximum(totalBytes);
  loader->setValue(incomingBytes);
}

void Ui_ImageView::close()
{
  image->setPixmap(QPixmap());
  image->clear();
  link->clear();
  setVisible(false);
}

void Ui_ImageView::finished(QNetworkReply *reply)
{
  reply->deleteLater();
  QVariant possibleRedirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
  QUrl _urlRedirectedTo = possibleRedirectUrl.toUrl();
  if (!_urlRedirectedTo.isEmpty()) {
    openImage(_urlRedirectedTo);
  } else if (reply->error() == QNetworkReply::NoError) {
    if (isVisible()) {
      QPixmap pixmap;
      if (pixmap.loadFromData(reply->readAll())) {
        if (!pixmap.isNull()) {
          if (pixmap.height() > (Global::Get()->height()*0.8) or pixmap.width() > (Global::Get()->width()*0.8))
            resizer->startResizing(pixmap);
          else {
            image->setPixmap(pixmap);
            loader->setVisible(false);
          }
        }
      } else {
        if (image->pixmap() == 0) {
          image->setText("Failed to load the image :(");
          loader->setVisible(false);
        }
      }
    }
  } else {
    if (isVisible()) {
      loader->setVisible(false);
      image->setText("Failed to load image :(");
    }
  }
}

void Ui_ImageView::doneLoading(QImage inputImage)
{
  image->setPixmap(QPixmap::fromImage(inputImage));
  loader->setVisible(false);
}

void Ui_ImageView::showError(QString error)
{
  image->setText(error);
  loader->setVisible(false);
}

void Resizer::startResizing(QPixmap pixmap)
{
  if (isRunning())
    exit();
  toResize = pixmap.toImage();
  start();
}

void Resizer::run()
{
  QImage scaled = toResize.scaled((int) Global::Get()->width()*0.8, (int) Global::Get()->height()*0.8, Qt::KeepAspectRatio);
  emit doneLoading(scaled);
  exit();
}