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
package org.sipfoundry.sipxconfig.conference;

import junit.framework.TestCase;

import org.easymock.MockControl;
import org.easymock.classextension.MockClassControl;
import org.sipfoundry.sipxconfig.TestHelper;
import org.sipfoundry.sipxconfig.phone.PhoneDefaults;
import org.sipfoundry.sipxconfig.setting.type.FileSetting;

public class BridgeTest extends TestCase {

    public void testInsertConference() {
        Conference c = new Conference();
        c.setUniqueId();

        Bridge bridge = new Bridge();
        assertTrue(bridge.getConferences().isEmpty());
        bridge.insertConference(c);

        assertEquals(1, bridge.getConferences().size());
        assertSame(c, bridge.getConferences().iterator().next());

        assertSame(bridge, c.getBridge());
    }

    public void testRemoveConference() {
        Conference c = new Conference();
        c.setUniqueId();

        Conference c1 = new Conference();
        c1.setUniqueId();

        Bridge bridge = new Bridge();
        assertTrue(bridge.getConferences().isEmpty());
        bridge.insertConference(c);

        bridge.removeConference(c1);
        assertEquals(1, bridge.getConferences().size());

        bridge.removeConference(c);
        assertTrue(bridge.getConferences().isEmpty());
        assertNull(c.getBridge());
    }

    public void testGetDefaults() {
        final String audioDir = "/really/strange/directory";

        MockControl defaultsCtrl = MockClassControl.createControl(PhoneDefaults.class);
        PhoneDefaults defaults = (PhoneDefaults) defaultsCtrl.getMock();
        defaults.getDomainName();
        defaultsCtrl.setReturnValue("xyz.org");
        defaultsCtrl.replay();

        Bridge bridge = (Bridge) TestHelper.getApplicationContext().getBean(Bridge.BEAN_NAME);
        bridge.setAudioDirectory(audioDir);

        bridge.setSystemDefaults(defaults);

        assertEquals("xyz.org", bridge.getSettingValue(Bridge.SIP_DOMAIN));

        FileSetting settingType = (FileSetting) bridge.getSettings().getSetting(
                "bridge-bridge/BOSTON_BRIDGE_HOLD_MUSIC").getType();

        assertEquals(audioDir, settingType.getDirectory());

        defaultsCtrl.verify();
    }
}
