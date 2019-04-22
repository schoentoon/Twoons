#include "notification.h"
#include "moc_notification.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDebug>

#include "orgfreedesktopnotificationsinterface.h"

bool Notification::s_isInitted = false;
QString Notification::s_appName;
org::freedesktop::Notifications * Notification::s_notifications = 0;

bool Notification::init(const QString & appName)
{
  if(!QDBusConnection::sessionBus().isConnected())
    return false;

  s_notifications = new org::freedesktop::Notifications("org.freedesktop.Notifications",
                                                        "/org/freedesktop/Notifications",
                                                        QDBusConnection::sessionBus());
  s_appName = appName;
  s_isInitted = s_notifications->isValid();
  return s_isInitted;
}

void Notification::uninit()
{
  s_isInitted = false;
  delete s_notifications;
}

bool Notification::isInitted()
{
  return s_isInitted;
}

const QString & Notification::getAppName()
{
  return s_appName;
}

QStringList Notification::getServerCaps()
{
  QDBusPendingReply<QStringList> reply = s_notifications->getCapabilities();
  reply.waitForFinished();

  if(reply.isValid())
    return reply.argumentAt(0).toStringList();

  return QStringList();
}

bool Notification::getServerInfo(QString & name, QString & vendor, QString & version)
{
  QDBusPendingReply<QString, QString, QString> reply = s_notifications->getServerInformation();
  reply.waitForFinished();

  if(reply.isValid()) {
    name = reply.argumentAt(SERVER_INFO_NAME).toString();
    vendor = reply.argumentAt(SERVER_INFO_VENDOR).toString();
    version = reply.argumentAt(SERVER_INFO_VERSION).toString();
    return true;
  }
  return false;
}

Notification::Notification(const QString & summary, const QString & body, const QString & iconName, QObject * parent) :
  QObject(parent),
  m_id(0),
  m_summary(summary),
  m_body(body),
  m_iconName(iconName),
  m_timeout(5000),
  m_autoDelete(true)
{
  setUrgency(NOTIFICATION_URGENCY_NORMAL);
  QDBusConnection::sessionBus().connect(QString(), QString(), org::freedesktop::Notifications::staticInterfaceName(),
                                        "NotificationClosed", this, SLOT(onNotificationClosed(quint32, quint32)));
  QDBusConnection::sessionBus().connect(QString(), QString(), org::freedesktop::Notifications::staticInterfaceName(),
                                        "ActionInvoked", this, SLOT(onActionInvoked(quint32,QString)));
}

Notification::~Notification()
{
  disconnect(s_notifications);
}

bool Notification::show()
{
  QDBusPendingReply<quint32> reply = s_notifications->notify(getAppName(), m_id, m_iconName, m_summary, m_body
                                                            ,m_actions, m_hints, m_timeout);
  if (m_id == 0) {
    reply.waitForFinished();
    if (!reply.isValid())
      return false;
    m_id = reply.argumentAt(0).toInt();
  }
  return true;
}

void Notification::setSummary(const QString & summary)
{
  m_summary = summary;
}

void Notification::setBody(const QString & body)
{
  m_body = body;
}

void Notification::setIconName(const QString & iconName)
{
  m_iconName = iconName;
}

void Notification::setTimeout(qint32 timeout)
{
  m_timeout = timeout;
}

void Notification::setUrgency(NotificationUrgency urgency)
{
  setHintByte("urgency", urgency);
}

void Notification::setCategory(const QString & category)
{
  setHintString("category", category);
}

void Notification::setLocation(qint32 x, qint32 y)
{
  setHintInt32("x", x);
  setHintInt32("y", y);
}

void Notification::setHint(const QString & key, const QVariant & value)
{
  m_hints.insert(key, value);
}

void Notification::setHintInt32(const QString & key, qint32 value)
{
  m_hints.insert(key, value);
}

void Notification::setHintDouble(const QString & key, double value)
{
  m_hints.insert(key, value);
}

void Notification::setHintString(const QString & key, const QString & value)
{
  m_hints.insert(key, value);
}

void Notification::setHintByte(const QString & key, char value)
{
  setHintByteArray(key, QByteArray(1, value));
}

void Notification::setHintByteArray(const QString & key, const QByteArray & value)
{
  m_hints.insert(key, value);
}

void Notification::clearHints()
{
  m_hints.clear();
}

void Notification::addAction(const QString & actionKey, const QString & label)
{
  m_actions << actionKey << label;
}

void Notification::clearActions()
{
  m_actions.clear();
}

bool Notification::close()
{
  QDBusPendingReply<> reply = s_notifications->closeNotification(m_id);
  reply.waitForFinished();
  return reply.isValid();
}

bool Notification::autoDelete() const
{
  return m_autoDelete;
}

void Notification::setAutoDelete(bool autoDelete)
{
  m_autoDelete = autoDelete;
}

void Notification::onNotificationClosed(quint32 id, quint32 reason)
{
  if (m_id == id) {
    emit closed(reason);
    if (m_autoDelete)
      deleteLater();
  }
}

void Notification::onActionInvoked(quint32 id, const QString & actionKey)
{
  if (m_id == id)
    emit actionInvoked(actionKey);
}