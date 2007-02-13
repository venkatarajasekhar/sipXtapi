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
package org.sipfoundry.sipxconfig.site.dialplan;

import java.util.Collection;

import org.apache.tapestry.IPage;
import org.apache.tapestry.IRequestCycle;
import org.apache.tapestry.html.BasePage;
import org.apache.tapestry.valid.IValidationDelegate;
import org.apache.tapestry.valid.ValidationConstraint;
import org.sipfoundry.sipxconfig.admin.dialplan.AttendantInUseException;
import org.sipfoundry.sipxconfig.admin.dialplan.AutoAttendant;
import org.sipfoundry.sipxconfig.admin.dialplan.DialPlanContext;
import org.sipfoundry.sipxconfig.admin.dialplan.VxmlGenerator;
import org.sipfoundry.sipxconfig.components.SelectMap;
import org.sipfoundry.sipxconfig.components.TapestryUtils;
import org.sipfoundry.sipxconfig.setting.Group;
import org.sipfoundry.sipxconfig.site.setting.GroupSettings;

public abstract class ManageAttendants extends BasePage {

    public static final String PAGE = "ManageAttendants";
    public static final String DEFAULTS_PAGE = "EditAttendantDefaults";

    public abstract DialPlanContext getDialPlanContext();

    public abstract VxmlGenerator getVxmlGenerator();

    public abstract AutoAttendant getCurrentRow();

    public abstract SelectMap getSelections();

    public void deleteSelected() {
        Collection selectedRows = getSelections().getAllSelected();
        if (selectedRows != null) {
            DialPlanContext manager = getDialPlanContext();
            try {
                manager.deleteAutoAttendantsByIds(selectedRows, getVxmlGenerator()
                        .getScriptsDirectory());
            } catch (AttendantInUseException e) {
                IValidationDelegate validator = TapestryUtils.getValidator(this);
                validator.record(e.getMessage(), ValidationConstraint.CONSISTENCY);
            }
        }
    }

    public IPage edit(IRequestCycle cycle, Integer id) {
        EditAutoAttendant page = (EditAutoAttendant) cycle.getPage(EditAutoAttendant.PAGE);
        AutoAttendant attendant = getDialPlanContext().getAutoAttendant(id);
        page.setAttendant(attendant);
        page.setReturnPage(PAGE);
        return page;
    }

    public IPage add(IRequestCycle cycle) {
        EditAutoAttendant page = (EditAutoAttendant) cycle.getPage(EditAutoAttendant.PAGE);
        page.setAttendant(null);
        page.setReturnPage(PAGE);
        return page;
    }
    
    public IPage defaultGroup(IRequestCycle cycle) {
        GroupSettings page = (GroupSettings) cycle.getPage(DEFAULTS_PAGE);
        Group defaultGroup = getDialPlanContext().getDefaultAutoAttendantGroup();
        AutoAttendant aa = getDialPlanContext().newAutoAttendantWithDefaultGroup();
        page.editGroup(defaultGroup.getId(), aa, PAGE);
        return page;        
    }
}