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
package org.sipfoundry.sipxconfig.admin.dialplan;

import java.util.Collection;
import java.util.List;

public interface FlexibleDialPlanContext {

    public boolean addRule(IDialingRule rule);

    public List getRules();

    public void setRules(List rules);

    public DialingRule getRule(Integer id);

    public void deleteRules(Collection selectedRows);

    public boolean updateRule(Integer id, DialingRule rule);

    public void duplicateRules(Collection selectedRows);

    public List getGenerationRules();
}