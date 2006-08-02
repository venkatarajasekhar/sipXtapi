// 
//
// Copyright (C) 2004 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// Copyright (C) 2004 Pingtel Corp.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// $$
//////////////////////////////////////////////////////////////////////////////

#ifndef URLMAPPING_H
#define URLMAPPING_H

// SYSTEM INCLUDES
//#include <...>

// APPLICATION INCLUDES
#include <os/OsDefs.h>
#include "os/OsStatus.h"
#include "sipdb/ResultSet.h"
#include "xmlparser/tinyxml.h"

// DEFINES
#define XML_TAG_MAPPINGS            "mappings"

#define XML_TAG_HOSTMATCH           "hostMatch"
#define XML_TAG_HOSTPATTERN         "hostPattern"

#define XML_TAG_USERMATCH           "userMatch"
#define XML_TAG_USERPATTERN         "userPattern"

#define XML_TAG_PERMISSIONMATCH     "permissionMatch"
#define XML_TAG_PERMISSION          "permission"
#define XML_ATT_AUTHTYPE            "authType"

#define XML_PERMISSION_911          "emergency-dialing"
#define XML_PERMISSION_1800         "1800-dialing"
#define XML_PERMISSION_1900         "1900-dialing"
#define XML_PERMISSION_1877         "1877-dialing"
#define XML_PERMISSION_1888         "1888-dialing"
#define XML_PERMISSION_1            "domestic-dialing"
#define XML_PERMISSION_011          "international-dialing"

#define XML_TAG_TRANSFORM           "transform"
#define XML_TAG_URL                 "url"
#define XML_TAG_HOST                "host"
#define XML_TAG_USER                "user"
#define XML_TAG_FIELDPARAMS         "fieldparams"
#define XML_TAG_URLPARAMS           "urlparams"
#define XML_TAG_HEADERPARAMS        "headerparams"

#define XML_SYMBOL_USER             "{user}"
#define XML_SYMBOL_DIGITS           "{digits}"
#define XML_SYMBOL_HOST             "{host}"
#define XML_SYMBOL_HEADERPARAMS     "{headerparams}"
#define XML_SYMBOL_URLPARAMS        "{urlparams}"
#define XML_SYMBOL_URI              "{uri}"
#define XML_SYMBOL_LOCALHOST        "{localhost}"
#define XML_SYMBOL_MEDIASERVER      "{mediaserver}"
#define XML_SYMBOL_VOICEMAIL        "{voicemail}"
#define XML_SYMBOL_VDIGITS          "{vdigits}"

class TiXmlNode;
class Url;
class RegEx;
class UrlMappingTest;

// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS

class UrlMapping
{
/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:

/* ============================ CREATORS ================================== */

    UrlMapping();

    virtual ~UrlMapping();
    //:Destructor

/* ============================ MANIPULATORS ============================== */

/* ============================ ACCESSORS ================================= */
    OsStatus loadMappings(
        const UtlString configFileName,
        const UtlString mediaserver = "",
        const UtlString& voicemail = "",
        const UtlString& localhost = "");

    OsStatus getPermissionRequired(
        const Url& requestUri,
        UtlBoolean& isPSTNNumber,
        ResultSet& rPermissions);

    OsStatus getContactList(const Url& requestUri,
        ResultSet& rRegistratons,
        UtlBoolean& isPSTNNumber,
        ResultSet& rPermissions);

/* //////////////////////////// PROTECTED ///////////////////////////////// */
protected:
    friend class UrlMappingTest;
    
    TiXmlNode* mPrevMappingNode;
    TiXmlElement* mPrevMappingElement;
    TiXmlNode* mPrevHostMatchNode;
    TiXmlNode* mPrevUserMatchNode;
    TiXmlNode* mPrevPermMatchNode;
    TiXmlDocument *mDoc;
    UtlBoolean mParseFirstTime;
    UtlString mVoicemail;
    UtlString mLocalhost;
    UtlString mMediaServer;

    OsStatus parseHostMatchContainer(const Url& requestUri,
        ResultSet& rRegistratons,
        UtlBoolean& isPSTNNumber,
        UtlBoolean& doTransform,
        ResultSet& rPermissions,
        TiXmlNode* mappingsNode,
        TiXmlNode* previousHostMatchNode = NULL);

    OsStatus parseUserMatchContainer(const Url& requestUri,
        ResultSet& rRegistratons,
        UtlBoolean& isPSTNNumber,
        UtlBoolean& doTransform,
        ResultSet& rPermissions,
        TiXmlNode* hostMatchNode,
        TiXmlNode* previousUserMatchNode = NULL);

    OsStatus parsePermMatchContainer(const Url& requestUri,
        const UtlString& vdigits,
        ResultSet& rRegistratons,
        UtlBoolean& doTransform,
        ResultSet& rPermissions,
        TiXmlNode* userMatchNode,
        TiXmlNode* previousPermMatchNode = NULL);

    OsStatus doTransform(const Url& requestUri,
        const UtlString& vdigits,
        ResultSet& rRegistratons,
        TiXmlNode* permMatchNode);

    void replaceAll(const UtlString& originalString,
        UtlString &modifiedString,
        const UtlString& replaceWhat,
        const UtlString& replaceWith);

   void convertDialString2RegEx(const UtlString& source, UtlString& regExp);

   void getVDigits(RegEx& userPattern,
      UtlString& vdigits);

    void replaceSymbols(const UtlString &string,
      const Url& requestUri,
      const UtlString& vdigits,
      UtlString& modifiedString);

/* //////////////////////////// PRIVATE /////////////////////////////////// */
private:

};

/* ============================ INLINE METHODS ============================ */

#endif  // URLMAPPING_H

