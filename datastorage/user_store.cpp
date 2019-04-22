#include "user_store.h"

StoreUser::StoreUser(TwUser* user)
{
  id = user->getId();
  screen_name = user->getName();
}