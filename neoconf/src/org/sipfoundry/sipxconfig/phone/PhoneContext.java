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
package org.sipfoundry.sipxconfig.phone;

import java.util.Collection;
import java.util.Map;

import org.sipfoundry.sipxconfig.setting.Folder;



/**
 * Context for entire sipXconfig framework. Holder for service layer bean factories.
 */
public interface PhoneContext {
    
    public static final String CONTEXT_BEAN_NAME = "phoneContext";
        
    /** 
     * int value for objects that haven't been saved to database yet
     * <pre>
     *   Example:
     *      public class MyObject {
     * 
     *          prviate int id = PhoneDao.UNSAVED_ID
     *         
     *           ...
     * </pre>
     */
    public static final Integer UNSAVED_ID = new Integer(-1);
    
    public Map getPhoneFactoryIds();

    public void setPhoneFactoryIds(Map phoneIds);

    /**
     * Commits the transaction and performs a batch of SQL commands
     * to database. Call this as high in the application stack as possible
     * for better performance and data integrity.
     * 
     * You need to call this before you attempt to delete an object
     * that was created before last call to flush. Not unreasonable, most
     * times you don't delete and object before it's created, but happens
     * a lot in unittests. 
     */
    public void flush();
    
    public Collection loadPhones();      

    public void storeCredential(Credential credential);
    
    public void deleteCredential(Credential credential);

    public Credential loadCredential(Integer id);

    public void storeLine(Line line);
    
    public void deleteLine(Line line);

    public Line loadLine(Integer id);
    
    public Phone newPhone(String factoryId);

    public Phone loadPhone(Integer id);
    
    public Object load(Class c, Integer id);
        
    public void storePhone(Phone phone);

    public void deletePhone(Phone phone);
    
    public Folder loadRootPhoneFolder();
    
    public Folder loadRootLineFolder();
    
    /** unittesting only */
    public void clear();
}
