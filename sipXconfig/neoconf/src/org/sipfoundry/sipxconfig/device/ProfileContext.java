/*
 * 
 * 
 * Copyright (C) 2007 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2007 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $
 */
package org.sipfoundry.sipxconfig.device;

import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

import org.sipfoundry.sipxconfig.setting.BeanWithSettings;
import org.sipfoundry.sipxconfig.setting.Setting;
import org.sipfoundry.sipxconfig.setting.SettingFilter;
import org.sipfoundry.sipxconfig.setting.SettingSet;
import org.sipfoundry.sipxconfig.setting.SettingUtil;

/**
 * State used by profile generator
 */
public class ProfileContext {
    /**
     * Shows all settings and groups in a flat collection
     */
    private static final SettingFilter RECURSIVE_SETTINGS = new SettingFilter() {
        public boolean acceptSetting(Setting root_, Setting setting) {
            boolean group = SettingSet.class.isAssignableFrom(setting.getClass());
            return !group;
        }
    };

    private static final SettingFilter SETTINGS = new SettingFilter() {
        public boolean acceptSetting(Setting root, Setting setting) {
            boolean firstGeneration = (setting.getParent() == root);
            boolean isLeaf = setting.getValues().isEmpty();
            return firstGeneration && isLeaf;
        }
    };

    private BeanWithSettings m_device;

    public ProfileContext(BeanWithSettings device) {
        m_device = device;
    }

    public Map<String, Object> getContext() {
        HashMap<String, Object> context = new HashMap<String, Object>();
        context.put("phone", m_device);
        context.put("gateway", m_device);
        context.put("cfg", this);
        return context;
    }

    protected BeanWithSettings getDevice() {
        return m_device;
    }

    /**
     * Velocity macro convienence method. Recursive list of all settings, ignoring groups
     */
    public Collection getRecursiveSettings(Setting group) {
        return SettingUtil.filter(RECURSIVE_SETTINGS, group);
    }

    public Collection getSettings(Setting group) {
        return SettingUtil.filter(SETTINGS, group);
    }

    /**
     * Velocity macro convienence method for accessing endpoint settings
     */
    public Setting getEndpointSettings() {
        return m_device.getSettings();
    }
}