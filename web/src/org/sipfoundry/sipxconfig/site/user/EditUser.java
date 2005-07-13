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
package org.sipfoundry.sipxconfig.site.user;

import org.apache.tapestry.IRequestCycle;
import org.apache.tapestry.event.PageEvent;
import org.apache.tapestry.event.PageRenderListener;
import org.apache.tapestry.html.BasePage;
import org.sipfoundry.sipxconfig.common.CoreContext;
import org.sipfoundry.sipxconfig.common.User;

public abstract class EditUser extends BasePage implements PageRenderListener {
    
    public static final String PAGE = "EditUser";
    
    public abstract void setUserId(Integer userId);
    
    public abstract String getReturnPage();
    
    public abstract void setReturnPage(String returnPage);
    
    public abstract CoreContext getCoreContext();
    
    public abstract User getUser();
    
    public abstract Integer getUserId();
    
    public abstract void setUser(User user);
    
    public void editUser(Integer userId, String returnPage) {
        setUserId(userId);
        setReturnPage(returnPage);
    }

    public void ok(IRequestCycle cycle) {
        save();
        cycle.activate(getReturnPage());
    }

    public void apply(IRequestCycle cycle_) {
        save();
    }
    
    private void save() {
        getCoreContext().saveUser(getUser());
    }
    
    public void cancel(IRequestCycle cycle) {
        cycle.activate(getReturnPage());
    }

    public void pageBeginRender(PageEvent event_) {
        User user = getCoreContext().loadUser(getUserId());
        setUser(user);
    }
}
