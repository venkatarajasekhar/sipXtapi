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

import java.io.File;
import java.io.IOException;
import java.io.OutputStream;

import junit.framework.TestCase;

import org.apache.commons.io.IOUtils;
import org.sipfoundry.sipxconfig.TestHelper;
import org.sipfoundry.sipxconfig.gateway.Gateway;
import org.sipfoundry.sipxconfig.phone.Phone;
import org.sipfoundry.sipxconfig.phone.TestPhone;

public class DeviceTest extends TestCase {

    private final class ProfileGeneratorStub extends AbstractProfileGenerator {
        protected void generateProfile(ProfileContext context, OutputStream out) throws IOException {
            IOUtils.write("profile", out);
        }
    }

    public void testGenerateAndRemoveProfilesPhone() {
        AbstractProfileGenerator generator = new ProfileGeneratorStub();

        String root = TestHelper.getTestDirectory() + "/phone";
        FileSystemProfileLocation location = new FileSystemProfileLocation();
        location.setParentDir(root);

        generator.setProfileLocation(location);

        final String profileFilename = "gateway.cfg";

        Phone device = new TestPhone() {
            public String getPhoneFilename() {
                return profileFilename;
            }
        };

        device.setProfileGenerator(generator);

        File profile = new File(root, profileFilename);
        device.generateProfiles();
        assertTrue(profile.exists());
        device.removeProfiles();
        assertFalse(profile.exists());

        new File(root).delete();
    }

    public void testGenerateAndRemoveProfilesGateway() {
        AbstractProfileGenerator generator = new ProfileGeneratorStub();

        String root = TestHelper.getTestDirectory() + "/gateway";
        FileSystemProfileLocation location = new FileSystemProfileLocation();
        location.setParentDir(root);

        generator.setProfileLocation(location);

        final String profileFilename = "gateway.cfg";

        Gateway device = new Gateway() {
            protected String getProfileFilename() {
                return profileFilename;
            }
        };

        device.setProfileGenerator(generator);

        File profile = new File(root, profileFilename);
        device.generateProfiles();
        assertTrue(profile.exists());
        device.removeProfiles();
        assertFalse(profile.exists());

        new File(root).delete();
    }
}
