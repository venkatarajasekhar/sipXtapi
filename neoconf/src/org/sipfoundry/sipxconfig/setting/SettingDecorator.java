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
package org.sipfoundry.sipxconfig.setting;

import java.util.Collection;
import java.util.List;


/**
 * Wrap another setting object, delegate ALL methods to the Settting class
 */
public abstract class SettingDecorator implements Setting {
    
    private Setting m_delegate;
    
    /** BEAN ACCESS ONLY */
    public SettingDecorator() {        
    }
    
    public SettingDecorator(Setting delegate) {
        m_delegate = delegate;
    }
    
    protected Setting getDelegate() {
        return m_delegate;
    }
    
    protected void setDelegate(Setting delegate) {
        m_delegate = delegate;
    }
    
    public SettingGroup getSettingGroup() {
        return m_delegate.getSettingGroup();
    }

    public void setSettingGroup(SettingGroup settingGroup) {
        m_delegate.setSettingGroup(settingGroup);        
    }

    public String getPath() {
        return m_delegate.getPath();
    }

    public Setting addSetting(Setting setting) {
        return m_delegate.addSetting(setting);
    }

    public Setting getSetting(String name) {
        return m_delegate.getSetting(name);
    }

    public String getLabel() {
        return m_delegate.getLabel();
    }

    public void setLabel(String label) {
        m_delegate.setLabel(label);
    }

    public String getName() {
        return m_delegate.getName();
    }

    public void setName(String name) {
        m_delegate.setName(name);
    }

    public String getProfileName() {
        return m_delegate.getProfileName();
    }

    public void setProfileName(String profileName) {
        m_delegate.setProfileName(profileName);
    }

    public String getValue() {
        return m_delegate.getValue();
    }

    public void setValue(String value) {
        m_delegate.setValue(value);
    }

    public String getType() {
        return m_delegate.getType();
    }

    public void setType(String type) {
        m_delegate.setType(type);
    }

    public void addPossibleValue(String value) {
        m_delegate.addPossibleValue(value);
    }

    public List getPossibleValues() {
        return m_delegate.getPossibleValues();
    }

    public void setPossibleValues(List possibleValues) {
        m_delegate.setPossibleValues(possibleValues);
    }

    public String getDescription() {
        return m_delegate.getDescription();
    }

    public void setDescription(String description) {
        m_delegate.setDescription(description);
    }

    public Collection getValues() {
        return m_delegate.getValues();
    }

    public boolean isHidden() {
        return m_delegate.isHidden();
    }

    /** 
     * Does not use delegate! Assumes subclass is a Setting and not a SettingGroup
     */
    public void acceptVisitor(SettingVisitor visitor) {
        visitor.visitSetting(this);
    }

    public void setHidden(boolean hidden) {
        m_delegate.setHidden(hidden);
    }
}
