/*
 * 
 * 
 * Copyright (C) 2004 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2004 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $$
 */


package com.pingtel.pds.jbossauth;

import javax.security.auth.login.LoginException;

/**
 * <description of class including its purpose>
 * @author ibutcher
 * 
 */
public class Stub implements SIPxchangeAuthPlugin {


//////////////////////////////////////////////////////////////////////////
// Constants
////


//////////////////////////////////////////////////////////////////////////
// Attributes
////


//////////////////////////////////////////////////////////////////////////
// Construction
////


//////////////////////////////////////////////////////////////////////////
// Public Methods
////
    public boolean isValidUser(String userName, String password)
            throws LoginException {
        return true;
    }


//////////////////////////////////////////////////////////////////////////
// Implementation Methods
////


//////////////////////////////////////////////////////////////////////////
// Nested / Inner classes
////    


//////////////////////////////////////////////////////////////////////////
// Native Method Declarations
////

}
