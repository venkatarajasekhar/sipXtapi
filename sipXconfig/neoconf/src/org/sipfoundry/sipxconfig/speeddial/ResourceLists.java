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
package org.sipfoundry.sipxconfig.speeddial;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;
import org.sipfoundry.sipxconfig.admin.dialplan.config.ConfigFileType;
import org.sipfoundry.sipxconfig.admin.dialplan.config.XmlFile;
import org.sipfoundry.sipxconfig.common.CoreContext;
import org.sipfoundry.sipxconfig.common.User;

public class ResourceLists extends XmlFile {
    private static final String NAMESPACE = "http://www.sipfoundry.org/sipX/schema/xml/resource-lists-00-00";

    private Document m_document;

    private CoreContext m_coreContext;

    public Document getDocument() {
        return m_document;
    }

    public void generate(SpeedDialManager speedDialManager) {
        m_document = FACTORY.createDocument();
        Element lists = m_document.addElement("lists", NAMESPACE);
        List<User> users = m_coreContext.loadUsers();
        for (User user : users) {
            SpeedDial speedDial = speedDialManager.getSpeedDialForUserId(user.getId(), false);
            // ignore disabled orbits
            if (speedDial == null) {
                continue;
            }
            List<Button> buttons = speedDial.getButtons();
            Element list = null;
            for (Button button : buttons) {
                if (!button.isBlf()) {
                    continue;
                }
                if (list == null) {
                    list = createListForUser(lists, speedDial);
                }
                createResourceForUser(list, button);
            }
        }
    }

    private Element createResourceForUser(Element list, Button button) {
        Element resource = list.addElement("resource");
        resource.addAttribute("uri", button.getUri(m_coreContext.getDomainName()));
        addNameElement(resource, button.getLabel());
        return resource;
    }

    private void addNameElement(Element parent, String name) {
        parent.addElement("name").setText(name);
    }

    private Element createListForUser(Element lists, SpeedDial speedDial) {
        Element list = lists.addElement("list");
        list.addAttribute("user", speedDial.getResourceListId());
        addNameElement(list, speedDial.getResourceListName());
        return list;
    }

    public void setCoreContext(CoreContext coreContext) {
        m_coreContext = coreContext;
    }

    public ConfigFileType getType() {
        return ConfigFileType.RESOURCE_LISTS;
    }
}
