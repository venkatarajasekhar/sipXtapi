// 
// 
// Copyright (C) 2004 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
// 
// Copyright (C) 2004 Pingtel Corp.
// Licensed to SIPfoundry under a Contributor Agreement.
// 
// $$
//////////////////////////////////////////////////////////////////////////////

// SYSTEM INCLUDES
#include <assert.h>
#include <stdlib.h>

// APPLICATION INCLUDES
#include "os/OsBSem.h"
#include "os/OsFS.h"
#include "os/OsConfigDb.h"
#include "os/OsSysLog.h"
#include "utl/PluginHooks.h"
#include "net/HttpServer.h"
#include "net/Url.h"
#include "net/SipMessage.h"
#include "net/SipUserAgent.h"
#include "net/NameValueTokenizer.h"
#include "net/XmlRpcDispatch.h"
#include "sipdb/RegistrationDB.h"
#include "registry/SipRegistrar.h"
#include "registry/RegisterPlugin.h"
#include "registry/SipRedirectServer.h"
#include "SipRegistrarServer.h"
#include "RegistrarPeer.h"
#include "RegistrarPersist.h"
#include "RegistrarTest.h"
#include "RegistrarSync.h"
#include "RegistrarInitialSync.h"
#include "SyncRpc.h"

// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS

#define CONFIG_SETTING_LOG_LEVEL      "SIP_REGISTRAR_LOG_LEVEL"
#define CONFIG_SETTING_LOG_CONSOLE    "SIP_REGISTRAR_LOG_CONSOLE"
#define CONFIG_SETTING_LOG_DIR        "SIP_REGISTRAR_LOG_DIR"

const int REGISTRAR_DEFAULT_SIP_PORT  = 5070;
const int REGISTRAR_DEFAULT_SIPS_PORT = 5071;

const char* RegisterPlugin::Prefix  = "SIP_REGISTRAR";
const char* RegisterPlugin::Factory = "getRegisterPlugin";

// STATIC VARIABLE INITIALIZATIONS

const int SipRegistrar::SIP_REGISTRAR_DEFAULT_XMLRPC_PORT = 5077;

SipRegistrar* SipRegistrar::spInstance = NULL;
OsBSem SipRegistrar::sLock(OsBSem::Q_PRIORITY, OsBSem::FULL);


// Constructor
SipRegistrar::SipRegistrar(OsConfigDb* configDb) :
   OsServerTask("SipRegistrarMain", NULL, SIPUA_DEFAULT_SERVER_OSMSG_QUEUE_SIZE),
   mConfigDb(configDb),
   mRegistrationDB(RegistrationDB::getInstance()), // implicitly loads database
   mHttpServer(NULL),
   mXmlRpcDispatch(NULL),
   mReplicationConfigured(false),
   mSipUserAgent(NULL),
   mRedirectServer(NULL),
   mRedirectMsgQ(NULL),
   // Create the SipRegistrarServer object so it will be available immediately,
   // but don't start the associated thread until the registrar is operational.
   mRegistrarServer(new SipRegistrarServer(*this)),
   mRegistrarMsgQ(NULL),
   mRegistrarInitialSync(NULL),
   mRegistrarSync(NULL),
   mRegistrarTest(NULL),
   mRegistrarPersist(NULL)
{
   OsSysLog::add(FAC_SIP, PRI_DEBUG, "SipRegistrar::SipRegistrar constructed.");

   mHttpPort = mConfigDb->getPort("SIP_REGISTRAR_XMLRPC_PORT");
   if (PORT_NONE == mHttpPort)
   {
      OsSysLog::add(FAC_SIP, PRI_CRIT,
                    "SipRegistrar::SipRegistrar"
                    " SIP_REGISTRAR_XMLRPC_PORT == PORT_NONE :"
                    " peer synchronization disabled"
                    );
   }
   else // HTTP/RPC port is configured
   {
      if (PORT_DEFAULT == mHttpPort)
      {
         mHttpPort = SIP_REGISTRAR_DEFAULT_XMLRPC_PORT;
      }

      configurePeers();
   }

   // Some phones insist (incorrectly) on putting the proxy port number on urls;
   // we get it from the configuration so that we can ignore it.
   mProxyNormalPort = mConfigDb->getPort("SIP_REGISTRAR_PROXY_PORT");
   if (mProxyNormalPort == PORT_DEFAULT)
   {
      mProxyNormalPort = SIP_PORT;
   }
    
   // Domain Name
   mConfigDb->get("SIP_REGISTRAR_DOMAIN_NAME", mDefaultDomain);
   if ( mDefaultDomain.isNull() )
   {
      OsSocket::getHostIp(&mDefaultDomain);
      OsSysLog::add(FAC_SIP, PRI_CRIT,
                    "SIP_REGISTRAR_DOMAIN_NAME not configured using IP '%s'",
                    mDefaultDomain.data()
                    );
   }
   // get the url parts for the domain
   Url defaultDomainUrl(mDefaultDomain);
   mDefaultDomainPort = defaultDomainUrl.getHostPort();
   defaultDomainUrl.getHostAddress(mDefaultDomainHost);
   // make sure that the unspecified domain name is also valid
   addValidDomain(mDefaultDomainHost, mDefaultDomainPort);

   // Domain Aliases
   //   (other domain names that this registrar accepts as valid in the request URI)
   UtlString domainAliases;
   mConfigDb->get("SIP_REGISTRAR_DOMAIN_ALIASES", domainAliases);
    
   UtlString aliasString;
   int aliasIndex = 0;
   while(NameValueTokenizer::getSubField(domainAliases.data(), aliasIndex,
                                         ", \t", &aliasString))
   {
      Url aliasUrl(aliasString);
      UtlString hostAlias;
      aliasUrl.getHostAddress(hostAlias);
      int port = aliasUrl.getHostPort();

      addValidDomain(hostAlias,port);
      aliasIndex++;
   }
}

