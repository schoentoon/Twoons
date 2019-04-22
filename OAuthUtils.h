#ifndef OAUTHUTILS_H
#define OAUTHUTILS_H

#include "CString.h"
#include "OAuthUtils.h"
#include "MD5.h"
#include "SHA256.h"
/* Qt includes */
#include <QUrl>
#include <QHash>
#include <QCryptographicHash>
/* Other includes */
#include <set>
#include <string>
#include <vector>
#include <list>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <iostream>

using namespace std;
using std::map;
using std::set;
using std::string;
using std::vector;
using std::list;
using std::pair;
using std::stringstream;

class OAuthUtils;
/** @brief OAuth utilities that are used internally. */
class OAuthUtils {
public:
  /** @brief Only required to create nonce strings
   * @param uLength Lenght of the nonce string.
   * @return A nonce string, internally required for OAuth. */
  static CString Nonce(unsigned int uLength);
  /** @brief This is only internally used to make OAuth signatures */
  static CString HMACSHA1(CString& sInput, CString& sKey);
  /** @brief Used internally to generate signatures
   * @param sHTTPMethod The HTTP Method for the request
   * @param sNormURL The URL in the request
   * @param sParams The parameters, joined with JoinParams
   * @return The actually signature */
  static CString GenerateSignature(CString sHTTPMethod, CString sNormURL, CString sParams);
  /** @brief Used internally to generate signatures
   * @param sHTTPMethod The HTTP Method for the request
   * @param sNormURL The URL in the request
   * @param Params A map with the parameters
   * @return The actually signature */
  static CString GenerateSignature(CString sHTTPMethod, CString sNormURL, std::map<CString, CString> Params);
  /** @brief Only used internally to join parameters into a string
   * @param Params A map with the parameters
   * @return String with the joined parameters */
  static CString JoinParams(std::map<CString, CString> Params);
  /** @brief Only used internally to join parameters into a string for urls
   * @param urlParams A map with the parameters to join
   * @return String with the joined parameters, ready to be used in a GET request */
  static CString JoinUrlParams(std::map<CString, CString> urlParams);
  /** @brief Only used internally to join parameters into a string for post requests
   * @param postParams A map with the parameters to join
   * @param encode Should they be encoded?
   * @return String with the joined parameters, ready to be used in a POST request */
  static CString JoinPostParams(std::map<CString, CString> postParams, bool encode = false);
  /** @brief Easy function to actually create the render
   * @param Params A map with all the parameters that have to be in the header
   * @return The actual header */
  static CString RenderHeader(std::map<CString, CString> Params);
  /** @brief Used to parse data from the received headers
   * @param input The raw headers
   * @param wantedField The field we want as output */
  static CString parseHeader(CString input, CString wantedField);
};
#endif // !OAUTHUTILS_H