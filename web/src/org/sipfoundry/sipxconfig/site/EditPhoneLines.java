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
package org.sipfoundry.sipxconfig.site;

import java.util.ArrayList;
import java.util.List;

import org.apache.tapestry.IRequestCycle;
import org.apache.tapestry.event.PageEvent;
import org.apache.tapestry.event.PageRenderListener;
import org.apache.tapestry.html.BasePage;
import org.sipfoundry.sipxconfig.components.PhonePageUtils;
import org.sipfoundry.sipxconfig.components.SelectMap;
import org.sipfoundry.sipxconfig.phone.EndpointLine;
import org.sipfoundry.sipxconfig.phone.Phone;


/**
 * Comments
 */
public abstract class EditPhoneLines extends BasePage implements PageRenderListener {

    public static final String PAGE = "EditPhoneLines";

    public abstract Phone getPhone();
    
    public abstract void setPhone(Phone phone);

    public abstract List getEndpointLines();
    
    public abstract void setEndpointLines(List endpointLines);
    
    public abstract EndpointLine getCurrentRow();

    public abstract void setCurrentRow(EndpointLine line);
    
    public abstract SelectMap getSelections();
    
    public abstract void setSelections(SelectMap selections);
    
    public void pageBeginRender(PageEvent eventTemp) {
        setEndpointLines(new ArrayList());
        
        // Generate the list of phone items
        if (getSelections() == null) {
            setSelections(new SelectMap());
        }
    }
    
    public void newLine(IRequestCycle cycle) {
        Phone phone = PhonePageUtils.getPhoneFromParameter(cycle, 0);
        NewLine page = (NewLine) cycle.getPage(NewLine.PAGE);
        page.setPhone(phone);
        page.setReturnPage(PAGE);
        cycle.activate(page);
    }

}
