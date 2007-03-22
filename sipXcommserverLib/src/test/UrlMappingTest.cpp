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

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>
#include <sipxunit/TestUtilities.h>

#include "net/Url.h"
#include "digitmaps/UrlMapping.h"
#include "sipdb/ResultSet.h"

#include "testlib/FileTestContext.h"

#define VM "VoIcEmAiL"
#define MS "MeDiAsErVeR"
#define LH "LoCaLhOsT"

class UrlMappingTest : public CppUnit::TestCase, public UrlMapping
{
      CPPUNIT_TEST_SUITE(UrlMappingTest);
      CPPUNIT_TEST(testConvertDialString2RegEx);
      CPPUNIT_TEST(testSimpleMap);
      CPPUNIT_TEST(testFieldParams);
      CPPUNIT_TEST(testAddUrlParams);
      CPPUNIT_TEST(testHeaderParamAdd);
      CPPUNIT_TEST(testFieldAdd);
      CPPUNIT_TEST(testAddFieldParams);
      CPPUNIT_TEST(testDigits);
      CPPUNIT_TEST(testVDigits);
      CPPUNIT_TEST(testUserPat);
      CPPUNIT_TEST(testEscape);
      CPPUNIT_TEST(testSpecials);
      CPPUNIT_TEST_SUITE_END();


      public:
      void setUp()
      {
         mFileTestContext = new FileTestContext(TEST_DATA_DIR "/mapdata", TEST_WORK_DIR "/mapdata");
      }
            
      void tearDown()
      {
         delete mFileTestContext;
      }
   
      void testConvertDialString2RegEx()
      {
         UtlString regexp;

         UtlString pattern;
         pattern = "12x";
         convertDialString2RegEx(pattern,regexp);
         ASSERT_STR_EQUAL("^12(.)$",regexp.data());

         pattern = "12.";
         convertDialString2RegEx(pattern,regexp);
         ASSERT_STR_EQUAL("^12(.*)$",regexp.data());

         pattern = "12*";
         convertDialString2RegEx(pattern,regexp);
         ASSERT_STR_EQUAL("^12\\*$",regexp.data());

         pattern = "(12)";
         convertDialString2RegEx(pattern,regexp);
         ASSERT_STR_EQUAL("^\\(12\\)$",regexp.data());

         pattern = "[12]3";
         convertDialString2RegEx(pattern,regexp);
         ASSERT_STR_EQUAL("^([12]3)$",regexp.data());

         pattern = "1[23]";
         convertDialString2RegEx(pattern,regexp);
         ASSERT_STR_EQUAL("^1([23])$",regexp.data());

         pattern = "1\\[23]";
         convertDialString2RegEx(pattern,regexp);
         ASSERT_STR_EQUAL("^1\\[23]$",regexp.data());

         pattern = "1\\[23\\]"; // userPattern 1\[23]
         convertDialString2RegEx(pattern,regexp);
         ASSERT_STR_EQUAL("^1\\[23\\]$",regexp.data());

         pattern = "\\1\\\\2\\\\\\3"; // userPattern \1\\2\\\3
         convertDialString2RegEx(pattern,regexp);
         ASSERT_STR_EQUAL("^1\\\\2\\\\3$",regexp.data()); // regexp 1\\2\\3

         // printf("pattern '%s' regexp '%s'\n", pattern.data(), regexp.data());

      }
   
