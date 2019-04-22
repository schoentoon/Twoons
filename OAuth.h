#ifndef OAUTH_H
#define OAUTH_H

class OAuth; /* Else it refuses to compiles */
class StreamThread;

#include <CString.h>
#include <string.h>
#include "OAuthUtils.h"
#include <sstream>
#include "datastorage.h"
#include <iostream>
#include <time.h>
#include "debug.h"
/* Qt includes */
#include <QNetworkRequest>
#include <QtGlobal>
#include <QUrl>

#ifndef CONSUMER_KEY
#error "You should go and define a CONSUMER_KEY in OAuth.h"
#endif
#ifndef CONSUMER_SECRET
#error "You should go and define a CONSUMER_SECRET in OAuth.h"
#endif

using namespace std;

/** @brief OAuth class */
class OAuth {
public:
  /** @brief Constructor
   * @return OAuth object without any keys */
  OAuth();
  /** @brief Constructor
   * @param access_key The access key to use
   * @param access_secret The access secret key to use
   * @return OAuth object with these keys set */
  OAuth(CString access_key, CString access_secret);
  /** @brief Set keys in case of an empty init
   * @param access_key The access key to use
   * @param access_secret The access secret key to use */
  void SetKeys(CString access_key, CString access_secret);
  /** @brief Used to toggle the ssl switch,
   * if ssl is set to true all the request will be made with ssl */
  void SetSsl(bool newState) { ssl = newState; };
  /** @brief Used to toggle streaming */
  void setStream(bool newState) { stream = newState; };
  /** @brief Used to make api calls
   * @param sUrl The URL called
   * @param method The method to use, usually GET or POST
   * @return A pointer to the QNetworkRequest object */
  QNetworkRequest APICall(CString sUrl, CString method = "GET");
  QNetworkRequest Echo(map<CString,CString> params, CString realm, CString credentials_header);
  map<CString,CString> Echo(CString sUrl);
  /** @brief Used to make api calls with parameters
   * @param sUrl The URL called
   * @param urlParams A map with the parameters
   * @param method The method to use, usually GET or POST
   * @return A pointer to the QNetworkRequest object */
  QNetworkRequest APICall(CString sUrl, map<CString, CString> urlParams, CString method = "GET");
  void setStreamThread(StreamThread* thread) { stream_thread = thread; };
  StreamThread* getStream() { return stream_thread; };
  /** @brief Used to check if it has emtpy keys */
  bool isEmpty();
  /** @brief Returns the user id of the user the keys are for */
  unsigned int getUserId() { return aKey.Token(0,false,"-").ToUInt(); };
  /** @brief Used to see if https only is enabled or disabled */
  bool isSsl() { return ssl; };
  /** @brief Used to check if we're allowed to stream or not */
  bool isStreaming() { return stream; };
protected:
  /** @brief If set to true all the requests will be done with ssl */
  bool          ssl;
  /** @brief This holds if we're streaming or not */
  bool          stream;
  /** @brief Save a pointer to the stream thread over here */
  StreamThread  *stream_thread;
private:
  /** @brief Used internally to make sure the url is https if ssl is toggled on
   * @param sUrl The base url which is checked
   * @return The url that will be called */
  CString CheckUrl(CString sUrl);
  /** @brief The required access key, these can only be set during init */
  CString       aKey;
  /** @brief The required access secret key, these can only be set during init */
  CString       aSec;
};

#endif // !OAUTH_H
