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

/**
 * Database services for setting business objects
 */
public interface SettingDao {
       
    public void storeMetaStorage(MetaStorage meta);

    public MetaStorage loadRootMetaStorage();
    
    public void storeValueStorage(ValueStorage storage);

    public ValueStorage loadValueStorage(int storageId);
}
