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
package org.sipfoundry.sipxconfig.core;

public class RefProperty {

    private int m_id;

    private String m_content;

    private int m_profileType;

    private String m_name;

    private String m_code;

    public int getId() {
        return m_id;
    }

    public void setId(int id) {
        this.m_id = id;
    }

    public String getContent() {
        return m_content;
    }

    public void setContent(String content) {
        this.m_content = content;
    }

    public int getProfileType() {
        return m_profileType;
    }

    public void setProfileType(int profile_type) {
        this.m_profileType = profile_type;
    }

    public String getName() {
        return m_name;
    }

    public void setName(String name) {
        this.m_name = name;
    }

    public String getCode() {
        return m_code;
    }

    public void setCode(String code) {
        this.m_code = code;
    }

}

