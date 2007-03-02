// 
// 
// Copyright (C) 2006 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
// 
// Copyright (C) 2006 Pingtel Corp.
// Licensed to SIPfoundry under a Contributor Agreement.
// 
// $$
//////////////////////////////////////////////////////////////////////////////

#ifndef _RlsSubscribePolicy_h_
#define _RlsSubscribePolicy_h_

// SYSTEM INCLUDES

// APPLICATION INCLUDES
#include <net/SipSubscribeServerEventHandler.h>

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// FORWARD DECLARATIONS
class SipMessage;
class UtlString;
class SipPublishContentMgr;

// TYPEDEFS

//! Specialize SipSubscribeServerEventHandler to enforce RLS policies.
/*! In particular, SUBSCRIBEs that do not support 'eventlist' are to be
 *  rejected.
 */
class RlsSubscribePolicy : public SipSubscribeServerEventHandler
{
/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:

/* ============================ CREATORS ================================== */

    //! Default Dialog constructor
    RlsSubscribePolicy();

    //! Destructor
    virtual
    ~RlsSubscribePolicy();

/* ============================ MANIPULATORS ============================== */

    //! Determine if the given SUBSCRIBE request is authorized to subscribe
    /*! Only allowed if 'eventlist' is supported.
     */
    virtual UtlBoolean isAuthorized(const SipMessage& subscribeRequest,
                                    const UtlString& resourceId,
                                    const UtlString& eventTypeKey,
                                    SipMessage& subscribeResponse);

/* ============================ ACCESSORS ================================= */


/* ============================ INQUIRY =================================== */


/* //////////////////////////// PROTECTED ///////////////////////////////// */
protected:

/* //////////////////////////// PRIVATE /////////////////////////////////// */
private:
    //! Copy constructor NOT ALLOWED
    RlsSubscribePolicy(const RlsSubscribePolicy& rRlsSubscribePolicy);

    //! Assignment operator NOT ALLOWED
    RlsSubscribePolicy& operator=(const RlsSubscribePolicy& rhs);

};

/* ============================ INLINE METHODS ============================ */

#endif  // _RlsSubscribePolicy_h_