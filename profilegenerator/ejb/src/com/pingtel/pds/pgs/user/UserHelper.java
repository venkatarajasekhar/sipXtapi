/*
 * 
 * 
 * Copyright (C) 2004 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2004 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $$
 */

package com.pingtel.pds.pgs.user;

import java.rmi.RemoteException;
import java.util.Collection;
import java.util.Iterator;

import org.jdom.CDATA;
import org.jdom.Element;

import com.pingtel.pds.common.MD5Encoder;
import com.pingtel.pds.pgs.organization.OrganizationBusiness;
import com.pingtel.pds.pgs.phone.DeviceHelper;
import com.pingtel.pds.pgs.profile.RefPropertyBusiness;

public class UserHelper {
    private UserBusiness m_user;

    public UserHelper(UserBusiness user) {
        m_user = user;
    }

    String calculatePrimaryLineUrl(OrganizationBusiness org) throws RemoteException {
        String firstName = m_user.getFirstName();
        String lastName = m_user.getLastName();
        String displayID = m_user.getDisplayID();

        StringBuffer xmlContent = new StringBuffer();

        boolean needsWrapping = false;

        if (firstName != null && firstName.length() > 0) {
            xmlContent.append(firstName);
            needsWrapping = true;
        }

        if (lastName != null && lastName.length() > 0) {
            if (needsWrapping) {
                xmlContent.append(" ");
            }

            xmlContent.append(lastName);
            needsWrapping = true;
        }

        if (needsWrapping) {
            xmlContent.append("<");
        }

        xmlContent.append("sip:");
        xmlContent.append(displayID);
        xmlContent.append("@" + org.getDNSDomain());

        if (needsWrapping) {
            xmlContent.append(">");
        }

        return xmlContent.toString();
    }

    String createInitialLine(OrganizationBusiness org, RefPropertyBusiness rpXp,
            RefPropertyBusiness rpXp_2028, RefPropertyBusiness rpCs, String password)
            throws RemoteException {
        StringBuffer xmlContent = new StringBuffer();

        xmlContent.append("<PROFILE>");
        xmlContent.append(createPingtelPrimaryLineMarkup(org, rpXp, rpXp_2028, password));
        xmlContent.append(create79xxPrimaryLineMarkup(org, rpCs, password));
        xmlContent.append("</PROFILE>");

        return xmlContent.toString();
    }

    private String createPingtelPrimaryLineMarkup(OrganizationBusiness org,
            RefPropertyBusiness rp, RefPropertyBusiness rpXp_2028, String password)
            throws RemoteException {
        StringBuffer xmlContent = new StringBuffer();

        String primaryLineURL = calculatePrimaryLineUrl(org);
        String primaryLineCredentialPasstoken = digestPassword(org, password);
        String userAuthID = m_user.getDisplayID();

        xmlContent.append("<PRIMARY_LINE ref_property_id=\"" + rp.getID() + "\">");
        xmlContent.append("<PRIMARY_LINE>");
        xmlContent.append("<ALLOW_FORWARDING>" + DeviceHelper.CDATAIt("ENABLE")
                + "</ALLOW_FORWARDING>");
        xmlContent
                .append("<REGISTRATION>" + DeviceHelper.CDATAIt("REGISTER") + "</REGISTRATION>");
        xmlContent.append("<URL>" + DeviceHelper.CDATAIt(primaryLineURL) + "</URL>");
        xmlContent.append("<CREDENTIAL autogenerated=\"true\">");
        xmlContent.append("<REALM>" + DeviceHelper.CDATAIt(org.getAuthenticationRealm())
                + "</REALM>");
        xmlContent.append("<USERID>" + DeviceHelper.CDATAIt(userAuthID) + "</USERID>");
        xmlContent.append("<PASSTOKEN>" + DeviceHelper.CDATAIt(primaryLineCredentialPasstoken)
                + "</PASSTOKEN>");

        xmlContent.append("</CREDENTIAL>");
        xmlContent.append("</PRIMARY_LINE>");
        xmlContent.append("</PRIMARY_LINE>");

        xmlContent.append("<USER_DEFAULT_OUTBOUND_LINE ref_property_id=\"" + rpXp_2028.getID()
                + "\">");
        xmlContent.append("<USER_DEFAULT_OUTBOUND_LINE>" + DeviceHelper.CDATAIt(primaryLineURL)
                + "</USER_DEFAULT_OUTBOUND_LINE>");
        xmlContent.append("</USER_DEFAULT_OUTBOUND_LINE>");

        return xmlContent.toString();
    }

