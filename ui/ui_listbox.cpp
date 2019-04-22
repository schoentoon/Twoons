#include "ui_listbox.h"
#include "moc_ui_listbox.h"

ListBox::ListBox(TwList list)
{
  setObjectName("list");
  pList = list;
  QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
  setSizePolicy(sizePolicy);
  setMinimumSize(QSize(250, 50));
  setMaximumSize(QSize(250, 9001));
  QFont font;
  font.setPointSize(8);
  setFont(font);
  setTitle(CString(list.getName() + " by " + list.getUser()->getName()).c_str());
  verticalLayout = new QVBoxLayout(this);
  verticalLayout->setContentsMargins(-1, -1, 19, -1);

  description = new QLabel(this);
  font.setPointSize(10);
  description->setFont(font);
  description->setText(list.getDescription(true).c_str());
  description->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);
  description->setTextFormat(Qt::RichText);
  description->setWordWrap(true);
  if (!description->text().isEmpty())
    verticalLayout->addWidget(description);

  extraText = new QLabel(this);
  CString tmp = "Subsscribers:";
  tmp = tmp + CString(list.getSubscribers()) + ", members: ";
  tmp = tmp + CString(list.getMembers());
  extraText->setText(tmp.c_str());
  extraText->setTextFormat(Qt::RichText);
  extraText->setMaximumSize(QSize(250, 16));
  verticalLayout->addWidget(extraText);
}
