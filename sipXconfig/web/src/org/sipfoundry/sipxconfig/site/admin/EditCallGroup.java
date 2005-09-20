/*
 * 
 * 
 * Copyright (C) 2005 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2005 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $
 */
package org.sipfoundry.sipxconfig.site.admin;

import java.util.Collection;

import org.apache.tapestry.AbstractComponent;
import org.apache.tapestry.IRequestCycle;
import org.apache.tapestry.callback.ICallback;
import org.apache.tapestry.callback.PageCallback;
import org.apache.tapestry.event.PageEvent;
import org.apache.tapestry.event.PageRenderListener;
import org.apache.tapestry.html.BasePage;
import org.apache.tapestry.valid.IValidationDelegate;
import org.sipfoundry.sipxconfig.admin.callgroup.CallGroup;
import org.sipfoundry.sipxconfig.admin.callgroup.CallGroupContext;
import org.sipfoundry.sipxconfig.components.StringSizeValidator;
import org.sipfoundry.sipxconfig.components.TapestryUtils;

public abstract class EditCallGroup extends BasePage implements PageRenderListener {
    public static final String PAGE = "EditCallGroup";

    public abstract CallGroupContext getCallGroupContext();

    public abstract Integer getCallGroupId();

    public abstract void setCallGroupId(Integer id);

    public abstract CallGroup getCallGroup();

    public abstract void setCallGroup(CallGroup callGroup);

    public abstract ICallback getCallback();

    public abstract void setCallback(ICallback callback);

    public void pageBeginRender(PageEvent event_) {
        CallGroup callGroup = getCallGroup();
        if (null != callGroup) {
            return;
        }
        Integer id = getCallGroupId();
        if (null != id) {
            CallGroupContext context = getCallGroupContext();
            callGroup = context.loadCallGroup(id);
        } else {
            callGroup = new CallGroup();
        }
        setCallGroup(callGroup);
        if (null == getCallback()) {
            setCallback(new PageCallback(ListCallGroups.PAGE));
        }
    }

    /**
     * Called when any of the submit components on the form is activated.
     * 
     * Usually submit components are setting properties. formSubmit will first check if the form
     * is valid, then it will call all the "action" listeners. Only one of the listeners (the one
     * that recognizes the property that is set) will actually do something. This is a strange
     * consequence of the fact that Tapestry listeners are pretty much useless because they are
     * called while the form is still rewinding and not all changes are committed to beans.
     * 
     * @param cycle current request cycle
     */
    public void formSubmit(IRequestCycle cycle) {
        if (!isValid()) {
            return;
        }
        UserRingTable ringTable = getUserRingTable();
        if (delete(ringTable) || move(ringTable)) {
            saveValid();
        }
        addRow(cycle, ringTable);
    }

    public void commit(IRequestCycle cycle_) {
        if (!isValid()) {
            return;
        }
        saveValid();
        getCallGroupContext().activateCallGroups();
    }

    /**
     * Saves current call group and displays add ring page.
     * 
     * @param cycle current request cycle
     * @param ringTable component with table of rings
     */
    private void addRow(IRequestCycle cycle, UserRingTable ringTable) {
        if (!ringTable.getAddRow()) {
            return;
        }
        saveValid();
        AddUserRing page = (AddUserRing) cycle.getPage(AddUserRing.PAGE);
        page.setCallGroupId(getCallGroupId());
        cycle.activate(page);
    }

    private boolean isValid() {
        IValidationDelegate delegate = TapestryUtils.getValidator(this);
        AbstractComponent component = (AbstractComponent) getComponent("common");
        StringSizeValidator descriptionValidator = (StringSizeValidator) component.getBeans()
                .getBean("descriptionValidator");
        descriptionValidator.validate(delegate);
        return !delegate.getHasErrors();
    }

    private void saveValid() {
        CallGroupContext context = getCallGroupContext();
        CallGroup callGroup = getCallGroup();
        context.storeCallGroup(callGroup);
        Integer id = getCallGroup().getId();
        setCallGroupId(id);
    }

    private boolean delete(UserRingTable ringTable) {
        Collection ids = ringTable.getRowsToDelete();
        if (null == ids) {
            return false;
        }
        CallGroup callGroup = getCallGroup();
        callGroup.removeRings(ids);
        return true;
    }

    private boolean move(UserRingTable ringTable) {
        int step = -1;
        Collection ids = ringTable.getRowsToMoveUp();
        if (null == ids) {
            step = 1;
            ids = ringTable.getRowsToMoveDown();
            if (null == ids) {
                // nothing to do
                return false;
            }
        }
        CallGroup callGroup = getCallGroup();
        callGroup.moveRings(ids, step);
        return true;
    }

    private UserRingTable getUserRingTable() {
        return (UserRingTable) getComponent("ringTable");
    }
}
