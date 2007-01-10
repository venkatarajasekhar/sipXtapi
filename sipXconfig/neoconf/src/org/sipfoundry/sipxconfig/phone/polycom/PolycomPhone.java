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
package org.sipfoundry.sipxconfig.phone.polycom;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.Reader;
import java.io.StringReader;
import java.io.StringWriter;
import java.io.Writer;
import java.util.Collection;

import org.apache.commons.io.IOUtils;
import org.dom4j.Document;
import org.dom4j.DocumentException;
import org.dom4j.io.OutputFormat;
import org.dom4j.io.SAXReader;
import org.dom4j.io.XMLWriter;
import org.sipfoundry.sipxconfig.device.DeviceVersion;
import org.sipfoundry.sipxconfig.device.VelocityProfileGenerator;
import org.sipfoundry.sipxconfig.phone.Line;
import org.sipfoundry.sipxconfig.phone.LineInfo;
import org.sipfoundry.sipxconfig.phone.Phone;
import org.sipfoundry.sipxconfig.phonebook.PhonebookEntry;
import org.sipfoundry.sipxconfig.speeddial.SpeedDial;

/**
 * Support for Polycom 300, 400, and 500 series phones and model 3000 conference phone
 */
public class PolycomPhone extends Phone {
    public static final String BEAN_ID = "polycom";
    public static final String CALL = "call";
    static final String REGISTRATION_PATH = "reg/server/1/address";
    static final String REGISTRATION_PORT_PATH = "reg/server/1/port";
    static final String CONTACT_MODE = "contact";
    static final String DISPLAY_NAME_PATH = "reg/displayName";
    static final String PASSWORD_PATH = "reg/auth.password";
    static final String USER_ID_PATH = "reg/address";
    static final String AUTHORIZATION_ID_PATH = "reg/auth.userId";
    private String m_phoneConfigDir = "polycom/mac-address.d";
    private String m_phoneTemplate = m_phoneConfigDir + "/phone.cfg.vm";
    private String m_sipTemplate = m_phoneConfigDir + "/sip-%s.cfg.vm";
    private String m_coreTemplate = m_phoneConfigDir + "/ipmid.cfg.vm";
    private String m_directoryTemplate = "polycom/mac-address-directory.xml.vm";
    private String m_applicationTemplate = "polycom/mac-address.cfg.vm";

    public PolycomPhone() {
        super(new PolycomModel());
        init();
    }

    private void init() {
        setDeviceVersion(PolycomModel.VER_2_0);
    }

    public String getDefaultVersionId() {
        DeviceVersion version = getDeviceVersion();
        return version != null ? version.getVersionId() : null;
    }

    /**
     * Default firmware version for polycom phones. Default is 1.6 right now
     * 
     * @param defaultVersionId 1.6 or 2.0
     */
    public void setDefaultVersionId(String defaultVersionId) {
        setDeviceVersion(DeviceVersion.getDeviceVersion(PolycomPhone.BEAN_ID + defaultVersionId));
    }

    @Override
    public void initialize() {
        SpeedDial speedDial = getPhoneContext().getSpeedDial(this);
        PolycomPhoneDefaults phoneDefaults = new PolycomPhoneDefaults(getPhoneContext()
                .getPhoneDefaults(), speedDial);
        addDefaultBeanSettingHandler(phoneDefaults);

        PolycomIntercomDefaults intercomDefaults = new PolycomIntercomDefaults(this);
        addDefaultBeanSettingHandler(intercomDefaults);
    }

    @Override
    public void initializeLine(Line line) {
        PolycomLineDefaults lineDefaults = new PolycomLineDefaults(getPhoneContext()
                .getPhoneDefaults(), line);
        line.addDefaultBeanSettingHandler(lineDefaults);
    }

    public String getPhoneTemplate() {
        return m_phoneTemplate;
    }

    public void setPhoneTemplate(String phoneTemplate) {
        m_phoneTemplate = phoneTemplate;
    }

    public String getCoreTemplate() {
        return m_coreTemplate;
    }

    public void setCoreTemplate(String coreTemplate) {
        m_coreTemplate = coreTemplate;
    }

    public String getApplicationTemplate() {
        return m_applicationTemplate;
    }

    public void setApplicationTemplate(String applicationTemplate) {
        m_applicationTemplate = applicationTemplate;
    }

