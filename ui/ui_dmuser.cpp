#include "ui_dmuser.h"
#include "moc_ui_dmuser.h"

DmUser::DmUser(Object& json, bool sender)
{
  layout = new QGridLayout(this);
  name = new QLabel(this);
  if (sender) {
    if (json.has<Object>("recipient")) {
      if (json.get<Object>("recipient").has<double>("id"))
        user_id = json.get<Object>("recipient").get<double>("id");
      if (json.get<Object>("recipient").has<string>("screen_name"))
        name->setText(json.get<Object>("recipient").get<string>("screen_name").c_str());
    }
  } else {
    if (json.has<Object>("sender")) {
      if (json.get<Object>("sender").has<double>("id"))
        user_id = json.get<Object>("sender").get<double>("id");
      if (json.get<Object>("sender").has<string>("screen_name"))
        name->setText(json.get<Object>("sender").get<string>("screen_name").c_str());
    }
  }
  QFont font;
  font.setPointSize(12);
  font.setBold(true);
  name->setFont(font);
  layout->addWidget(name, 0, 1, Qt::AlignLeft);
  counter = new QLabel(this);
  counter->setFont(font);
  layout->addWidget(counter, 0, 2, Qt::AlignRight);
  insertDM(json);
}

DmUser::DmUser(CString screen_name, unsigned int user_id)
{
  layout = new QGridLayout(this);
  name = new QLabel(this);
  this->user_id = user_id;
  name->setText(screen_name.c_str());
  QFont font;
  font.setPointSize(12);
  font.setBold(true);
  name->setFont(font);
  layout->addWidget(name, 0, 1, Qt::AlignLeft);
  counter = new QLabel(this);
  counter->setFont(font);
  counter->setText("0");
  layout->addWidget(counter, 0, 2, Qt::AlignRight);
}


double DmUser::getLastId()
{
  double highest = 0;
  for (unsigned int i = 0; i < dms.size(); i++) {
    if (dms[i].id > highest)
      highest = dms[i].id;
  }
  return highest;
}

void DmUser::focus()
{
  emit clearDMs(user_id);
  for (int i = dms.size() - 1; i >= 0; i--)
    emit sendDM(dms[i]);
}

void DmUser::mouseReleaseEvent(QMouseEvent*)
{
  emit clearDMs(user_id);
  for (int i = dms.size() - 1; i >= 0; i--)
    emit sendDM(dms[i]);
}

void DmUser::sendSince(double id)
{
  for (int i = dms.size() - 1; i >= 0; i--) {
    if (dms[i].id > id)
      emit sendDM(dms[i]);
  }
}

void DmUser::deleteDM(double id)
{
  if (dms.size() == 0)
    return;
  for (vector<DM>::iterator iter = dms.begin(); iter != dms.end(); iter++) {
    if (iter->id == id)
      dms.erase(iter);
  }
  counter->setText(CString(dms.size()).c_str());
  emit deleteDMfromView(id);
}

void DmUser::insertDM(Object& json)
{
  DM tmp;
  if (json.has<Object>("recipient")) {
    if (json.get<Object>("recipient").has<double>("id")) {
      if (json.get<Object>("recipient").get<double>("id") == user_id)
        tmp.sender=true;
      else
        tmp.sender=false;
    }
  }
  if (tmp.sender) {
    if (json.has<Object>("recipient")) {
      if (json.get<Object>("recipient").has<string>("screen_name")) {
        if (name->text() != QString(json.get<Object>("recipient").get<string>("screen_name").c_str()))
          name->setText(json.get<Object>("recipient").get<string>("screen_name").c_str());
      }
    }
  } else {
    if (json.has<Object>("sender")) {
      if (json.get<Object>("sender").has<string>("screen_name")) {
        if (name->text() != QString(json.get<Object>("sender").get<string>("screen_name").c_str()))
          name->setText(json.get<Object>("sender").get<string>("screen_name").c_str());
      }
    }
  }
  if (json.has<Object>("sender")) {
    if (json.get<Object>("sender").has<string>("profile_image_url"))
      tmp.profile_picture = json.get<Object>("sender").get<string>("profile_image_url");
  }
  if (json.has<string>("text"))
    tmp.text=json.get<string>("text").c_str();
  tmp.text.Replace("\n","");
  QString qtmp = tmp.text.c_str();
  if (json.has<Object>("entities")) {
    if (json.get<Object>("entities").has<Array>("user_mentions")) {
      Array& mentions = json.get<Object>("entities").get<Array>("user_mentions");
      for (unsigned int i = 0; i < mentions.size(); i++) {
        if (mentions.has<Object>(i)) {
          if (mentions.get<Object>(i).has<string>("screen_name") and mentions.get<Object>(i).has<double>("id"))
            qtmp.replace(QRegExp(CString("@(" + mentions.get<Object>(i).get<string>("screen_name") + ")\\b").c_str(), Qt::CaseInsensitive),
                         QString(CString("@<a href=\"http://twitter.com/" + mentions.get<Object>(i).get<string>("screen_name")
                         + "?user_id=" + CString(mentions.get<Object>(i).get<double>("id")).c_str() + "\">\\1</a>").c_str()));
        }
      }
    }
    if (json.get<Object>("entities").has<Array>("urls")) {
      Array& urls = json.get<Object>("entities").get<Array>("urls");
      for (unsigned int i = 0; i < urls.size(); i++) {
        if (urls.has<Object>(i)) {
          if (urls.get<Object>(i).has<string>("display_url") and urls.get<Object>(i).has<string>("url"))
            qtmp.replace(urls.get<Object>(i).get<string>("url").c_str(), CString("<a href=\"" + urls.get<Object>(i).get<string>("url") + "\">"
                         + urls.get<Object>(i).get<string>("display_url").c_str() + "</a>").c_str());
          else if (urls.get<Object>(i).has<string>("url"))
            qtmp.replace(urls.get<Object>(i).get<string>("url").c_str(), CString("<a href=\"" + urls.get<Object>(i).get<string>("url") + "\">"
                         + urls.get<Object>(i).get<string>("url").c_str() + "</a>").c_str());
        }
      }
    }
  }
  tmp.text=CString::ConvertUnicode(qtmp.toStdString()).Replace_n("&lt;","<").Replace_n("&gt;",">").Replace_n("&quot;","\"");
  if (json.has<double>("id"))
    tmp.id=json.get<double>("id");
  if (json.has<string>("created_at")) {
    tmp.timestamp = QDateTime::fromString(CString(json.get<string>("created_at")).Replace_n("+0000","").c_str());
    tmp.timestamp.setTimeSpec(Qt::UTC);
    tmp.timestamp = tmp.timestamp.toTimeSpec(Qt::LocalTime);
  }
  bool added = false;
  for (unsigned int i = 0; i < dms.size(); i++) {
    if (dms[i].id == tmp.id) {
      added = true;
      break;
    }
  }
  if (!added) {
    dms.push_back(tmp);
    sort(dms.begin(),dms.end(),DM());
  }
  counter->setText(CString(dms.size()).c_str());
}
