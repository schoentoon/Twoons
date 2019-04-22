#include "ui_errorbox.h"
#include "moc_ui_errorbox.h"

errorBoxT::errorBoxT()
{
  timer = new QTimer();
  timer->setInterval(5000);
  QObject::connect(timer, SIGNAL(timeout())
                  ,this, SLOT(clear()));
}

void errorBoxT::Error(QString tmp)
{
  emit getError(tmp);
  timer->setSingleShot(true);
  timer->start();
}

void errorBoxT::clear()
{
  emit getError("");
}