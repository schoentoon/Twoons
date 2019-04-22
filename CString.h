#ifndef CSTRING_H
#define CSTRING_H

#include <map>
#include <set>
#include <string>
#include <vector>
#include <list>
#include <sys/types.h>
#include <iostream>
#include <stdexcept>

using std::map;
using std::set;
using std::string;
using std::vector;
using std::list;
using std::pair;

#define _SQL(s) CString("'" + CString(s).Escape_n(CString::ESQL) + "'")
#define _URL(s) CString(s).Escape_n(CString::EURL)
#define _HTML(s) CString(s).Escape_n(CString::EHTML)

class CString;

typedef set<CString> SCString;
typedef vector<CString>                 VCString;
typedef vector<pair<CString, CString> > VPair;

static const unsigned char XX = 0xff;
static const unsigned char base64_table[256] = {
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,62, XX,XX,XX,63,
	52,53,54,55, 56,57,58,59, 60,61,XX,XX, XX,XX,XX,XX,
	XX, 0, 1, 2,  3, 4, 5, 6,  7, 8, 9,10, 11,12,13,14,
	15,16,17,18, 19,20,21,22, 23,24,25,XX, XX,XX,XX,XX,
	XX,26,27,28, 29,30,31,32, 33,34,35,36, 37,38,39,40,
	41,42,43,44, 45,46,47,48, 49,50,51,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
	XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX, XX,XX,XX,XX,
};

/**
 * @brief String class
 */
