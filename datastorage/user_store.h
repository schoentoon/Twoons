#ifndef USER_STORE_H
#define USER_STORE_H

#include <iostream>
#include "CString.h"
#include "user.h"
/* Qt includes */
#include <QIcon>
#include <QObject>

using namespace std;
/** @brief StoreUser class
 * @section description Used to store users inside the datastorage */
class StoreUser
{
public:
  StoreUser(TwUser* user);
  unsigned long getId() { return id; };
  CString getName() { return screen_name; };
protected:
  unsigned long id;
  CString screen_name;
};

#endif // DATASTORAGE_H