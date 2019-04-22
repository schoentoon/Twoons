#include "OAuth.h"

OAuth::OAuth()
{
  ssl = false;
}

OAuth::OAuth(CString access_key, CString access_secret)
{
  aKey = access_key;
  aSec = access_secret;
  ssl = false;
}

void OAuth::SetKeys(CString access_key, CString access_secret)
{
  aKey = access_key;
  aSec = access_secret;
}

CString OAuth::CheckUrl(CString sUrl)
{
  if (!sUrl.Left(5).AsLower().Equals("https"))
    return "https" + sUrl.LeftChomp_n(4);
  return sUrl;
}

QNetworkRequest OAuth::APICall(CString sUrl, CString method)
{
  map<CString,CString> meh;
  return APICall(sUrl, meh, method);
}

QNetworkRequest OAuth::APICall(CString sUrl, map<CString, CString> urlParams, CString method)
{
  sUrl = CheckUrl(sUrl);
  QNetworkRequest request = QNetworkRequest();
  QUrl url = QUrl(sUrl.c_str());
  map<CString, CString> Params;
  Params["oauth_consumer_key"] = CONSUMER_KEY;
  Params["oauth_nonce"] = OAuthUtils::Nonce(8);
  Params["oauth_signature_method"] = "HMAC-SHA1";
  Params["oauth_timestamp"] = CString(time(NULL));
  Params["oauth_token"] = aKey;
  Params["oauth_version"] = "1.0";
  for(map<CString,CString>::const_iterator iter = urlParams.begin(); iter != urlParams.end(); ++iter) {
    urlParams[iter->first] = QUrl::toPercentEncoding(iter->second.c_str()).data();
    Params[iter->first] = iter->second;
    if (method.Equals("GET"))
      url.addEncodedQueryItem(iter->first.c_str(), iter->second.c_str());
  }
  CString sParams = OAuthUtils::JoinParams(Params);
  CString raw = OAuthUtils::GenerateSignature(method, sUrl, sParams);
  DEBUG(raw);
  CString key = CString(CONSUMER_SECRET) + "&" + CString(aSec);
  Params["oauth_signature"] = QUrl::toPercentEncoding(OAuthUtils::HMACSHA1(raw, key).c_str()).data();
  DEBUG(url.toString().toStdString());
  request.setUrl(url);
  for (map<CString,CString>::iterator iter = Params.begin(); iter != Params.end(); ++iter) {
    if (!iter->first.Left(6).Equals("oauth_")) /* When streaming don't send the parameters in the headers */
      Params.erase(iter); /* So we take them out here before we generate the final header */
  } /* Turns out this is the correct way of doing OAuth in all cases.. */
  request.setRawHeader("Authorization", OAuthUtils::RenderHeader(Params).c_str());
  request.setRawHeader("User-Agent", "Twoons for " OS " ~ " VERSION);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
  return request;
}

map<CString,CString> OAuth::Echo(CString sUrl)
{
  map<CString,CString> params;
  params["oauth_consumer_key"] = CONSUMER_KEY;
  params["oauth_nonce"] = OAuthUtils::Nonce(8);
  params["oauth_signature_method"] = "HMAC-SHA1";
  params["oauth_timestamp"] = CString(time(NULL));
  params["oauth_token"] = aKey;
  params["oauth_version"] = "1.0";
  CString sParams = OAuthUtils::JoinParams(params);
  CString raw = OAuthUtils::GenerateSignature("GET", sUrl, sParams);
  CString key = CString(CONSUMER_SECRET) + "&" + CString(aSec);
  params["oauth_signature"] = QUrl::toPercentEncoding(OAuthUtils::HMACSHA1(raw, key).c_str()).data();
  return params;
}

QNetworkRequest OAuth::Echo(map<CString,CString> params, CString realm, CString credentials_header)
{
  QNetworkRequest request;
  params["realm"] = realm;
  request.setRawHeader(credentials_header.c_str(), OAuthUtils::RenderHeader(params).c_str());
  request.setRawHeader("User-Agent", "Twoons for " OS " ~ " VERSION);
  return request;
}

bool OAuth::isEmpty()
{
  if (aKey.size() < 1 or aSec.size() < 1)
    return true;
  return false;
}
