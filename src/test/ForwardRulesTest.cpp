// $Id: //depot/OPENDEV/sipXproxy/src/test/ForwardRulesTest.cpp#1 $
//
// Copyright (C) 2004 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// Copyright (C) 2004 Pingtel Corp.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// $$
//////////////////////////////////////////////////////////////////////////////

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>

/**
 * Unittest for UrlMapping
 *
 */
#include "os/OsDefs.h"
#include "net/Url.h"
#include "net/SipMessage.h"
#include "sipforkingproxy/ForwardRules.h"

#define VM "VoIcEmAiL"
#define MS "MeDiAsErVeR"
#define LH "LoCaLhOsT"

class ForwardRulesTest : public CppUnit::TestCase
{
      CPPUNIT_TEST_SUITE(ForwardRulesTest);
      CPPUNIT_TEST(testSimpleMapMatchMethod);
      CPPUNIT_TEST(testSimpleMapMatchFieldExact);
      CPPUNIT_TEST(testSimpleMapMatchFieldLong);
      CPPUNIT_TEST(testSimpleMapNonMatchFieldPrefix);
      CPPUNIT_TEST(testSimpleMapNoField);
      CPPUNIT_TEST_SUITE_END();


   public:
      void testSimpleMapDefault()
      {
         ForwardRules theRules;
         OsString     theRoute;
         OsString     mappingType;
         OsString     rulesFile(TEST_DATA_DIR "rulesdata/simple.xml");
            
         CPPUNIT_ASSERT( theRules.loadMappings(rulesFile, MS, VM, LH )
                        == OS_SUCCESS
                        );

         CPPUNIT_ASSERT( theRules.getRoute(Url("sip:OTHER_DOMAIN_NAME"),
                                           SipMessage("UNKNOWN sip:SIPXCHANGE_DOMAIN_NAME SIP/2.0\r\n"
                                                      "\r\n"
                                                      ),
                                           theRoute,
                                           mappingType
                                           )
                        != OS_SUCCESS
                        );
         //CPPUNIT_ASSERT( strcmp(theRoute.data(),"REGISTRAR_SERVER_DEFAULT") == 0 );
         //CPPUNIT_ASSERT( strcmp(mappingType.data(),"local") == 0 );
      }

      void testSimpleMapMatchMethod()
      {
         ForwardRules theRules;
         OsString     theRoute;
         OsString     mappingType;
         OsString     rulesFile(TEST_DATA_DIR "rulesdata/simple.xml");
            
         CPPUNIT_ASSERT( theRules.loadMappings(rulesFile, MS, VM, LH )
                        == OS_SUCCESS
                        );

         CPPUNIT_ASSERT( theRules.getRoute(Url("sip:SIPXCHANGE_DOMAIN_NAME"),
                                           SipMessage("REGISTER sip:SIPXCHANGE_DOMAIN_NAME SIP/2.0\r\n"
                                                      "\r\n"
                                                      ),
                                           theRoute,
                                           mappingType
                                           )
                        == OS_SUCCESS
                        );
         CPPUNIT_ASSERT( strcmp(theRoute.data(),"REGISTRAR_SERVER_DEFAULT") == 0 );
         CPPUNIT_ASSERT( strcmp(mappingType.data(),"local") == 0 );
      }

      void testSimpleMapMatchFieldExact()
      {
         ForwardRules theRules;
         OsString     theRoute;
         OsString     mappingType;
         OsString     rulesFile(TEST_DATA_DIR "rulesdata/simple.xml");

         CPPUNIT_ASSERT( theRules.loadMappings(rulesFile, MS, VM, LH )
                        == OS_SUCCESS
                        );

         CPPUNIT_ASSERT( theRules.getRoute(Url("sip:SIPXCHANGE_DOMAIN_NAME"),
                                           SipMessage("SUBSCRIBE sip:SIPXCHANGE_DOMAIN_NAME SIP/2.0\r\n"
                                                      "Event: sip-config\r\n"
                                                      "\r\n"
                                                      ),
                                           theRoute,
                                           mappingType
                                           )
                        == OS_SUCCESS
                        );
         CPPUNIT_ASSERT( strcmp(theRoute.data(),"CONFIG_SERVER_SUBSCRIBE") == 0 );
         CPPUNIT_ASSERT( strcmp(mappingType.data(),"local") == 0 );
      }

      void testSimpleMapMatchFieldLong()
      {
         ForwardRules theRules;
         OsString     theRoute;
         OsString     mappingType;
         OsString     rulesFile(TEST_DATA_DIR "rulesdata/simple.xml");

         CPPUNIT_ASSERT( theRules.loadMappings(rulesFile, MS, VM, LH )
                        == OS_SUCCESS
                        );

         CPPUNIT_ASSERT( theRules.getRoute(Url("sip:SIPXCHANGE_DOMAIN_NAME"),
                                           SipMessage("SUBSCRIBE sip:SIPXCHANGE_DOMAIN_NAME SIP/2.0\r\n"
                                                      "Event: sip-config-something\r\n"
                                                      "\r\n"
                                                      ),
                                           theRoute,
                                           mappingType
                                           )
                        == OS_SUCCESS
                        );
         CPPUNIT_ASSERT( strcmp(theRoute.data(),"CONFIG_SERVER_SUBSCRIBE") == 0 );
         CPPUNIT_ASSERT( strcmp(mappingType.data(),"local") == 0 );
      }

      void testSimpleMapNonMatchFieldPrefix()
      {
         ForwardRules theRules;
         OsString     theRoute;
         OsString     mappingType;
         OsString     rulesFile(TEST_DATA_DIR "rulesdata/simple.xml");

         CPPUNIT_ASSERT( theRules.loadMappings(rulesFile, MS, VM, LH )
                        == OS_SUCCESS
                        );

         CPPUNIT_ASSERT( theRules.getRoute(Url("sip:SIPXCHANGE_DOMAIN_NAME"),
                                           SipMessage("SUBSCRIBE sip:SIPXCHANGE_DOMAIN_NAME SIP/2.0\r\n"
                                                      "Event: not-sip-config-something\r\n"
                                                      "\r\n"
                                                      ),
                                           theRoute,
                                           mappingType
                                           )
                        == OS_SUCCESS
                        );
         CPPUNIT_ASSERT( strcmp(theRoute.data(),"REGISTRAR_SERVER_SUBSCRIBE") == 0 );
         CPPUNIT_ASSERT( strcmp(mappingType.data(),"local") == 0 );
      }

      void testSimpleMapNoField()
      {
         ForwardRules theRules;
         OsString     theRoute;
         OsString     mappingType;
         OsString     rulesFile(TEST_DATA_DIR "rulesdata/simple.xml");

         CPPUNIT_ASSERT( theRules.loadMappings(rulesFile, MS, VM, LH )
                        == OS_SUCCESS
                        );

         CPPUNIT_ASSERT( theRules.getRoute(Url("sip:SIPXCHANGE_DOMAIN_NAME"),
                                           SipMessage("UNKNOWN sip:SIPXCHANGE_DOMAIN_NAME SIP/2.0\r\n"
                                                      "\r\n"
                                                      ),
                                           theRoute,
                                           mappingType
                                           )
                        == OS_SUCCESS
                        );
         CPPUNIT_ASSERT( strcmp(theRoute.data(),"REGISTRAR_SERVER_DEFAULT") == 0 );
         CPPUNIT_ASSERT( strcmp(mappingType.data(),"local") == 0 );
      }

};

CPPUNIT_TEST_SUITE_REGISTRATION(ForwardRulesTest);

