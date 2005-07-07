/*
 * 
 * 
 * Copyright (C) 2004 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2004 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $
 */
package org.sipfoundry.sipxconfig.site.phone;

import org.apache.tapestry.BaseComponent;
import org.apache.tapestry.IRequestCycle;
import org.sipfoundry.sipxconfig.phone.Phone;
import org.sipfoundry.sipxconfig.phone.PhoneContext;


public abstract class PhoneLabel extends BaseComponent {
    
    public abstract Phone getPhone();
    
    public abstract void setPhone(Phone phone);
 
    public abstract PhoneContext getPhoneContext();

    public void prepareForRender(IRequestCycle cycle) {
        super.prepareForRender(cycle);        
    }        
}
