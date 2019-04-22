#include "ui_dmview.h"
#include "moc_ui_dmview.h"
#include "global.h"

DmView::DmView(DM dm)
{
  id = dm.id;
  setObjectName("DmView");
  setContentsMargins(5, 5, 5, 5);
  layout = new QGridLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  text = new QTextBrowser(this);
  text->setContentsMargins(0, 0, 0, 0);
  text->setAcceptRichText(true);
  text->setText(dm.text.c_str());
  text->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  text->setFrameShape(QTextEdit::NoFrame);
  text->setFrameShadow(QTextEdit::Plain);
  text->viewport()->setAutoFillBackground(false);
  text->setReadOnly(true);
  text->setOpenLinks(false);
  text->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);
  QObject::connect(text, SIGNAL(anchorClicked(QUrl))
                  ,this, SLOT(urlClicked(QUrl)));
  QObject::connect(text->document()->documentLayout(), SIGNAL(documentSizeChanged(QSizeF))
                  ,this, SLOT(adjustTextField(QSizeF)));
  if (dm.sender)
    layout->addWidget(text, 0, 0);
  else
    layout->addWidget(text, 0, 1);

  timestamp = new QLabel(this);
  timestamp->setText("<span style=\"font-size:7pt\">" + dm.timestamp.toString("hh:mm:ss") + "</span>");
  if (dm.sender)
    layout->addWidget(timestamp, 1, 0);
  else
    layout->addWidget(timestamp, 1, 1);

  profilePicture = new QLabel(this);
  if (dm.sender)
    layout->addWidget(profilePicture, 0, 1, 2, 1);
  else
    layout->addWidget(profilePicture, 0, 0, 2, 1);
  QPixmap pixmap;
  if (QPixmapCache::find(dm.profile_picture.c_str(), &pixmap)) {
    profilePicture->setPixmap(pixmap);
    profilePicture->setMaximumSize(pixmap.width(), pixmap.height());
    if (height() < profilePicture->pixmap()->height())
      setMinimumHeight(profilePicture->pixmap()->height() + 16);
  } else {
    QObject::connect(Global::dataStore(), SIGNAL(sendProfilePicture(CString,QPixmap))
                    ,this, SLOT(getProfilePicture(CString,QPixmap)));
    Global::dataStore()->getProfilePicture(dm.profile_picture);
    url = dm.profile_picture;
  }
}

DmView::~DmView()
{
  delete layout;
  delete text;
  delete timestamp;
  delete profilePicture;
}

void DmView::getProfilePicture(CString url, QPixmap pixmap)
{
  if (profilePicture->pixmap() != 0)
    return;
  if (this->url.Equals(url)) {
    profilePicture->setPixmap(pixmap);
    profilePicture->setMaximumSize(pixmap.width(), pixmap.height());
    if (profilePicture->pixmap() != 0) {
      if (height() < profilePicture->pixmap()->height())
        setMinimumHeight(profilePicture->pixmap()->height() + 16);
    }
  }
}

void DmView::adjustTextField(QSizeF size)
{
  text->setMinimumHeight(size.height() + 2 * text->frameWidth());
  setMinimumHeight(size.height() + 16);
  if (profilePicture->pixmap() != 0) {
    if (height() < profilePicture->pixmap()->height())
      setMinimumHeight(profilePicture->pixmap()->height() + 16);
  }
}

void DmView::urlClicked(QUrl url)
{
  QDesktopServices::openUrl(url);
}