class CString : public string {
public:
  typedef enum {
          EASCII,
          EURL,
          EHTML,
          ESQL
  } EEscape;
  explicit CString(bool b) : string(b ? "true" : "false") {}
  explicit CString(char c);
  explicit CString(unsigned char c);
  explicit CString(short i);
  explicit CString(unsigned short i);
  explicit CString(int i);
  explicit CString(unsigned int i);
  explicit CString(long i);
  explicit CString(unsigned long i);
  explicit CString(long long i);
  explicit CString(unsigned long long i);
  explicit CString(double i, int precision = 2);
  explicit CString(float i, int precision = 2);
  CString() : string() {}
  CString(const char* c) : string(c) {}
  CString(const char* c, size_t l) : string(c, l) {}
  CString(const string& s) : string(s) {}
  ~CString() {}
  /**
    * Compare this string caselessly to some other string.
    * @param s The string to compare to.
    * @param uLen The number of characters to compare.
    * @return An integer less than, equal to, or greater than zero if this
    *         string smaller, equal.... to the given string.
    */
  int CaseCmp(const CString& s, unsigned long uLen = CString::npos) const;
  /**
    * Compare this string case sensitively to some other string.
    * @param s The string to compare to.
    * @param uLen The number of characters to compare.
    * @return An integer less than, equal to, or greater than zero if this
    *         string smaller, equal.... to the given string.
    */
  int StrCmp(const CString& s, unsigned long uLen = CString::npos) const;
  /**
    * Check if this string is equal to some other string.
    * @param s The string to compare to.
    * @param bCaseSensitive True if you want the comparision to be case
    *                       sensitive.
    * @param uLen Number of characters to consider.
    * @return True if the strings are equal.
    */
  bool Equals(const CString& s, bool bCaseSensitive = false, unsigned long uLen = CString::npos) const;
  /**
    * Do a wildcard comparision between two strings.
    * For example, the following returns true:
    * <code>WildCmp("*!?bar@foo", "I_am!~bar@foo");</code>
    * @param sWild The wildcards used for the comparison.
    * @param sString The string that is used for comparing.
    * @return true if the wildcard matches.
    */
  static bool WildCmp(const CString& sWild, const CString& sString);
  /**
    * Do a wild compare on this string.
    * @param sWild The wildcards used to for the comparison.
    * @return The result of <code>this->WildCmp(sWild, *this);</code>.
    */
  bool WildCmp(const CString& sWild) const;
  /**
    * Turn all characters in this string into their upper-case equivalent.
    * @returns A reference to *this.
    */
  CString& MakeUpper();
  /**
    * Turn all characters in this string into their lower-case equivalent.
    * @returns A reference to *this.
    */
  CString& MakeLower();
  /**
    * Return a copy of this string with all characters turned into
    * upper-case.
    * @return The new string.
    */
  CString AsUpper() const;
  /**
    * Return a copy of this string with all characters turned into
    * lower-case.
    * @return The new string.
    */
  CString AsLower() const;
  static EEscape ToEscape(const CString& sEsc);
  CString Escape_n(EEscape eFrom, EEscape eTo) const;
  CString Escape_n(EEscape eTo) const;
  CString& Escape(EEscape eFrom, EEscape eTo);
  CString& Escape(EEscape eTo);
  /** Replace all occurrences in a string.
    *
    * You can specify a "safe zone" via sLeft and sRight. Anything inside
    * of such a zone will not be replaced. This does not do recursion, so
    * e.g. with <code>Replace("(a()a)", "a", "b", "(", ")", true)</code>
    * you would get "a(b)" as result. The second opening brace and the
    * second closing brace would not be seen as a delimitered and thus
    * wouldn't be removed. The first a is inside a "safe zone" and thus is
    * left alone, too.
    *
    * @param sStr The string to do the replacing on. This will also contain
    *             the result when this function returns.
    * @param sReplace The string that should be replaced.
    * @param sWith The replacement to use.
    * @param sLeft The string that marks the begin of the "safe zone".
    * @param sRight The string that marks the end of the "safe zone".
    * @param bRemoveDelims If this is true, all matches for sLeft and
    *                      sRight are removed.
    * @returns The number of replacements done.
    */
  static unsigned int Replace(CString& sStr, const CString& sReplace, const CString& sWith, const CString& sLeft = "", const CString& sRight = "", bool bRemoveDelims = false);
  /** Replace all occurrences in the current string.
    * @see CString::Replace
    * @param sReplace The string to look for.
    * @param sWith The replacement to use.
    * @param sLeft The delimiter at the beginning of a safe zone.
    * @param sRight The delimiter at the end of a safe zone.
    * @param bRemoveDelims If true, all matching delimiters are removed.
    * @return The result of the replacing. The current string is left
    *         unchanged.
    */
  CString Replace_n(const CString& sReplace, const CString& sWith, const CString& sLeft = "", const CString& sRight = "", bool bRemoveDelims = false) const;
  /** Replace all occurrences in the current string.
    * @see CString::Replace
    * @param sReplace The string to look for.
    * @param sWith The replacement to use.
    * @param sLeft The delimiter at the beginning of a safe zone.
    * @param sRight The delimiter at the end of a safe zone.
    * @param bRemoveDelims If true, all matching delimiters are removed.
    * @returns The number of replacements done.
    */
  unsigned int Replace(const CString& sReplace, const CString& sWith, const CString& sLeft = "", const CString& sRight = "", bool bRemoveDelims = false);
  /** Ellipsize the current string.
    * For example, ellipsizing "Hello, I'm Bob" to the length 9 would
    * result in "Hello,...".
    * @param uLen The length to ellipsize to.
    * @return The ellipsized string.
    */
  CString Ellipsize(unsigned int uLen) const;
  /** Return the left part of the string.
    * @param uCount The number of characters to keep.
    * @return The resulting string.
    */
  CString Left(unsigned int uCount) const;
  /** Return the right part of the string.
    * @param uCount The number of characters to keep.
    * @return The resulting string.
    */
  CString Right(unsigned int uCount) const;
  /** Get the first line of this string.
    * @return The first line of text.
    */
  CString FirstLine() const { return Token(0, false, "\n"); }
  /** Get a token out of this string. For example in the string "a bc d  e",
    *  each of "a", "bc", "d" and "e" are tokens.
    * @param uPos The number of the token you are interested. The first
    *             token has a position of 0.
    * @param bRest If false, only the token you asked for is returned. Else
    *              you get the substring starting from the beginning of
    *              your token.
    * @param sSep Seperator between tokens.
    * @param bAllowEmpty If this is true, empty tokens are allowed. In the
    *                    example from above this means that there is a
    *                    token "" before the "e" token.
    * @return The token you asked for and, if bRest is true, everything
    *         after it.
    * @see Split() if you need a string split into all of its tokens.
    */
  CString Token(unsigned int uPos, bool bRest = false, const CString& sSep = " ", bool bAllowEmpty = false) const;
  /** Get a token out of this string. This function behaves much like the
    *  other Token() function in this class. The extra arguments are
    *  handled similarly to Split().
    */
  CString Token(unsigned int uPos, bool bRest, const CString& sSep, bool bAllowEmpty, const CString& sLeft, const CString& sRight, bool bTrimQuotes = true) const;

