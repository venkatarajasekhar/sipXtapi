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
package org.sipfoundry.sipxconfig.common;

import java.util.Iterator;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;

import junit.framework.TestCase;

import org.apache.commons.lang.StringUtils;
import org.sipfoundry.sipxconfig.TestHelper;
import org.sipfoundry.sipxconfig.admin.forwarding.AliasMapping;
import org.sipfoundry.sipxconfig.permission.Permission;
import org.sipfoundry.sipxconfig.permission.PermissionManagerImpl;
import org.sipfoundry.sipxconfig.setting.Group;
import org.sipfoundry.sipxconfig.setting.Setting;

public class UserTest extends TestCase {

    public void testGetDisplayName() {
        User u = new User();
        assertNull(u.getDisplayName());
        u.setUserName("bob");
        assertNull(u.getDisplayName());
        u.setFirstName("First");
        assertEquals("First", u.getDisplayName());
        u.setLastName("Last");
        assertEquals("First Last", u.getDisplayName());
    }

    public void testGetUri() {
        User user = new User();
        user.setUserName("username");
        String uri = user.getUri("mycomp.com");

        assertEquals("sip:username@mycomp.com", uri);

        user.setLastName("Last");
        uri = user.getUri("mycomp.com");
        assertEquals("\"Last\"<sip:username@mycomp.com>", uri);

        user.setFirstName("First");
        uri = user.getUri("mycomp.com");
        assertEquals("\"First Last\"<sip:username@mycomp.com>", uri);
    }

    /** Test that setting a typical PIN yields expected results */
    public void testSetPin() throws Exception {
        checkSetPin("pin");
    }

    /** Test that setting a null PIN yields expected results */
    public void testSetNullPin() throws Exception {
        checkSetPin(null);
    }

    private void checkSetPin(String pin) throws Exception {
        User user = new User();
        user.setUserName("username");
        user.setPin(pin, "realm.sipfoundry.org");
        String pintoken = getPintoken("username", pin);
        assertEquals(pintoken, user.getPintoken());
    }

    public void testGetSipPasswordHash() throws Exception {
        User user = new User();
        user.setUserName("username");
        user.setSipPassword("sip password");
        String hash = Md5Encoder.digestPassword("username", "realm.sipfoundry.org",
                "sip password");

        assertEquals(hash, user.getSipPasswordHash("realm.sipfoundry.org"));
    }

    public void testGetSipPasswordHashEmpty() throws Exception {
        User user = new User();
        user.setUserName("username");
        user.setSipPassword(null);
        String hash = Md5Encoder.digestPassword("username", "realm.sipfoundry.org", "");

        assertEquals(hash, user.getSipPasswordHash("realm.sipfoundry.org"));
    }

    public void testGetSipPasswordHashMd5() throws Exception {
        User user = new User();
        user.setUserName("username");
        String hash = Md5Encoder.digestPassword("username", "realm.sipfoundry.org", "");
        user.setSipPassword(hash);

        String newHash = Md5Encoder.digestPassword("username", "realm.sipfoundry.org", hash);

        assertFalse(hash.equals(newHash));
        assertEquals(newHash, user.getSipPasswordHash("realm.sipfoundry.org"));
    }

    public void testGetAliases() {
        User user = new User();
        user.setUserName("username");

        Set aliases = new LinkedHashSet(); // use LinkedHashSet for stable ordering
        aliases.add("mambo");
        aliases.add("tango");
        user.setAliases(aliases);
        assertEquals("mambo tango", user.getAliasesString());
        checkAliases(user);

        user.setAliases(new LinkedHashSet());
        user.setAliasesString("mambo tango");
        checkAliases(user);

        List aliasMappings = user.getAliasMappings("sipfoundry.org");
        assertEquals(2, aliasMappings.size());
        AliasMapping alias = (AliasMapping) aliasMappings.get(0);
        assertEquals("mambo@sipfoundry.org", alias.getIdentity());

        final String CONTACT = "sip:username@sipfoundry.org";
        assertEquals(CONTACT, alias.getContact());
        alias = (AliasMapping) aliasMappings.get(1);
        assertEquals("tango@sipfoundry.org", alias.getIdentity());
        assertEquals(CONTACT, alias.getContact());
    }

    private void checkAliases(User user) {
        Set aliasesCheck = user.getAliases();
        assertEquals(2, aliasesCheck.size());
        Iterator i = aliasesCheck.iterator();
        assertEquals("mambo", i.next());
        assertEquals("tango", i.next());
    }

    public void testGetEmptyAliases() {
        User user = new User();
        user.setUserName("username");
        List aliasMappings = user.getAliasMappings("sipfoundry.org");
        assertEquals(0, aliasMappings.size());
    }

    public void testHasPermission() {
        PermissionManagerImpl pm = new PermissionManagerImpl();
        pm.setModelFilesContext(TestHelper.getModelFilesContext());

        User user = new User();
        user.setPermissionManager(pm);

        Group group = new Group();
        user.addGroup(group);

        String path = Permission.SUPERADMIN.getSettingPath();
        Setting superAdmin = user.getSettings().getSetting(path);
        assertNotNull(superAdmin);
        assertFalse(user.hasPermission(Permission.SUPERADMIN));
    }

    public void testGetExtension() throws Exception {
        User user = new User();
        user.setUserName("abc");

        assertNull(user.getExtension(true));

        user.addAliases(new String[] {
            "5a", "oooi", "333", "xyz", "4444"
        });
        assertEquals("333", user.getExtension(true));
        assertFalse(user.hasNumericUsername());

        user.addAlias("2344");
        assertEquals("333", user.getExtension(true));

        user.addAlias("01");
        assertEquals("01", user.getExtension(true));

        user.setUserName("12345");
        assertEquals("12345", user.getExtension(true));
        assertTrue(user.hasNumericUsername());

        user.setUserName("12345");
        assertEquals("01", user.getExtension(false));

        // make sure 0 is ignored as possible extension
        user.addAlias("0");
        assertEquals("01", user.getExtension(false));
    }

    private String getPintoken(String username, String pin) {
        // handle null pin
        String safePin = StringUtils.defaultString(pin);
        return Md5Encoder.digestPassword(username, "realm.sipfoundry.org", safePin);
    }
}