int SipRegistrar::run(void* pArg)
{
   startRpcServer();

   /*
    * If replication is configured,
    *   the following blocks until the state of each peer is known
    */
   startupPhase(); 
 
   operationalPhase();

   int taskResult = OsServerTask::run(pArg);

   return taskResult;
}

/// Launch all Startup Phase threads.
void SipRegistrar::startupPhase()
{
   OsSysLog::add(FAC_SIP, PRI_INFO, "SipRegistrar entering startup phase");

   // Create and start the persist thread, before making any changes
   // to the registration DB.
   mRegistrarPersist = new RegistrarPersist(*this);
   mRegistrarPersist->start();

   if (mReplicationConfigured)
   {
      // Create replication-related thread objects, but don't start them yet
      createReplicationThreads();

      // Begin the RegistrarInitialSync thread and then wait for it.
      mRegistrarInitialSync->start();
      yield();
      OsSysLog::add(FAC_SIP, PRI_DEBUG,
                    "SipRegistrar::startupPhase waiting for initialSyncThread"
                    );
      mRegistrarInitialSync->waitForCompletion();

      // The initial sync thread has no further value, to the ash heap of history it goes
      delete mRegistrarInitialSync;
      mRegistrarInitialSync = NULL;
   }
   else
   {
      OsSysLog::add(FAC_SIP, PRI_INFO,
                    "SipRegistrar::startupPhase no replication configured"
                    );
   }
}

