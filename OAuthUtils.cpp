#include "OAuthUtils.h"

CString OAuthUtils::Nonce(unsigned int uLength = 8) {
  CString sTmp = CString(CString(qrand()) + "/" + CString(rand()));
  return sTmp.MD5().Right(uLength);
}

CString OAuthUtils::HMACSHA1(CString& sInput, CString& sKey)
{
  QString key = sInput.c_str();
  QString secret = sKey.c_str();
  QByteArray K;
  int   K_lenght;
  K_lenght = secret.size();
  K = secret.toAscii();
  QByteArray ipad;
  QByteArray opad;
  if(K_lenght > 64)
  {
    QByteArray tempSecret;
    tempSecret.append(secret);
    K = QCryptographicHash::hash(tempSecret,
                                QCryptographicHash::Sha1);
    K_lenght = 20;
  }
  ipad.fill( 0, 64);
  opad.fill(0, 64);
  ipad.replace(0, K_lenght, K);
  opad.replace(0, K_lenght, K);
  for (int i=0; i<64; i++)
  {
    ipad[i] = ipad[i] ^ 0x36;
    opad[i] = opad[i] ^ 0x5c;
  }
  QByteArray context;
  context.append(ipad,64);
  context.append(key);
  QByteArray Sha1   = QCryptographicHash::hash(context,
                                              QCryptographicHash::Sha1);
  context.clear();
  context.append(opad,64);
  context.append(Sha1);
  Sha1.clear();
  Sha1 = QCryptographicHash::hash(context,
                                  QCryptographicHash::Sha1);
  QByteArray str(Sha1.toBase64());
  return QString(str).toLatin1().data();
}

CString OAuthUtils::GenerateSignature(CString sHTTPMethod, CString sNormURL, CString sParams)
{
  return CString(sHTTPMethod + "&" + sNormURL.Escape_n(CString::EURL).Replace_n("%3F", "&") + "&" + sParams.Escape_n(CString::EURL));
}

CString OAuthUtils::GenerateSignature(CString sHTTPMethod, CString sNormURL, std::map<CString, CString> Params)
{
  return CString(sHTTPMethod + "&" + sNormURL.Escape_n(CString::EURL).Replace_n("%3F", "&") + "&" + OAuthUtils::JoinParams(Params).Escape_n(CString::EURL));
}

CString OAuthUtils::JoinParams(map<CString, CString> Params)
{
  CString sTmp;
  for(map<CString,CString>::const_iterator iter = Params.begin(); iter != Params.end(); ++iter)
    sTmp = sTmp + CString(iter->first + "=" + iter->second + "&");
  return sTmp.RightChomp_n(1);
}

CString OAuthUtils::RenderHeader(map<CString, CString> Params)
{
  CString sTmp = "OAuth ";
  for(map<CString,CString>::const_iterator iter = Params.begin(); iter != Params.end(); ++iter)
    sTmp = sTmp + CString(iter->first + "=\"" + iter->second + "\",");
  return sTmp.RightChomp_n(1);
}

CString OAuthUtils::JoinUrlParams(map<CString, CString> urlParams)
{
  CString sTmp = "?";
  for(map<CString,CString>::const_iterator iter = urlParams.begin(); iter != urlParams.end(); ++iter)
    sTmp = sTmp + CString(iter->first + "=" + iter->second + "&");
  return sTmp.RightChomp_n(1);
}

CString OAuthUtils::JoinPostParams(map<CString, CString> postParams, bool encode)
{
  CString sTmp;
  for(map<CString,CString>::const_iterator iter = postParams.begin(); iter != postParams.end(); ++iter) {
    if (encode)
      sTmp += CString(iter->first + "=" + QUrl::toPercentEncoding(iter->second.c_str()).data() + "&");
    else
      sTmp += CString(iter->first + "=" + iter->second + "&");
  }
  return sTmp.RightChomp_n(1);
}

CString OAuthUtils::parseHeader(CString input, CString wantedField)
{
  VCString tmp;
  input.Split("\n",tmp,false);
  if (tmp.size() > 0) {
    for (unsigned int i = 0; i < tmp.size() - 1; i++) {
      if (tmp[i].Token(0,false,":").Equals(wantedField))
        return tmp[i].Token(1,true,":").Trim_n();
    }
  }
  return ""; /* If you ToUInt() this output you'll get 0 */
}
