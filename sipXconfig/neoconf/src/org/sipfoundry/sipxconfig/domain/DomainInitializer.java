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
package org.sipfoundry.sipxconfig.domain;

import java.net.InetAddress;
import java.net.UnknownHostException;

import org.sipfoundry.sipxconfig.common.InitTaskListener;

/**
 * When system first starts up, create initial domain object w/default value(s)
 */
public class DomainInitializer extends InitTaskListener {
    private DomainManager m_domainManager;
    private String m_initialDomain;

    public void setInitialDomain(String initialDomain) {
        m_initialDomain = initialDomain;
    }

    public void setDomainManager(DomainManager domainManager) {
        m_domainManager = domainManager;
    }

    @Override
    public void onInitTask(String task) {
        Domain domain = new Domain();
        domain.setName(getInitialDomainName());
        m_domainManager.saveDomain(domain);
    }

    String getInitialDomainName() {
        if (m_initialDomain != null) {
            return m_initialDomain;
        }

        try {
            InetAddress addr = InetAddress.getLocalHost();
            m_initialDomain = addr.getHostName();
        } catch (UnknownHostException e) {
            throw new RuntimeException("Could not determine hostname", e);
        }
        return m_initialDomain;
    }
}