  unsigned int QuoteSplit(VCString& vsRet) const;
  /** Split up this string into tokens.
    * Via sLeft and sRight you can define "markers" like with Replace().
    * Anything in such a marked section is treated as a single token. All
    * occurences of sDelim in such a block are ignored.
    * @param sDelim Delimiter between tokens.
    * @param vsRet Vector for returning the result.
    * @param bAllowEmpty Do empty tokens count as a valid token?
    * @param sLeft Left delimiter like with Replace().
    * @param sRight Right delimiter like with Replace().
    * @param bTrimQuotes Should sLeft and sRight be removed from the token
    *                    they mark?
    * @param bTrimWhiteSpace If this is true, CString::Trim() is called on
    *                        each token.
    * @return The number of tokens found.
    */
  unsigned int Split(const CString& sDelim, VCString& vsRet, bool bAllowEmpty = true,
                                      const CString& sLeft = "", const CString& sRight = "", bool bTrimQuotes = true,
                                      bool bTrimWhiteSpace = false) const;
  /** Split up this string into tokens.
    * This function is identical to the other CString::Split(), except that
    * the result is returned as a SCString instead of a VCString.
    */
  unsigned int Split(const CString& sDelim, SCString& ssRet, bool bAllowEmpty = true,
                                      const CString& sLeft = "", const CString& sRight = "", bool bTrimQuotes = true,
                                      bool bTrimWhiteSpace = false) const;
  /** Produces a random string.
    * @param uLength The length of the resulting string.
    * @return A random string.
    */
  static CString RandomString(unsigned int uLength);
  /** @return Converts a hex to html encoding */
  static CString HexToHtml(CString c);
  static unsigned int HexToInt(CString c);
  /** @return The hex that matches the interger */
  static CString IntToHex(int i);
  static CString ConvertUnicode(CString input, bool emoji = false);
  /** @return The MD5 hash of this string. */
  CString MD5() const;
  /** @return The SHA256 hash of this string. */
  CString SHA256() const;
  /** Treat this string as base64-encoded data and decode it.
    * @param sRet String to which the result of the decode is safed.
    * @return The length of the resulting string.
    */
  unsigned long Base64Decode(CString& sRet) const;
  /** Treat this string as base64-encoded data and decode it.
    *  The result is saved in this CString instance.
    * @return The length of the resulting string.
    */
  unsigned long Base64Decode();
  /** Treat this string as base64-encoded data and decode it.
    * @return The decoded string.
    */
  CString Base64Decode_n() const;
  /** Base64-encode the current string.
    * @param sRet String where the result is saved.
    * @param uWrap A boolean(!?!) that decides if the result should be
    *              wrapped after everywhere 57 characters.
    * @return true unless this code is buggy.
    */
  bool Base64Encode(CString& sRet, unsigned int uWrap = 0) const;
  /** Base64-encode the current string.
    *  This string is overwritten with the result of the encode.
    */
  bool Base64Encode(unsigned int uWrap = 0);
  /** Base64-encode the current string
    * @return The encoded string.
    */
  CString Base64Encode_n(unsigned int uWrap = 0) const;
  /** Pretty-print a percent value.
    * @param d The percent value. This should be in range 0-100.
    * @return The "pretty" string.
    */
  static CString ToPercent(double d);
  /** Pretty-print a number of bytes.
    * @param d The number of bytes.
    * @return A string describing the number of bytes.
    */
  static CString ToByteStr(unsigned long long d);
  /** Pretty-print a time span.
    * @param s Number of seconds to print.
    * @return A string like "4w 6d 4h 3m 58s".
    */
  static CString ToTimeStr(unsigned long s);
  /** @return True if this string is not "false". */
  bool ToBool() const;
  /** @return The numerical value of this string similar to atoi(). */
  short ToShort() const;
  /** @return The numerical value of this string similar to atoi(). */
  unsigned short ToUShort() const;
  /** @return The numerical value of this string similar to atoi(). */
  int ToInt() const;
  /** @return The numerical value of this string similar to atoi(). */
  long ToLong() const;
  /** @return The numerical value of this string similar to atoi(). */
  unsigned int ToUInt() const;
  /** @return The numerical value of this string similar to atoi(). */
  unsigned long ToULong() const;
  /** @return The numerical value of this string similar to atoi(). */
  unsigned long long ToULongLong() const;
  /** @return The numerical value of this string similar to atoi(). */
  long long ToLongLong() const;
  /** @return The numerical value of this string similar to atoi(). */
  double ToDouble() const;
  /** Trim this string. All leading/trailing occurences of characters from
    *  s are removed.
    * @param s A list of characters that should be trimmed.
    * @return true if this string was modified.
    */
  bool Trim(const CString& s = " \t\r\n");
  /** Trim this string. All leading occurences of characters from s are
    *  removed.
    * @param s A list of characters that should be trimmed.
    * @return true if this string was modified.
    */
  bool TrimLeft(const CString& s = " \t\r\n");
  /** Trim this string. All trailing occurences of characters from s are
    *  removed.
    * @param s A list of characters that should be trimmed.
    * @return true if this string was modified.
    */
  bool TrimRight(const CString& s = " \t\r\n");
  /** Trim this string. All leading/trailing occurences of characters from
    *  s are removed. This CString instance is not modified.
    * @param s A list of characters that should be trimmed.
    * @return The trimmed string.
    */
  CString Trim_n(const CString& s = " \t\r\n") const;
  /** Trim this string. All leading occurences of characters from s are
    *  removed. This CString instance is not modified.
    * @param s A list of characters that should be trimmed.
    * @return The trimmed string.
    */
  CString TrimLeft_n(const CString& s = " \t\r\n") const;
  /** Trim this string. All trailing occurences of characters from s are
    *  removed. This CString instance is not modified.
    * @param s A list of characters that should be trimmed.
    * @return The trimmed string.
    */
  CString TrimRight_n(const CString& s = " \t\r\n") const;
  /** Trim a given prefix.
    * @param sPrefix The prefix that should be removed.
    * @return True if this string was modified.
    */
  bool TrimPrefix(const CString& sPrefix);
  /** Trim a given suffix.
    * @param sSuffix The suffix that should be removed.
    * @return True if this string was modified.
    */
  bool TrimSuffix(const CString& sSuffix);
  /** Trim a given prefix.
    * @param sPrefix The prefix that should be removed.
    * @return A copy of this string without the prefix.
    */
  CString TrimPrefix_n(const CString& sPrefix) const;
  /** Trim a given suffix.
    * @param sSuffix The suffix that should be removed.
    * @return A copy of this string without the prefix.
    */
  CString TrimSuffix_n(const CString& sSuffix) const;
  /** Remove characters from the beginning of this string.
    * @param uLen The number of characters to remove.
    * @return true if this string was modified.
    */
  bool LeftChomp(unsigned int uLen = 1);
  /** Remove characters from the end of this string.
    * @param uLen The number of characters to remove.
    * @return true if this string was modified.
    */
  bool RightChomp(unsigned int uLen = 1);
  /** Remove characters from the beginning of this string.
    * This string object isn't modified.
    * @param uLen The number of characters to remove.
    * @return The result of the conversion.
    */
  CString LeftChomp_n(unsigned int uLen = 1) const;
  /** Remove characters from the end of this string.
    * This string object isn't modified.
    * @param uLen The number of characters to remove.
    * @return The result of the conversion.
    */
  CString RightChomp_n(unsigned int uLen = 1) const;
private:
protected:
  unsigned char* strnchr(const unsigned char* src, unsigned char c, unsigned int iMaxBytes, unsigned char* pFill = NULL, unsigned int* piCount = NULL) const;
};
#endif // !CSTRING_H
