<%@ page import="   javax.ejb.*,
                    javax.naming.*,
                    java.io.*,
                    javax.rmi.PortableRemoteObject,java.util.*,
                    com.pingtel.pds.pgs.organization.*,
                    com.pingtel.pds.pgs.user.*,
                    com.pingtel.pds.pgs.phone.*,
                    com.pingtel.pds.common.PathLocatorUtil,
                    com.pingtel.pds.pgs.common.*,
                    java.rmi.RemoteException,
                    org.apache.log4j.*,
                    javax.servlet.jsp.JspException,
                    com.pingtel.pds.common.EJBHomeFactory"
%>


<%
    // set user information in the session
    session.setAttribute("xsltBaseDir",
            PathLocatorUtil.getInstance().getPath(
                    PathLocatorUtil.XSLT_FOLDER, PathLocatorUtil.PGS ));

    // find out what organization the user belongs to.

        UserHome userHome = (UserHome)
                EJBHomeFactory.getInstance().getHomeInterface(
                        UserHome.class, "User");

        OrganizationHome organizationHome = (OrganizationHome)
                EJBHomeFactory.getInstance().getHomeInterface(
                        OrganizationHome.class, "Organization");

        DeviceHome deviceHome = (DeviceHome)
                EJBHomeFactory.getInstance().getHomeInterface(
                        DeviceHome.class, "Device");

        // find the stereotype of the organization
        Organization org = organizationHome.findByPrimaryKey(new Integer(1));

        // we tollerate users loggin in with a prefix of 'sip:' and/or
        // their organization's domain as a suffix.  e.g.
        // sip:you@it.com.   we are only interested in the user id portion,
        // in this case 'you'.
        User user = null;

        String trimmedUserName =
                new String (request.getUserPrincipal().toString());

        if (trimmedUserName.toLowerCase().startsWith("sip:")){
            trimmedUserName = trimmedUserName.substring(4);
        }

        if (trimmedUserName.indexOf('@') != -1){
           String enteredDnsDomain = 
                        trimmedUserName.substring(
                                trimmedUserName.indexOf('@') +1);

                // check that the DNS Domain/host part of the user ID is same
                // as the one configured for the sipxchange installation.
                if ( !enteredDnsDomain.equals( org.getDNSDomain() ) ) {
                    // redirect back to login
                    // for now just send back to login
                    response.sendRedirect( "../../login/login.html" );
                }

                trimmedUserName =
                        trimmedUserName.substring(0,
                                trimmedUserName.indexOf('@'));
        }

        Collection users = userHome.findByDisplayIDOrExtension(trimmedUserName);

        // This will only return one user
        for (Iterator iter = users.iterator(); iter.hasNext(); ) {
            user = (User) iter.next();
            session.setAttribute( "userid", user.getID() );
        }

        // find out what devices this user has associated with them
        Collection devicesAssigned = deviceHome.findByUserID(user.getID());
        Iterator it = devicesAssigned.iterator();
        Device deviceProp;
        String userDevices = "";
        while ( it.hasNext() ){
            deviceProp = ( Device ) it.next();
            userDevices = userDevices + deviceProp.getManufaturerName();
        }

        //set userDevices results in the session
        session.setAttribute("userdevices", userDevices);

        // set organization id in the session
        session.setAttribute("orgid", user.getOrganizationID().toString());

        // set organization stereotype in the session
        String stereotype = String.valueOf( org.getStereotype() );
        session.setAttribute("stereotype", stereotype);
        session.setAttribute("orgname", org.getName());

        // Set the "user" value to be the 'real' user id
        session.setAttribute("user", user.getDisplayID());

        UserAdvocateHome userAdvocateHome = (UserAdvocateHome)
                EJBHomeFactory.getInstance().getHomeInterface(
                        UserAdvocateHome.class, "UserAdvocate");

        UserAdvocate advocate = userAdvocateHome.create();

        Collection roles = advocate.getSecurityRoles( user.getID() );
        if (!roles.contains("SUPER") && !roles.contains("ADMIN")){
            session.setAttribute( "userpwd", user.getPassword () );
            response.sendRedirect( "../sipxchange/index.jsp" );
        }
        else {
            //redirect to frameset.html
            response.sendRedirect( "../frameset.html" );
        }

%>