      void testSimpleMap()
      {
         UrlMapping* urlmap;
         ResultSet registrations;
         UtlBoolean isPSTNnumber = false;
         ResultSet permissions;
         UtlString actual;

         CPPUNIT_ASSERT( urlmap = new UrlMapping() );
         UtlString simpleXml;
         mFileTestContext->inputFilePath("simple.xml", simpleXml);
         CPPUNIT_ASSERT( urlmap->loadMappings(simpleXml.data(),
                                              MS, VM, LH
                                              )
                        == OS_SUCCESS
                        );

         // the interface says getContactList returns an OsStatus,
         // but it is not set so don't test it

         urlmap->getContactList( Url("sip:THISUSER@THISHOST.THISDOMAIN")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact", actual);
         ASSERT_STR_EQUAL("sip:NEWUSER@NEWHOST.NEWDOMAIN",actual);
         registrations.destroyAll();

         urlmap->getContactList( Url("sip:THISUSER@THISDOMAIN")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact", actual);
         ASSERT_STR_EQUAL("sip:NEWUSER@NEWHOST.NEWDOMAIN", actual);
         registrations.destroyAll();

         urlmap->getContactList( Url("sip:THATUSER@THISHOST.THISDOMAIN")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact", actual);
         ASSERT_STR_EQUAL( "sip:NEWUSER@NEWHOST.NEWDOMAIN", actual);
         registrations.destroyAll();

         urlmap->getContactList( Url("sip:THATUSER@THISDOMAIN")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact", actual);
         ASSERT_STR_EQUAL( "sip:NEWUSER@NEWHOST.NEWDOMAIN", actual );
         registrations.destroyAll();

         urlmap->getContactList( Url("sip:OTHERUSER@THISHOST.THIDOMAIN")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 0 );

         urlmap->getContactList( Url("sip:OTHERUSER@THISDOMAIN")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 0 );

         urlmap->getContactList( Url("sip:THISUSER@OTHERHOST.THIDOMAIN")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 0 );

         urlmap->getContactList( Url("sip:THISUSER@OTHERDOMAIN")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 0 );

         urlmap->getContactList( Url("sip:THISUSER@UserChgDOMAIN")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact", actual);
         ASSERT_STR_EQUAL( "sip:NEWUSER@UserChgDOMAIN", actual );
         registrations.destroyAll();

         urlmap->getContactList( Url("sip:THATUSER@UserChgDOMAIN")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact", actual);
         ASSERT_STR_EQUAL( "sip:NEWUSER@UserChgDOMAIN", actual );
         registrations.destroyAll();

         urlmap->getContactList( Url("sip:OTHERUSER@UserChgDOMAIN")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 0 );
         registrations.destroyAll();

         urlmap->getContactList( Url("sip:THISUSER@HostChgDOMAIN")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact", actual);
         ASSERT_STR_EQUAL( "sip:THISUSER@NewHost", actual );
         registrations.destroyAll();

         urlmap->getContactList( Url("sip:THATUSER@HostChgDOMAIN")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact", actual);
         ASSERT_STR_EQUAL( "sip:THATUSER@NewHost", actual );
         registrations.destroyAll();

         // do the domain transformtion again and check that the transport is removed [XRR-114]
         urlmap->getContactList( Url("<sip:THATUSER@HostChgDOMAIN;transport=xyz>")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact", actual);
         ASSERT_STR_EQUAL( "sip:THATUSER@NewHost", actual );
         registrations.destroyAll();

         urlmap->getContactList( Url("sip:OTHERUSER@UserChgDOMAIN")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 0 );
         registrations.destroyAll();

         delete urlmap;
      }

      void testFieldParams()
      {
         UrlMapping urlmap;
         ResultSet registrations;
         UtlBoolean isPSTNnumber = false;
         ResultSet permissions;
         UtlString actual;

         UtlString paramsXml;
         mFileTestContext->inputFilePath("params.xml", paramsXml);

         CPPUNIT_ASSERT( loadUrlMap( urlmap, paramsXml.data()));

         // the interface says getContactList returns an OsStatus,
         // but it is not set so don't test it

         urlmap.getContactList( Url("sip:ADDFIELDS@thisdomain")
                               ,registrations, isPSTNnumber, permissions
                               );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact" , actual );
         ASSERT_STR_EQUAL("<sip:ADDFIELDS@thisdomain>;NEWFIELDPARAM=FIELDVALUE" , actual );
         registrations.destroyAll();
      }
      
      void testAddUrlParams()
      {
         UrlMapping urlmap;
         ResultSet registrations;
         UtlBoolean isPSTNnumber = false;
         ResultSet permissions;
         UtlString actual;

         UtlString paramsXml;
         mFileTestContext->inputFilePath("params.xml", paramsXml);

         CPPUNIT_ASSERT( loadUrlMap( urlmap, paramsXml.data()));

         urlmap.getContactList( Url("sip:ADDURLPARAM@thisdomain")
                               ,registrations, isPSTNnumber, permissions
                               );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact" , actual );
         ASSERT_STR_EQUAL("<sip:ADDURLPARAM@thisdomain;NEWURLPARAM=URLVALUE>" , actual );
         registrations.destroyAll();
    
      }
      
      void testHeaderParamAdd()
      {
         UrlMapping urlmap;
         ResultSet registrations;
         UtlBoolean isPSTNnumber = false;
         ResultSet permissions;
         UtlString actual;

         UtlString paramsXml;
         mFileTestContext->inputFilePath("params.xml", paramsXml);

         CPPUNIT_ASSERT( loadUrlMap( urlmap, paramsXml.data()));

         urlmap.getContactList( Url("sip:ADDHEADERPARAM@thisdomain")
                               ,registrations, isPSTNnumber, permissions
                               );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact" , actual );
         ASSERT_STR_EQUAL("<sip:ADDHEADERPARAM@thisdomain?NEWHEADERPARAM=HEADERVALUE>" , actual );
         registrations.destroyAll();

      }
      
      void testFieldAdd()
      {
         UrlMapping urlmap;
         ResultSet registrations;
         UtlBoolean isPSTNnumber = false;
         ResultSet permissions;
         UtlString actual;

         UtlString paramsXml;
         mFileTestContext->inputFilePath("params.xml", paramsXml);

         CPPUNIT_ASSERT( loadUrlMap( urlmap, paramsXml.data()));

         urlmap.getContactList( Url("<sip:ADDFIELDS@thisdomain;urlparam=avalue>;field=oldvalue")
                               ,registrations, isPSTNnumber, permissions
                               );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact"
                                  , actual
                        );
         ASSERT_STR_EQUAL("<sip:ADDFIELDS@thisdomain;urlparam=avalue>;field=oldvalue;NEWFIELDPARAM=FIELDVALUE"
                         , actual );
         registrations.destroyAll();


      }
      
      void testAddFieldParams()
      {
         UrlMapping urlmap;
         ResultSet registrations;
         UtlBoolean isPSTNnumber = false;
         ResultSet permissions;
         UtlString actual;

         UtlString paramsXml;
         mFileTestContext->inputFilePath("params.xml", paramsXml);

         CPPUNIT_ASSERT( loadUrlMap( urlmap, paramsXml.data()));

         urlmap.getContactList( Url("<sip:ADDFIELDS@thisdomain;NEWURLPARAM=oldvalue>")
                               ,registrations, isPSTNnumber, permissions
                               );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact", actual );

         ASSERT_STR_EQUAL("<sip:ADDFIELDS@thisdomain;NEWURLPARAM=oldvalue>;NEWFIELDPARAM=FIELDVALUE", actual);
         registrations.destroyAll();
      }


      void testDigits()
      {
         UrlMapping* urlmap;
         ResultSet registrations;
         UtlBoolean isPSTNnumber = false;
         ResultSet permissions;
         UtlString actual;

         CPPUNIT_ASSERT( urlmap = new UrlMapping() );

         UtlString digitsXml;
         mFileTestContext->inputFilePath("digits.xml", digitsXml);

         CPPUNIT_ASSERT( urlmap->loadMappings(digitsXml.data(),
                                              MS, VM, LH
                                              )
                        == OS_SUCCESS
                        );

         // the interface says getContactList returns an OsStatus,
         // but it is not set so don't test it

         urlmap->getContactList( Url("sip:911@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact"
                                  , actual
                        );
         ASSERT_STR_EQUAL("sip:911@GW-EMERGENCY-DIALING-ADDR"
                         , actual );
         registrations.destroyAll();

         urlmap->getContactList( Url("911@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact"
                                  , actual
                        );
         ASSERT_STR_EQUAL("sip:911@GW-EMERGENCY-DIALING-ADDR"
                         , actual );
         registrations.destroyAll();

         urlmap->getContactList( Url("100@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact"
                                  , actual
                        );
         ASSERT_STR_EQUAL("<sip:100@" MS ";play=" VM "autoattendant>"
                         , actual );
         registrations.destroyAll();

         urlmap->getContactList( Url("operator@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact"
                                  , actual
                        );
         ASSERT_STR_EQUAL("<sip:operator@" MS ";play=" VM "autoattendant>"
                         , actual );
         registrations.destroyAll();

         urlmap->getContactList( Url("101@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact"
                                  , actual
                        );
         ASSERT_STR_EQUAL("<sip:101@" MS ";play=" VM "mailbox%3D101>"
                         , actual );
         registrations.destroyAll();

         urlmap->getContactList( Url("2666@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact"
                                  , actual
                        );
         ASSERT_STR_EQUAL("<sip:666@" MS ";play=" VM "mailbox%3D666>"
                         , actual );
         registrations.destroyAll();

         // Check for plus sign in the URL
         // the plus should be escaped in the mailbox parameter
         // and NOT in the user part
         urlmap->getContactList( Url("+9663@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact"
                                  , actual
                        );
         ASSERT_STR_EQUAL("<sip:+9663@" MS ";play=" VM "mailbox%3D%2B9663>;q=0.1"
                         , actual );
         registrations.destroyAll();


         urlmap->getContactList( Url("918001234567@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact"
                                  , actual
                        );
         ASSERT_STR_EQUAL("sip:8001234567@GW-800-DIALING-ADDR"
                         , actual );
         registrations.destroyAll();

         urlmap->getContactList( Url("18001234567@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact"
                                  , actual
                        );
         ASSERT_STR_EQUAL("sip:8001234567@GW-800-DIALING-ADDR"
                         , actual );
         registrations.destroyAll();

         urlmap->getContactList( Url("8001234567@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact"
                                  , actual
                        );
         ASSERT_STR_EQUAL("sip:8001234567@GW-800-DIALING-ADDR"
                         , actual );
         registrations.destroyAll();

         urlmap->getContactList( Url("91800123456@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 0 );

         urlmap->getContactList( Url("691800123@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 0 );


         delete urlmap;
      }



      void testVDigits()
      {
         UrlMapping* urlmap;
         ResultSet registrations;
         UtlBoolean isPSTNnumber = false;
         ResultSet permissions;
         UtlString actual;

         CPPUNIT_ASSERT( urlmap = new UrlMapping() );

         UtlString vdigitsXml;
         mFileTestContext->inputFilePath("vdigits.xml", vdigitsXml);

         CPPUNIT_ASSERT( urlmap->loadMappings(vdigitsXml.data(),
                                              MS, VM, LH
                                              )
                        == OS_SUCCESS
                        );

         urlmap->getContactList( Url("sip:15@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 2 );
         getResult( registrations, 0, "contact"
                                  , actual
                        );
         ASSERT_STR_EQUAL("sip:12315@pattern1"
                         , actual );
         getResult( registrations, 1, "contact"
                                  , actual
                        );
         ASSERT_STR_EQUAL("sip:32115@pattern2"
                         , actual );
         registrations.destroyAll();

         urlmap->getContactList( Url("sip:156@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 2 );
         getResult( registrations, 0, "contact"
                                  , actual
                        );
         ASSERT_STR_EQUAL("sip:123156@pattern1"
                         , actual );
         getResult( registrations, 1, "contact"
                                  , actual
                        );
         ASSERT_STR_EQUAL("sip:321156@pattern2"
                         , actual );
         registrations.destroyAll();

         urlmap->getContactList( Url("sip:156789@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 2 );
         getResult( registrations, 0, "contact"
                                  , actual
                        );
         ASSERT_STR_EQUAL("sip:123156789@pattern1"
                         , actual );
         getResult( registrations, 1, "contact"
                                  , actual
                        );
         ASSERT_STR_EQUAL("sip:321156789@pattern2"
                         , actual );
         registrations.destroyAll();

         urlmap->getContactList( Url("sip:5789@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact"
                                  , actual
                        );
         ASSERT_STR_EQUAL("sip:fourfive5789@pattern3"
                         , actual );
         registrations.destroyAll();

         urlmap->getContactList( Url("sip:4789@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact"
                                  , actual
                        );
         ASSERT_STR_EQUAL("sip:fourfive4789@pattern3"
                         , actual );
         registrations.destroyAll();

         urlmap->getContactList( Url("sip:6789@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 0 );
         registrations.destroyAll();

         urlmap->getContactList( Url("sip:489@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 0 );
         registrations.destroyAll();

         urlmap->getContactList( Url("sip:89123@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 2 );
         getResult( registrations, 0, "contact"
                                  , actual
                        );
         ASSERT_STR_EQUAL("sip:eight9123@pattern4"
                         , actual );
         getResult( registrations, 1, "contact"
                                  , actual
                        );
         ASSERT_STR_EQUAL("sip:digits89123@pattern5"
                         , actual );
         registrations.destroyAll();

         urlmap->getContactList( Url("sip:81456@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 0 );
         registrations.destroyAll();

         urlmap->getContactList( Url("sip:8045@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 0 );
         registrations.destroyAll();

         urlmap->getContactList( Url("sip:9999@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact"
                   , actual
                   );
         /* this is actually the wrong answer, because the rule is improper,
            but this tests that there is no pointer fault, which there used to be */
         ASSERT_STR_EQUAL("sip:thisdomain"
                          , actual );

         registrations.destroyAll();

         delete urlmap;
      }

      void testUserPat()
      {
         UrlMapping* urlmap;
         ResultSet registrations;
         UtlBoolean isPSTNnumber = false;
         ResultSet permissions;
         UtlString actual;

         CPPUNIT_ASSERT( urlmap = new UrlMapping() );

         UtlString userpatXml;
         mFileTestContext->inputFilePath("userpat.xml", userpatXml);

         CPPUNIT_ASSERT( urlmap->loadMappings(userpatXml.data(),
                                              MS, VM, LH
                                              )
                        == OS_SUCCESS
                        );

         // this one illustrates a problem - vdigits matches everything after the first non-constant
         urlmap->getContactList( Url("sip:THISUSER@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact", actual);
         ASSERT_STR_EQUAL( "sip:NEWTHISUSER@LEFTDOMAIN", actual );
         registrations.destroyAll();


         urlmap->getContactList( Url("sip:USERTHIS@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact", actual);
         ASSERT_STR_EQUAL( "sip:OLDTHIS@RIGHTDOMAIN", actual );
         registrations.destroyAll();

         // checks case sensitivity
         urlmap->getContactList( Url("sip:upperTHIS@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 0 );
         registrations.destroyAll();

         urlmap->getContactList( Url("sip:UPPERTHIS@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact", actual);
         ASSERT_STR_EQUAL( "sip:NEWTHIS@UPPERDOMAIN", actual );
         registrations.destroyAll();

         delete urlmap;
      }

      void testEscape()
      {
         UrlMapping* urlmap;
         ResultSet registrations;
         UtlBoolean isPSTNnumber = false;
         ResultSet permissions;
         UtlString actual;

         CPPUNIT_ASSERT( urlmap = new UrlMapping() );

         UtlString escapeXml;
         mFileTestContext->inputFilePath("escape.xml", escapeXml);

         CPPUNIT_ASSERT( urlmap->loadMappings(escapeXml.data(),
                                              MS, VM, LH
                                              )
                        == OS_SUCCESS
                        );


         urlmap->getContactList( Url("sip:Fixed01@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 2 );
         getResult( registrations, 0, "contact", actual);
         ASSERT_STR_EQUAL( "sip:Fixed01@digits", actual );
         getResult( registrations, 1, "contact", actual);
         ASSERT_STR_EQUAL( "sip:01@vdigits", actual );
         registrations.destroyAll();

         urlmap->getContactList( Url("sip:FiNed01@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 0 );
         registrations.destroyAll();


         urlmap->getContactList( Url("sip:aa.999@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 2 );
         getResult( registrations, 0, "contact", actual);
         ASSERT_STR_EQUAL( "sip:aa.999@digits", actual );

         getResult( registrations, 1, "contact", actual);
         ASSERT_STR_EQUAL( "sip:999@vdigits", actual );
         registrations.destroyAll();

         urlmap->getContactList( Url("sip:aa0888@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 0 );
         registrations.destroyAll();


         urlmap->getContactList( Url("sip:*99123@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 2 );
         getResult( registrations, 0, "contact", actual);
         ASSERT_STR_EQUAL( "sip:*99123@digits", actual );
         getResult( registrations, 1, "contact", actual);
         ASSERT_STR_EQUAL( "sip:123@vdigits", actual );
         registrations.destroyAll();

         delete urlmap;
      }

   void testSpecials()
      {
         /* tests characters that are special in Perl Regular Expressions
          * but not in a dial string
          */

         UrlMapping* urlmap;
         ResultSet registrations;
         UtlBoolean isPSTNnumber = false;
         ResultSet permissions;
         UtlString actual;

         CPPUNIT_ASSERT( urlmap = new UrlMapping() );

         UtlString specialsXml;
         mFileTestContext->inputFilePath("specials.xml", specialsXml);


         CPPUNIT_ASSERT( urlmap->loadMappings(specialsXml.data(),
                                              MS, VM, LH
                                              )
                        == OS_SUCCESS
                        );

         // the interface says getContactList returns an OsStatus,
         // but it is not set so don't test it

         urlmap->getContactList( Url("sip:101@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 0 );

         registrations.destroyAll();

         urlmap->getContactList( Url("sip:101+@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact", actual);
         ASSERT_STR_EQUAL("sip:OneZeroOnePlus@thisdomain",actual);
         registrations.destroyAll();

         urlmap->getContactList( Url("sip:1011@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 0 );

         registrations.destroyAll();

         urlmap->getContactList( Url("sip:1012?@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact", actual);
         ASSERT_STR_EQUAL("sip:OneZeroOneTwoQmark@thisdomain",actual);
         registrations.destroyAll();

         urlmap->getContactList( Url("sip:(101)@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact", actual);
         ASSERT_STR_EQUAL("sip:ParenOneZeroOneParen@thisdomain",actual);
         registrations.destroyAll();

         urlmap->getContactList( Url("sip:1013*@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact", actual);
         ASSERT_STR_EQUAL("sip:OneZeroOneThreeStar@thisdomain",actual);
         registrations.destroyAll();

         urlmap->getContactList( Url("sip:101$@thisdomain")
                                ,registrations, isPSTNnumber, permissions
                                );
         CPPUNIT_ASSERT( permissions.getSize() == 0 );
         CPPUNIT_ASSERT( registrations.getSize() == 1 );
         getResult( registrations, 0, "contact", actual);
         ASSERT_STR_EQUAL("sip:OneZeroOneDollar@thisdomain",actual);
         registrations.destroyAll();

         delete urlmap;
      }

      protected:

      void getResult( ResultSet& resultSet
                     ,int         index
                     ,const char* key
                     ,UtlString&  result
                     )
      {
         UtlHashMap hash;
         resultSet.getIndex( index, hash );
         UtlString theKey(key);
         result = *((UtlString*)hash.findValue(&theKey));
      }

      bool loadUrlMap( UrlMapping& urlmap, const char* mapfile )
      {
         return (   urlmap.loadMappings(mapfile, MS, VM, LH )
                 == OS_SUCCESS
                 );
      };

   FileTestContext* mFileTestContext;
   
};

CPPUNIT_TEST_SUITE_REGISTRATION(UrlMappingTest);