    private String create79xxPrimaryLineMarkup(OrganizationBusiness org, RefPropertyBusiness rp,
            String password) throws RemoteException {
        StringBuffer xmlContent = new StringBuffer();
        String displayID = m_user.getDisplayID();
        String displayName = new String("\"" + m_user.getFirstName() + " " + m_user.getLastName()
                + "\"").trim();

        xmlContent.append("<line1 ref_property_id=\"" + rp.getID() + "\">");
        xmlContent.append("<container>");
        xmlContent.append("<line1_password>" + DeviceHelper.CDATAIt(password)
                + "</line1_password>");
        xmlContent.append("<line1_displayname>" + DeviceHelper.CDATAIt(displayName)
                + "</line1_displayname>");
        xmlContent.append("<line1_authname>" + DeviceHelper.CDATAIt(displayID)
                + "</line1_authname>");
        xmlContent.append("<line1_name>" + DeviceHelper.CDATAIt(displayID) + "</line1_name>");
        xmlContent.append("<line1_shortname>" + DeviceHelper.CDATAIt(displayID + "_line")
                + "</line1_shortname>");
        xmlContent.append("</container>");
        xmlContent.append("</line1>");

        return xmlContent.toString();
    }

    /**
     * Create a digest of user password.
     * 
     * @param org organization to which user belongs
     * @param clearPassword password in cleartext
     * @return digested password
     * @throws RemoteException
     */
    public String digestPassword(OrganizationBusiness org, String clearPassword)
            throws RemoteException {
        return digestPassword(m_user.getDisplayID(), org, clearPassword);
    }

    /**
     * Create a digest of user password - username @ dns.domain is used Use only
     * for compatibility purposes now. Does not work if relm is different from
     * DNS Domain
     * @param org organization to which user belongs
     * @param clearPassword password in cleartext
     * @return digested password
     * @throws RemoteException
     * 
     */
    public String digestPasswordQualifiedUsername(OrganizationBusiness org, String clearPassword)
            throws RemoteException {
        String displayID = m_user.getDisplayID();
        String dnsDomain = org.getDNSDomain();
        String realm = org.getAuthenticationRealm();
        return MD5Encoder.digestPassword(displayID, dnsDomain, realm, clearPassword);
    }

    /**
     * Create a digest of user password. Use this version when user bean has not
     * been created yet.
     * 
     * @param displayID
     * @param org organization to which user belongs
     * @param clearPassword password in cleartext
     * @return digested password
     * @throws RemoteException
     */
    public static String digestPassword(String displayID, OrganizationBusiness org,
            String clearPassword) throws RemoteException {
        String realm = org.getAuthenticationRealm();
        return MD5Encoder.digestPassword(displayID, realm, clearPassword);
    }

    /**
     * Recalculates credentials for primary line (after password or domain name
     * change)
     * 
     * @param setting JDOM element - parent of "PRIMARY_LINE"
     * @param realm current authentication realm
     * @param passtoken can be null if we are not supposed to calculate it again
     * @throws RemoteException
     */
    void fixUserPrimaryLine(Element setting, OrganizationBusiness org, String realm,
            boolean fixCredential, String passtoken) throws RemoteException {
        Element holder = setting.getChild("PRIMARY_LINE");
        Element url = holder.getChild("URL");
        url.removeContent();
        url.addContent(new CDATA(calculatePrimaryLineUrl(org)));
        if (!fixCredential) {
            return;
        }
        Collection credentials = holder.getChildren("CREDENTIAL");
        for (Iterator iCred = credentials.iterator(); iCred.hasNext();) {
            Element credential = (Element) iCred.next();

            String childRealm = credential.getChild("REALM").getText();
            if (!childRealm.equals(realm)) {
                // only consider credentials for our realm
                continue;
            }
            String autogenerated = credential.getAttributeValue("autogenerated");
            if (autogenerated == null) {
                credential.setAttribute("autogenerated", "true");
            }
            Element passtokenElement = credential.getChild("PASSTOKEN");
            passtokenElement.removeContent();
            passtokenElement.addContent(new CDATA(passtoken));
        }
    }
}