    public String getSipTemplate() {
        return String.format(m_sipTemplate, getDeviceVersion().getVersionId());
    }

    public void setSipTemplate(String sipTemplate) {
        m_sipTemplate = sipTemplate;
    }

    public void generateProfiles() {
        ApplicationConfiguration app = new ApplicationConfiguration(this);
        generateProfile(app, getApplicationTemplate(), app.getAppFilename());

        SipConfiguration sip = new SipConfiguration(this);
        generateProfile(sip, getSipTemplate(), app.getSipFilename());

        PhoneConfiguration phone = new PhoneConfiguration(this);
        generateProfile(phone, getPhoneTemplate(), app.getPhoneFilename());

        app.deleteStaleDirectories();

        Collection<PhonebookEntry> entries = getPhoneContext().getPhonebookEntries(this);
        SpeedDial speedDial = getPhoneContext().getSpeedDial(this);
        DirectoryConfiguration dir = new DirectoryConfiguration(this, entries, speedDial);
        generateProfile(dir, getDirectoryTemplate(), app.getDirectoryFilename());
    }

    public void removeProfiles() {
        ApplicationConfiguration app = new ApplicationConfiguration(this);
        File cfgFile = new File(getTftpRoot(), app.getAppFilename());
        File phonebookFile = new File(getTftpRoot(), app.getDirectoryFilename());
        // new to call this function to generate stale directories list
        app.getDirectory();
        // this will remove all old directories
        app.deleteStaleDirectories();

        File[] files = {
            cfgFile, 
            phonebookFile
        };

        // and this will remove new ones
        VelocityProfileGenerator.removeProfileFiles(files);
    }

    private void generateProfile(VelocityProfileGenerator cfg, String template, String outputFile) {
        FileWriter out = null;
        try {
            File f = new File(getTftpRoot(), outputFile);
            VelocityProfileGenerator.makeParentDirectory(f);
            Writer unformatted = new StringWriter();
            generateProfile(cfg, template, unformatted);
            out = new FileWriter(f);
            format(new StringReader(unformatted.toString()), out);
        } catch (IOException ioe) {
            throw new RuntimeException("Could not generate profile " + outputFile
                    + " from template " + template, ioe);
        } finally {
            if (out != null) {
                IOUtils.closeQuietly(out);
            }
        }
    }

    /**
     * Polycom 430 1.6.5 would not read files w/being formatted first. Unclear why.
     */
    static void format(Reader in, Writer wtr) {
        SAXReader xmlReader = new SAXReader();
        Document doc;
        try {
            doc = xmlReader.read(in);
        } catch (DocumentException e1) {
            throw new RuntimeException(e1);
        }
        OutputFormat pretty = OutputFormat.createPrettyPrint();
        XMLWriter xml = new XMLWriter(wtr, pretty);
        try {
            xml.write(doc);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    @Override
    protected void setLineInfo(Line line, LineInfo externalLine) {
        line.setSettingValue(DISPLAY_NAME_PATH, externalLine.getDisplayName());
        line.setSettingValue(USER_ID_PATH, externalLine.getUserId());
        line.setSettingValue(PASSWORD_PATH, externalLine.getPassword());

        // Both userId and authId are required, see XCF-914
        line.setSettingValue(AUTHORIZATION_ID_PATH, externalLine.getUserId());

        line.setSettingValue(REGISTRATION_PATH, externalLine.getRegistrationServer());
        line.setSettingValue(REGISTRATION_PORT_PATH, externalLine.getRegistrationServerPort());
    }

    @Override
    protected LineInfo getLineInfo(Line line) {
        LineInfo lineInfo = new LineInfo();
        lineInfo.setUserId(line.getSettingValue(USER_ID_PATH));
        lineInfo.setDisplayName(line.getSettingValue(DISPLAY_NAME_PATH));
        lineInfo.setPassword(line.getSettingValue(PASSWORD_PATH));
        lineInfo.setRegistrationServer(line.getSettingValue(REGISTRATION_PATH));
        lineInfo.setRegistrationServerPort(line.getSettingValue(REGISTRATION_PORT_PATH));
        return lineInfo;
    }

    public void restart() {
        sendCheckSyncToFirstLine();
    }

    public String getDirectoryTemplate() {
        return m_directoryTemplate;
    }

    public void setDirectoryTemplate(String directoryTemplate) {
        m_directoryTemplate = directoryTemplate;
    }
}