/// Launch all Operational Phase threads.
void SipRegistrar::operationalPhase()
{
   OsSysLog::add(FAC_SIP, PRI_INFO, "SipRegistrar entering operational phase");

   // Start the sip stack
   int tcpPort = PORT_DEFAULT;
   int udpPort = PORT_DEFAULT;
   int tlsPort = PORT_DEFAULT;

   udpPort = mConfigDb->getPort("SIP_REGISTRAR_UDP_PORT");
   if (udpPort == PORT_DEFAULT)
   {
      udpPort = REGISTRAR_DEFAULT_SIP_PORT;
   }
  
   tcpPort = mConfigDb->getPort("SIP_REGISTRAR_TCP_PORT");
   if (tcpPort == PORT_DEFAULT)
   {
      tcpPort = REGISTRAR_DEFAULT_SIP_PORT;
   }

   tlsPort = mConfigDb->getPort("SIP_REGISTRAR_TLS_PORT");
   if (tlsPort == PORT_DEFAULT)
   {
      tlsPort = REGISTRAR_DEFAULT_SIPS_PORT;
   }

   mSipUserAgent = new SipUserAgent(tcpPort,
                                    udpPort,
                                    tlsPort,
                                    NULL,   // public IP address (not used)
                                    NULL,   // default user (not used)
                                    NULL,   // default SIP address (not used)
                                    NULL,   // outbound proxy
                                    NULL,   // directory server
                                    NULL,   // registry server
                                    NULL,   // auth scheme
                                    NULL,   // auth realm
                                    NULL,   // auth DB
                                    NULL,   // auth user IDs
                                    NULL,   // auth passwords
                                    NULL,   // nat ping URL
                                    0,      // nat ping frequency
                                    "PING", // nat ping method
                                    NULL,   // line mgr
                                    SIP_DEFAULT_RTT, // first resend timeout
                                    TRUE,   // default to UA transaction
                                    SIPUA_DEFAULT_SERVER_UDP_BUFFER_SIZE, // socket layer read buffer size
                                    SIPUA_DEFAULT_SERVER_OSMSG_QUEUE_SIZE, // OsServerTask message queue size
                                    FALSE,  // use next available port                                                  
                                    FALSE   // do not do UA message checks for METHOD, requires, etc...
                                    );

   if ( mSipUserAgent )
   {
      mSipUserAgent->addMessageObserver( *this->getMessageQueue(), NULL /* all methods */ );

      // the above causes us to actually receive all methods
      // the following sets what we send in Allow headers
      mSipUserAgent->allowMethod(SIP_REGISTER_METHOD);
      mSipUserAgent->allowMethod(SIP_SUBSCRIBE_METHOD);
      mSipUserAgent->allowMethod(SIP_OPTIONS_METHOD);
      mSipUserAgent->allowMethod(SIP_CANCEL_METHOD);

      mSipUserAgent->allowExtension("gruu"); // should be moved to gruu processor?

      mSipUserAgent->setUserAgentHeaderProperty("sipX/registry");
   }

   if (mReplicationConfigured)
   {
      // Start the test and sync threads.  (We ran the init sync thread earlier.)
      mRegistrarTest->start();
      mRegistrarSync->start();

      // Register the pushUpdates and reset methods.
      // (We registered the pullUpdates method earlier because it only needs DB access.)
      SyncRpcPushUpdates::registerSelf(*this);
      SyncRpcReset::registerSelf(*this);
   }

   mSipUserAgent->start();
   startRegistrarServer();
   startRedirectServer();

}

/// Get the XML-RPC dispatcher
XmlRpcDispatch* SipRegistrar::getXmlRpcDispatch()
{
   return mXmlRpcDispatch;
}

/// Get the RegistrarPersist thread object
RegistrarPersist* SipRegistrar::getRegistrarPersist()
{
   return mRegistrarPersist;
}

/// Get the RegistrarTest thread object
RegistrarTest* SipRegistrar::getRegistrarTest()
{
   return mRegistrarTest;
}

/// Get the RegistrarSync thread object
RegistrarSync* SipRegistrar::getRegistrarSync()
{
   return mRegistrarSync;
}

/// Return true if replication is configured, false otherwise
bool SipRegistrar::isReplicationConfigured()
{
   return mReplicationConfigured;
}

/// Get the RegistrationDB thread object
RegistrationDB* SipRegistrar::getRegistrationDB()
{
   return mRegistrationDB;
}    

/// Get the config DB
OsConfigDb* SipRegistrar::getConfigDB()
{
   return mConfigDb;
}

// Destructor
SipRegistrar::~SipRegistrar()
{
    // Wait for the owned servers to shutdown first
    // Deleting a server task is the only way of
    // waiting for shutdown to complete cleanly

    if ( mRedirectServer )
    {
        mRedirectServer->requestShutdown();
        delete mRedirectServer;
        mRedirectServer = NULL;
        mRedirectMsgQ = NULL;
    }

    if ( mRegistrarServer )
    {
       mRegistrarServer->requestShutdown();
       delete mRegistrarServer;
       mRegistrarServer = NULL;
       mRegistrarMsgQ = NULL;
    }

    if ( mHttpServer )
    {
       mHttpServer->requestShutdown();
       delete mHttpServer;
       mHttpServer = NULL;
    }

    if ( mRegistrarTest )
    {
       mRegistrarTest->requestShutdown();
       delete mRegistrarTest;
       mRegistrarTest = NULL;
    }

    if ( mRegistrarSync )
    {
       mRegistrarSync->requestShutdown();
       delete mRegistrarSync;
       mRegistrarSync = NULL;
    }

    if ( mRegistrarPersist )
    {
       mRegistrarPersist->requestShutdown();
       delete mRegistrarPersist;
       mRegistrarPersist = NULL;
    }

    if ( mSipUserAgent )
    {
       mSipUserAgent->shutdown();
       delete mSipUserAgent ;
       mSipUserAgent = NULL ;
    }

    mValidDomains.destroyAll();

    // release the registration database instance
    if (mRegistrationDB)
    {
       mRegistrationDB->releaseInstance();
       mRegistrationDB = NULL;
    }
}

