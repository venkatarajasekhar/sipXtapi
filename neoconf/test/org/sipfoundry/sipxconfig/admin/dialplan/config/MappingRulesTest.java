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
package org.sipfoundry.sipxconfig.admin.dialplan.config;

import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import org.custommonkey.xmlunit.XMLTestCase;
import org.custommonkey.xmlunit.XMLUnit;
import org.dom4j.Document;
import org.dom4j.Element;
import org.dom4j.VisitorSupport;
import org.easymock.MockControl;
import org.sipfoundry.sipxconfig.admin.dialplan.FlexibleDialPlan;
import org.sipfoundry.sipxconfig.admin.dialplan.Gateway;
import org.sipfoundry.sipxconfig.admin.dialplan.IDialingRule;
import org.sipfoundry.sipxconfig.admin.dialplan.MappingRule;

/**
 * MappingRulesTest
 */
public class MappingRulesTest extends XMLTestCase {
    public void testGetDocument() throws Exception {
        MappingRules mappingRules = new MappingRules();
        Document document = mappingRules.getDocument();

        org.w3c.dom.Document domDoc = TransformTest.getDomDoc(document);
        assertXpathExists("/mappings/hostMatch/hostPattern", domDoc);
        assertXpathEvaluatesTo("${SIPXCHANGE_DOMAIN_NAME}", "/mappings/hostMatch/hostPattern",
                domDoc);
        assertXpathEvaluatesTo("${MY_FULL_HOSTNAME}", "/mappings/hostMatch/hostPattern[2]",
                domDoc);
        assertXpathEvaluatesTo("${MY_HOSTNAME}", "/mappings/hostMatch/hostPattern[3]", domDoc);
        assertXpathEvaluatesTo("${MY_IP_ADDR}", "/mappings/hostMatch/hostPattern[4]", domDoc);
    }

    /**
     * This is mostly to demonstrate how complicated the XPatch expression becomes for a document
     * with a namespace
     * 
     * @param document
     */
    static void dumpXPaths(Document document) {
        VisitorSupport support = new VisitorSupport() {
            public void visit(Element node) {
                System.err.println(node.getPath());
            }
        };
        document.accept(support);
    }

    public void testGetHostMatch() throws Exception {
        MappingRules mappingRules = new MappingRules();
        Element hostMatch = mappingRules.getFirstHostMatch();
        assertSame(mappingRules.getDocument(), hostMatch.getDocument());
        assertEquals("/mappings/hostMatch", hostMatch.getPath());
    }

    public void testGenerate() throws Exception {
        UrlTransform voicemail = new UrlTransform();
        voicemail
                .setUrl("<sip:{digits}@{mediaserver};"
                        + "play={voicemail}/sipx-cgi/voicemail/mediaserver.cgi?action=deposit&mailbox={digits}>;q=0.1");

        UrlTransform voicemail2 = new UrlTransform();
        voicemail2
                .setUrl("<sip:{digits}@testserver;"
                        + "play={voicemail}/sipx-cgi/voicemail/mediaserver.cgi?action=deposit&mailbox={digits}>;q=0.001");

        MockControl control = MockControl.createControl(IDialingRule.class);
        IDialingRule rule = (IDialingRule) control.getMock();
        control.expectAndReturn(rule.getPatterns(), new String[] {
            "x."
        });
        control.expectAndReturn(rule.getPermissions(), Arrays.asList(new Permission[] {
            Permission.VOICEMAIL
        }));
        control.expectAndReturn(rule.getGateways(), Collections.EMPTY_LIST, 2);
        control.expectAndReturn(rule.getTransforms(), new Transform[] {
            voicemail, voicemail2
        });
        control.replay();

        MappingRules mappingRules = new MappingRules();
        mappingRules.generate(rule);

        Document document = mappingRules.getDocument();

        org.w3c.dom.Document domDoc = TransformTest.getDomDoc(document);

        assertXpathEvaluatesTo("x.", "/mappings/hostMatch/userMatch/userPattern", domDoc);
        assertXpathEvaluatesTo("Voicemail",
                "/mappings/hostMatch/userMatch/permissionMatch/permission", domDoc);
        assertXpathEvaluatesTo(voicemail.getUrl(),
                "/mappings/hostMatch/userMatch/permissionMatch/transform/url", domDoc);
        assertXpathEvaluatesTo(voicemail2.getUrl(),
                "/mappings/hostMatch/userMatch/permissionMatch/transform[2]/url", domDoc);

        control.verify();
    }

    public void testGenerateRuleWithGateways() throws Exception {
        List gateways = Arrays.asList(new Gateway[] {
            new Gateway()
        });

        MockControl control = MockControl.createControl(IDialingRule.class);
        IDialingRule rule = (IDialingRule) control.getMock();
        control.expectAndReturn(rule.getGateways(), gateways);
        control.replay();

        MappingRules mappingRules = new MappingRules();
        mappingRules.generate(rule);

        Document document = mappingRules.getDocument();
        org.w3c.dom.Document domDoc = TransformTest.getDomDoc(document);

        assertXpathNotExists("/mappings/hostMatch/userMatch/userPattern", domDoc);
        assertXpathNotExists("/mappings/hostMatch/userMatch/permissionMatch", domDoc);
        assertXpathExists("/mappings/hostMatch/hostPattern", domDoc);

        control.verify();
    }

    public void testInternalRules() throws Exception {
        int extension = 3;
        List rules = new ArrayList();
        rules.add(new MappingRule.Operator("100"));
        rules.add(new MappingRule.Voicemail("101"));
        rules.add(new MappingRule.VoicemailTransfer("2", extension));
        rules.add(new MappingRule.VoicemailFallback(extension));

        FlexibleDialPlan plan = new FlexibleDialPlan();
        plan.setRules(rules);
        ConfigGenerator generator = new ConfigGenerator();
        generator.generate(plan);

        String generatedXml = generator.getFileContent(ConfigFileType.MAPPING_RULES);

        InputStream referenceXmlStream = MappingRulesTest.class
                .getResourceAsStream("mappingrules.test.xml");
        XMLUnit.setIgnoreWhitespace(true);

        assertXMLEqual(new InputStreamReader(referenceXmlStream), new StringReader(generatedXml));
    }

}
