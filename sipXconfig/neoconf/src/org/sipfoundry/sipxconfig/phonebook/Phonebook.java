/*
 * 
 * 
 * Copyright (C) 2006 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2006 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $
 */
package org.sipfoundry.sipxconfig.phonebook;

import java.util.Collection;
import java.util.Set;
import java.util.TreeSet;

import org.sipfoundry.sipxconfig.common.BeanWithId;
import org.sipfoundry.sipxconfig.common.NamedObject;
import org.sipfoundry.sipxconfig.setting.Group;

public class Phonebook extends BeanWithId implements NamedObject {
    private String m_membersCsvFilename;
    private String m_name;
    private String m_description;
    private Set<Group> m_members = new TreeSet<Group>();
    private Set<Group> m_consumers = new TreeSet<Group>();
    
    public String getMembersCsvFilename() {
        return m_membersCsvFilename;
    }
    
    public void setMembersCsvFilename(String externalUsersFilename) {
        m_membersCsvFilename = externalUsersFilename;
    }
    
    public Set<Group> getMembers() {
        return m_members;
    }
    
    public void setMembers(Set<Group> members) {
        m_members = members;
    }
    
    public void replaceMembers(Collection<Group> groups) {
        m_members.clear();
        m_members.addAll(groups);
    }

    public void replaceConsumers(Collection<Group> groups) {
        m_consumers.clear();
        m_consumers.addAll(groups);
    }

    public String getName() {
        return m_name;
    }

    public void setName(String name) {
        m_name = name;
    }

    public Set<Group> getConsumers() {
        return m_consumers;
    }

    public void setConsumers(Set<Group> consumers) {
        m_consumers = consumers;
    }

    public String getDescription() {
        return m_description;
    }

    public void setDescription(String description) {
        m_description = description;
    }       
}