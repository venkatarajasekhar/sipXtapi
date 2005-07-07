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
package org.sipfoundry.sipxconfig.admin.dialplan;

import java.util.ArrayList;
import java.util.List;

import junit.framework.TestCase;

import org.sipfoundry.sipxconfig.admin.dialplan.config.Permission;
import org.sipfoundry.sipxconfig.admin.dialplan.config.UrlTransform;

/**
 * InternalRuleTest
 */
public class InternalRuleTest extends TestCase {

    private static final String RULE_NAME_PARAM = "%26name%3Dxcf-1";

    private static final String URL_PARAMS = ";play={voicemail}%2Fcgi-bin%2Fvoicemail%2Fmediaserver.cgi%3Faction%3D";
    private static final String URL_PREFIX = "<sip:{digits}@{mediaserver}" + URL_PARAMS;
    private static final String OPERATOR_URL = URL_PREFIX + "autoattendant" + RULE_NAME_PARAM
            + ">";
    private static final String VOICEMAIL_URL = URL_PREFIX + "retrieve%26mailbox%3D{digits}>";
    private static final String VOICEMAIL_FALLBACK_URL = URL_PREFIX
            + "deposit%26mailbox%3D{digits}>;q=0.1";
    private static final String VOICEMAIL_TRANSFER_URL = "<sip:{vdigits}@{mediaserver}"
            + URL_PARAMS + "deposit%26mailbox%3D{vdigits}>";

    public void testAppendToGenerationRules() throws Exception {
        InternalRule ir = new InternalRule();
        ir.setName("kuku");
        ir.setLocalExtensionLen(5);
        AutoAttendant aa = new AutoAttendant();
        aa.setExtension("100");
        ir.setAutoAttendant(aa);
        ir.setVoiceMail("20004");
        ir.setVoiceMailPrefix("7");
        ir.setEnabled(true);

        List rules = new ArrayList();
        ir.appendToGenerationRules(rules);

        assertEquals(4, rules.size());

        MappingRule o = (MappingRule) rules.get(0);
        MappingRule v = (MappingRule) rules.get(1);
        MappingRule vt = (MappingRule) rules.get(2);
        MappingRule vf = (MappingRule) rules.get(3);

        assertEquals("100", o.getPatterns()[0]);
        assertEquals(0, o.getPermissions().size());
        UrlTransform to = (UrlTransform) o.getTransforms()[0];
        assertEquals(OPERATOR_URL, to.getUrl());

        assertEquals("20004", v.getPatterns()[0]);
        assertEquals(0, v.getPermissions().size());
        UrlTransform tv = (UrlTransform) v.getTransforms()[0];
        assertEquals(VOICEMAIL_URL, tv.getUrl());

        assertEquals("7xxxxx", vt.getPatterns()[0]);
        assertEquals(0, vt.getPermissions().size());
        UrlTransform tvt = (UrlTransform) vt.getTransforms()[0];
        assertEquals(VOICEMAIL_TRANSFER_URL, tvt.getUrl());

        assertEquals("xxxxx", vf.getPatterns()[0]);
        assertEquals(Permission.VOICEMAIL, vf.getPermissions().get(0));
        UrlTransform tvf = (UrlTransform) vf.getTransforms()[0];
        assertEquals(VOICEMAIL_FALLBACK_URL, tvf.getUrl());
    }

    public void testAppendToGenerationRulesAutoAttandantOnly() throws Exception {
        InternalRule ir = new InternalRule();
        ir.setName("kuku");
        ir.setLocalExtensionLen(5);
        AutoAttendant aa = new AutoAttendant();
        aa.setExtension("100");
        ir.setAutoAttendant(aa);
        ir.setEnabled(true);
        ir.setVoiceMail("");
        ir.setVoiceMailPrefix("");
        List rules = new ArrayList();
        ir.appendToGenerationRules(rules);
        assertEquals(1, rules.size());

        MappingRule o = (MappingRule) rules.get(0);
        assertEquals("100", o.getPatterns()[0]);
        assertEquals(0, o.getPermissions().size());
        UrlTransform to = (UrlTransform) o.getTransforms()[0];
        assertEquals(OPERATOR_URL, to.getUrl());
    }

}
