#include "ui_autocomplete.h"
#include "moc_ui_autocomplete.h"
#include "global.h"

Ui_Autocomplete::Ui_Autocomplete()
{
  setMinimumSize(QSize(200, 200));
  setVisible(false);
  setAutoFillBackground(true);
  layout = new QGridLayout(this);

  header = new QLabel("Autocomplete",this);
  layout->addWidget(header, 0, 0, Qt::AlignTop);

  closeButton = new QPushButton("X", this);
  layout->addWidget(closeButton, 0, 0, Qt::AlignTop|Qt::AlignRight);
  QObject::connect(closeButton, SIGNAL(clicked())
                  ,this, SLOT(hide()));

  inputBox = new QLineEdit(this);
  inputBox->installEventFilter(this);
  layout->addWidget(inputBox, 1, 0, Qt::AlignTop);
  QObject::connect(inputBox, SIGNAL(textChanged(QString))
                  ,this, SLOT(query(QString)));

  listWidget = new QListWidget(this);
  listWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
  listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
  layout->addWidget(listWidget, 2, 0);
  QObject::connect(listWidget, SIGNAL(activated(QModelIndex))
                  ,this, SLOT(activatedItem(QModelIndex)));
}

void Ui_Autocomplete::appear()
{
  setVisible(true);
  inputBox->clear();
  query("");
  listWidget->scrollToTop();
  inputBox->setFocus();
}

void Ui_Autocomplete::query(QString text)
{
  listWidget->clear();
  VCString tmp = Global::dataStore()->queryUserDB(text.toLatin1().data());
  for (unsigned int i = 0; i < tmp.size(); i++) {
    QListWidgetItem *__qlistwidgetitem = new QListWidgetItem(listWidget);
    __qlistwidgetitem->setText(tmp[i].c_str());
  }
}

void Ui_Autocomplete::activatedItem(QModelIndex tmp)
{
  QAbstractItemModel* model = listWidget->model();
  emit sendNickname(model->index(tmp.row(),0).data(Qt::DisplayRole).toString().toLatin1().data());
  setVisible(false);
}

bool Ui_Autocomplete::eventFilter(QObject *, QEvent * event)
{
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent* eventKey = static_cast<QKeyEvent*>(event);
    int key = eventKey->key();
    switch (key) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
      if (listWidget->selectedItems().count() > 0)
        emit sendNickname(listWidget->currentItem()->data(Qt::DisplayRole).toString().toLatin1().data());
      else
        emit sendNickname(inputBox->text().toLatin1().data());
      setVisible(false);
      return true;
    case Qt::Key_Escape:
      emit focus();
      setVisible(false);
      return true;
    case Qt::Key_Tab:
      if (listWidget->count() > 0 and listWidget->selectedItems().isEmpty()) {
        listWidget->setCurrentRow(0);
        return true;
      }
      return false;
    }
    if (listWidget->selectedItems().count() > 0) {
      switch (key) {
      case Qt::Key_Home:
        if (listWidget->count() > 0)
          listWidget->setCurrentRow(0);
        return true;
      case Qt::Key_End:
        if (listWidget->count() > 0)
          listWidget->setCurrentRow(listWidget->count() - 1);
        return true;
      case Qt::Key_Down:
      case Qt::Key_PageDown:
      case Qt::Key_S:
      case Qt::Key_K:
        if (listWidget->currentRow() + 1 < listWidget->count())
          listWidget->setCurrentRow(listWidget->currentRow() + 1);
        return true;
      case Qt::Key_Up:
      case Qt::Key_PageUp:
      case Qt::Key_W:
      case Qt::Key_J:
        if (listWidget->currentRow() > 0)
          listWidget->setCurrentRow(listWidget->currentRow() - 1);
        return true;
      }
    }
  }
  return false;
}