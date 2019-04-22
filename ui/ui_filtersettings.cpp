#include "ui_filtersettings.h"
#include "moc_ui_filtersettings.h"

FilterSettings::FilterSettings(QWidget *)
{
  setMovable(true);
  Username = new QWidget();
  userLayout = new QGridLayout(Username);
  userTextField = new QTextEdit(Username);
  userLayout->addWidget(userTextField, 0, 0, 0, 0);
  userTextField->setToolTip("Each line represents one rule.\n"
                            "It'll only match and filter full usernames.\n"
                            "These filter rules support * as wildcard.");
  QObject::connect(userTextField, SIGNAL(textChanged())
                  ,this, SLOT(sendUserFilter()));
  addTab(Username, "Username");

  Source = new QWidget();
  sourceLayout = new QGridLayout(Source);
  sourceTextField = new QTextEdit(Source);
  sourceLayout->addWidget(sourceTextField, 0, 0, 0, 0);
  sourceTextField->setToolTip("Each line represents one rule.\n"
                              "It'll only match and filter full sources.\n"
                              "These filter rules support * as wildcard.");
  QObject::connect(sourceTextField, SIGNAL(textChanged())
                  ,this, SLOT(sendSourceFilter()));
  addTab(Source, "Source");

  Text = new QWidget();
  textLayout = new QGridLayout(Text);
  textTextField = new QTextEdit(Text);
  textLayout->addWidget(textTextField, 0, 0, 0, 0);
  textTextField->setToolTip("Each line represents one rule.\n"
                            "It'll only match if the word is in the tweet.\n"
                            "These filter rules support * as wildcard.");
  QObject::connect(textTextField, SIGNAL(textChanged())
                  ,this, SLOT(sendTextFilter()));
  addTab(Text, "Text");
}

void FilterSettings::clear()
{
  userTextField->clear();
  sourceTextField->clear();
  textTextField->clear();
}

void FilterSettings::setField(CString field, VCString info)
{
  CString tmp;
  if (field.Equals("user")) {
    for (VCString::size_type i = 0; i < info.size(); i++)
      tmp = tmp + info[i] + "\n";
    userTextField->setText(tmp.c_str());
  }
  if (field.Equals("source")) {
    for (VCString::size_type i = 0; i < info.size(); i++)
      tmp = tmp + info[i] + "\n";
    sourceTextField->setText(tmp.c_str());
  }
  if (field.Equals("text")) {
    for (VCString::size_type i = 0; i < info.size(); i++)
      tmp = tmp + info[i] + "\n";
    textTextField->setText(tmp.c_str());
  }
}

void FilterSettings::sendUserFilter()
{
  VCString tmp;
  CString(userTextField->toPlainText().toLatin1().data()).Split("\n", tmp, false);
  emit sendFilters("user", tmp);
}

void FilterSettings::sendSourceFilter()
{
  VCString tmp;
  CString(sourceTextField->toPlainText().toLatin1().data()).Split("\n", tmp, false);
  emit sendFilters("source", tmp);
}

void FilterSettings::sendTextFilter()
{
  VCString tmp;
  CString(textTextField->toPlainText().toLatin1().data()).Split("\n", tmp, false);
  emit sendFilters("text", tmp);
}