/// Get the default domain name for this registrar 
const char* SipRegistrar::defaultDomain() const
{
   return mDefaultDomain.data();
}


/// Get the proxy port for the domain
int SipRegistrar::domainProxyPort() const
{
   return mProxyNormalPort;
}


UtlBoolean SipRegistrar::handleMessage( OsMsg& eventMessage )
{
    OsSysLog::add(FAC_SIP, PRI_DEBUG, "SipRegistrar::handleMessage()"
                  " Start processing SIP message") ;

    int msgType = eventMessage.getMsgType();
    int msgSubType = eventMessage.getMsgSubType();

    if ( (msgType == OsMsg::PHONE_APP) &&
         (msgSubType == SipMessage::NET_SIP_MESSAGE) )
    {
        const SipMessage* message =
           ((SipMessageEvent&)eventMessage).getMessage();
        UtlString callId;
        if ( message )
        {
            message->getCallIdField(&callId);
            UtlString method;
            message->getRequestMethod(&method);

            if ( !message->isResponse() ) // is a request ?
            {
                if ( method.compareTo(SIP_REGISTER_METHOD) == 0 )
                {
                    //send to Register Thread
                    sendToRegistrarServer(eventMessage);
                }
                else
                {
                    //send to redirect thread
                    sendToRedirectServer(eventMessage);
                }
            }
            else
            {
               // responses are ignored.
            }
        }
        else
        {
           OsSysLog::add(FAC_SIP, PRI_DEBUG,
                         "SipRegistrar::handleMessage no message."
                         ) ;
        }
    }
    else
    {
       OsSysLog::add(FAC_SIP, PRI_DEBUG,
                     "SipRegistrar::handleMessage unexpected message type %d/%d",
                     msgType, msgSubType
                     ) ;
    }
    
    return(TRUE);
}

SipRegistrar*
SipRegistrar::getInstance(OsConfigDb* configDb)
{
    OsLock singletonLock(sLock);

    if ( spInstance == NULL )
    {
       OsSysLog::add(FAC_SIP, PRI_DEBUG, "SipRegistrar::getInstance(%p)",
                     configDb);
       
       spInstance = new SipRegistrar(configDb);
    }

    return spInstance;
}

SipRegistrarServer&
SipRegistrar::getRegistrarServer()
{
   // The SipRegistrarServer is created in the SipRegistrar constructor, so
   // mRegistrarServer should never be null
   assert(mRegistrarServer);

   return *mRegistrarServer;
}

/// Read peer configuration and initialize peer state
void SipRegistrar::configurePeers()
{
   // in case we can ever do this on the fly, clear out any old peer configuration
   mReplicationConfigured = false;
   mPeers.destroyAll();
   mPrimaryName.remove(0);

   UtlString peersMsg;
   
   mConfigDb->get("SIP_REGISTRAR_NAME", mPrimaryName);

   if (! mPrimaryName.isNull())
   {
      UtlString peerNames;
      mConfigDb->get("SIP_REGISTRAR_SYNC_WITH", peerNames);

      if (!peerNames.isNull())
      {
         UtlString peerName;
         
         for (int peerIndex = 0;
              NameValueTokenizer::getSubField(peerNames.data(), peerIndex, ", \t", &peerName);
              peerIndex++
              )
         {
            if ( peerName != mPrimaryName )
            {
               RegistrarPeer* thisPeer = new RegistrarPeer(this, peerName, mHttpPort);
               assert(thisPeer);

               mPeers.append(thisPeer);
               if (!peersMsg.isNull())
               {
                  peersMsg.append(", ");
               }
               peersMsg.append(peerName);
            }
         }

         if (mPeers.isEmpty())
         {
            OsSysLog::add(FAC_SIP, PRI_WARNING,
                          "SipRegistrar::configurePeers - no peers configured"
                          );
         }
         else
         {
            OsSysLog::add(FAC_SIP, PRI_NOTICE,
                          "SipRegistrar::configurePeers: %s", peersMsg.data()
                          );
            mReplicationConfigured = true;
         }
      }
   }
   else
   {
      OsSysLog::add(FAC_SIP, PRI_INFO, "SipRegistrar::configurePeers "
                    "SIP_REGISTRAR_NAME not set - replication disbled"
                    );
   }
}

    
/// If replication is configured, then name of this registrar as primary
const UtlString& SipRegistrar::primaryName() const
{
   return mPrimaryName;
}


