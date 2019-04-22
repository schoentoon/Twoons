#include "ui_trendingtopic.h"
#include "moc_ui_trendingtopic.h"

TrendingTopic::TrendingTopic(QString topic)
{
  layout = new QGridLayout(this);
  text = new QLabel(topic);
  text->setTextFormat(Qt::RichText);
  layout->addWidget(text, 0, 0);
}

TrendingTopic::~TrendingTopic()
{
  delete layout;
  delete text;
}