/// Server for XML-RPC requests
void SipRegistrar::startRpcServer()
{
   // Begins operation of the HTTP/RPC service
   // sets mHttpServer and mXmlRpcDispatcher

   if (mReplicationConfigured)
   {
      // Initialize mHttpServer and mXmlRpcDispatch
      mXmlRpcDispatch = new XmlRpcDispatch(mHttpPort, true /* use https */);
      mHttpServer = mXmlRpcDispatch->getHttpServer();
   }
}

/// Get an iterator over all peers.
UtlSListIterator* SipRegistrar::getPeers()
{
   return (  ( ! mReplicationConfigured || mPeers.isEmpty() )
           ? NULL : new UtlSListIterator(mPeers));
}

/// Get peer state object by name.
RegistrarPeer* SipRegistrar::getPeer(const UtlString& peerName)
{
   return (  mReplicationConfigured
           ? dynamic_cast<RegistrarPeer*>(mPeers.find(&peerName))
           : NULL
           );
}

void
SipRegistrar::startRedirectServer()
{
   mRedirectServer = new SipRedirectServer(mConfigDb, mSipUserAgent);
   mRedirectMsgQ = mRedirectServer->getMessageQueue();
   mRedirectServer->start();
}

void
SipRegistrar::startRegistrarServer()
{
    mRegistrarMsgQ = mRegistrarServer->getMessageQueue();
    mRegistrarServer->initialize(mConfigDb, mSipUserAgent);
    mRegistrarServer->start();
}

void
SipRegistrar::sendToRedirectServer(OsMsg& eventMessage)
{
    if ( mRedirectMsgQ )
    {
        mRedirectMsgQ->send(eventMessage);
    }
    else
    {
       OsSysLog::add(FAC_SIP, PRI_CRIT, "sendToRedirectServer - queue not initialized.");
    }
}

void
SipRegistrar::sendToRegistrarServer(OsMsg& eventMessage)
{
    if ( mRegistrarMsgQ )
    {
        mRegistrarMsgQ->send(eventMessage);
    }
    else
    {
       OsSysLog::add(FAC_SIP, PRI_CRIT, "sendToRegistrarServer - queue not initialized.");
    }
}

/// Create replication-related thread objects, but don't start them yet
void SipRegistrar::createReplicationThreads()
{
   mRegistrarInitialSync = new RegistrarInitialSync(*this);
   mRegistrarSync = new RegistrarSync(*this);
   mRegistrarTest = new RegistrarTest(*this);
}

bool
SipRegistrar::isValidDomain(const Url& uri) const
{
   bool isValid = false;

   UtlString domain;
   uri.getHostAddress(domain);
   domain.toLower();

   int port = uri.getHostPort();
   if (port == PORT_NONE)
   {
      port = SIP_PORT;
   }
   char portNum[15];
   sprintf(portNum,"%d",port);

   domain.append(":");
   domain.append(portNum);

   if ( mValidDomains.contains(&domain) )
   {
      isValid = true;
      OsSysLog::add(FAC_AUTH, PRI_DEBUG,
                    "SipRegistrar::isValidDomain(%s) VALID",
                    domain.data()) ;
   }
   return isValid;
}

void
SipRegistrar::addValidDomain(const UtlString& host, int port)
{
   UtlString* valid = new UtlString(host);
   valid->toLower();

   char explicitPort[20];
   sprintf(explicitPort,":%d", PORT_NONE==port ? SIP_PORT : port );
   valid->append(explicitPort);
   
   OsSysLog::add(FAC_AUTH, PRI_DEBUG, "SipRegistrar::addValidDomain(%s)",valid->data()) ;

   mValidDomains.insert(valid);
}
