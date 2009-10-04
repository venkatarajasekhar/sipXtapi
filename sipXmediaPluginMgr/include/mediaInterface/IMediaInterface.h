// Copyright 2008 AOL LLC.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA. 
// 
// Copyright (C) 2005-2008 SIPez LLC.
// Licensed to SIPfoundry under a Contributor Agreement.
// 
// Copyright (C) 2004-2008 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// Copyright (C) 2004-2006 Pingtel Corp.  All rights reserved.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// $$
//////////////////////////////////////////////////////////////////////////////

// Author: Dan Petrie (dpetrie AT SIPez DOT com)

#ifndef _IMediaInterface_h_
#define _IMediaInterface_h_

// SYSTEM INCLUDES

// APPLICATION INCLUDES
#include <os/OsStatus.h>
#include <os/OsDefs.h>
#include <os/OsProtectEvent.h>
#include <os/OsMsgQ.h>
#include <os/OsDatagramSocket.h>
#include <net/SdpBody.h>
#include <net/ProxyDescriptor.h>
#include <os/IOsNatSocket.h>
#include <tapi/sipXtapi.h>
#include <mediaBaseImpl/MediaDeviceInfo.h>
#include <mediaBaseImpl/MediaConnectivityInfo.h>

// DEFINES
#define CPMI_FLAGS_ENABLE_LOCAL     0x00000001
#define CPMI_FLAGS_ENABLE_STUN      0x00000002
#define CPMI_FLAGS_ENABLE_TURN      0x00000004
#define CPMI_FLAGS_ENABLE_ARS       0x00000008
#define CPMI_FLAGS_ENABLE_ICE       0x00010000
#define CPMI_FLAGS_ENABLE_ALL       0x0001000F

#define CPMI_FLAGS_DEFAULT          (CPMI_FLAGS_ENABLE_ALL)


// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
  /// The intended data that will be flowing through a socket.
typedef enum SocketPurpose
{
   UNKNOWN,
   RTP_AUDIO,  ///< Socket is intended to transport RTP Audio data
   RTCP_AUDIO, ///< Socket is intended to transport RTCP Audio control data
   RTP_VIDEO,  ///< Socket is intended to transport RTP Video data
   RTCP_VIDEO  ///< Socket is intended to transport RTCP Video control data
} SocketPurpose;

  /// SipX Media Interface Audio Bandwidth IDs
typedef enum SIPXMI_AUDIO_BANDWIDTH_ID
{
     /// ID for codecs with variable bandwidth requirements
   AUDIO_MICODEC_BW_VARIABLE=0,

     /// ID for codecs with low bandwidth requirements
   AUDIO_MICODEC_BW_LOW,
     /// ID for codecs with normal bandwidth requirements
   AUDIO_MICODEC_BW_NORMAL,
     /// ID for codecs with high bandwidth requirements
   AUDIO_MICODEC_BW_HIGH,

     /**
     *  Possible return value for sipxConfigGetAudioCodecPreferences.
     *  This ID indicates the available list of codecs was overridden by a 
     *  sipxConfigSetAudioCodecByName call.
     */
   AUDIO_MICODEC_BW_CUSTOM,       

     /**
     *  Value used to signify the default bandwidth level when calling 
     *  sipxCallConnect, sipxCallAccept, or sipxConferenceAdd 
     */
   AUDIO_MICODEC_BW_DEFAULT       

} SIPXMI_AUDIO_BANDWIDTH_ID;


/**
*  @brief Interface declaration for receiving socket idle notifications.
*/
class ISocketEvent
{
public:
    virtual void onIdleData(OsSocket* const pSocket,
                             SocketPurpose purpose,
                             const int millisecondsIdle) = 0;

   virtual void onReadData(OsSocket* const pSocket,
                           SocketPurpose purpose) = 0;

   virtual ~ISocketEvent() { } ;
};

class IMediaEventEmitter
{
public:
   virtual void onListenerRemoved() = 0;
    virtual ~IMediaEventEmitter() {} ;
};

typedef enum IMediaEvent_DeviceErrors
{
   IError_DeviceUnplugged
} IMediaEvent_DeviceErrors;

typedef enum IMediaEvent_DeviceTypes
{
   IDevice_Audio,
   IDevice_Video
} IMediaEvent_DeviceTypes;

/**
 *  @brief Interface declaration for receiving device error 
 *  notifications/audio events
 */
class IMediaEventListener
{
public:    
   virtual void onFileStart(IMediaEvent_DeviceTypes type) = 0 ;
   virtual void onFileStop(IMediaEvent_DeviceTypes type) = 0 ;
   virtual void onBufferStart(IMediaEvent_DeviceTypes type) = 0 ;
   virtual void onBufferStop(IMediaEvent_DeviceTypes type) = 0 ;
   virtual void onDeviceError(IMediaEvent_DeviceTypes type, IMediaEvent_DeviceErrors errCode) = 0;
    virtual void onIceFailed(IMediaEvent_DeviceTypes type) = 0;
   virtual void onListenerAddedToEmitter(IMediaEventEmitter* pEmitter) = 0;

   virtual ~IMediaEventListener() { } ;
};


// FORWARD DECLARATIONS
class SdpCodec;
class SdpCodecList;
class MpStreamPlaylistPlayer;
class MpStreamPlayer;
class MpStreamQueuePlayer;
class OsMsgDispatcher;

/** 
 * @brief Abstract media control interface.
 * 
 * The CpCallManager creates a IMediaInterface object for each call created.
 * The media interface is then used to control and query the media sub-system 
 * used for that call.  As connections are added to the call, the 
 * media interface is used to add those connections to the media control system 
 * such that all connections in that call are bridged together.
 * 
 * @note This abstract class must be sub-classed and implemented to replace 
 *       the default media sub-system.
 */
class IMediaInterface : public UtlInt
{
/* //////////////////////////// PUBLIC //////////////////////////// */
public:

/* =========================== CREATORS =========================== */
/* ========================= DESTRUCTORS ========================== */
    virtual ~IMediaInterface() { }

public:

     /// @brief public interface for destroying this media interface
   virtual void release() = 0; 

/* ========================= MANIPULATORS ========================= */
     /// @brief Create a media connection in the media processing subsystem.
   virtual 
   OsStatus createConnection(
               int& connectionId,
               bool bInitiating,
               const char* szLocalAddress,
               int localPort = 0,
               void* videoWindowHandle = NULL,
               unsigned long flags = 0,
               void* const pSecurityAttributes = NULL,
               ISocketEvent* pSocketIdleSink = NULL,
               IMediaEventListener* pMediaEventListener = NULL,
               const RtpTransportOptions rtpTransportOptions=RTP_TRANSPORT_UDP,
               int callHandle=0) = 0 ;
     /**<
     *  One instance of the IMediaInterface exists for each call, however, 
     *  each leg of the call requires in individual connection.
     *
     *  @param[out] connectionId - A newly allocated connection id returned via 
     *              this call.  The connection passed to many other media 
     *              processing methods in this interface.
     *  @param[in]  szLocalAddress - Local address (interface) that should 
     *              be used for this connection.
     *  @param[in]  localPort - Local port that should be used for this
     *              connection.
     *              Note, that in fact two ports will be allocated - 
     *              (localPort) for RTP and (localPort+1) for RTCP.
     *              If 0 is passed, port number will be selected automatically.
     *  @param[in]  videoWindowHandle - Video Window handle if using video.  
     *              Supply a window handle of \c NULL to disable video for this 
     *              call/connection.
     *  @param      pSecurityAttributes - Pointer to a 
     *              SIPXVE_SECURITY_ATTRIBUTES object.  
     *  @param      pSocketIdleSink - <<UNKNOWN -- What does this do? 
     *              -- kkyzivat 20070801 >>
     *  @param      pMediaEventListener - <<UNKNOWN -- What does this do?
     *              -- kkyzivat 20070801 >>
     *  @param[in]  rtpTransportOptions RTP_TRANSPORT_UDP, RTP_TRANSPORT_TCP, 
     *              or BOTH
     *  @retval     UNKNOWN - << TODO: Add useful return values here - i.e.
     *              failure codes to expect, etc. -- kkyzivat 20070801 >>
     */ 

     /// @brief Set PLC method to use for codecs which does not have own PLC.
   virtual OsStatus setPlcMethod(int connectionId,
                                 const UtlString &methodName = "") = 0;
     /**<
     *  @param[in] connectionId - Connection Id for the call leg obtained from 
     *             createConnection
     *  @param[in] methodName - Name of PLC method. If method name is unknown,
     *                          default method will be used. This also means
     *                          you could pass "" to select default method.
     *
     *  @retval OS_SUCCESS - message to connection was sent, PLC method will
     *          be changed on next message processing interval.
     *  @retval OS_NOT_FOUND - connection with given \p connectionId was not found.
     *  @retval OS_NOT_SUPPORTED - this implementation of IMediaInterface
     *          does not support PLC algorithm changing.
     *  @retval OS_NOT_YET_IMPLEMENTED - this implementation of IMediaInterface
     *          might support PLC algorithm changing, but this feature is not
     *          yet implemented.
     */

     /// @brief Add, replace, or clear the media notification dispatcher held by the MI.
   virtual OsMsgDispatcher* 
   setNotificationDispatcher(OsMsgDispatcher* pNotificationDispatcher) = 0;
     /**<
     *  Gives the Media Interface an object to help in the dispatching of 
     *  notification messages to users of the media interface.  Users
     *  are free to subclass OsMsgDispatcher to filter messages as
     *  they see fit, and should hold on to it to receive their messages.
     *  
     *  @param[in] pNotificationDispatcher - A notification dispatcher to give 
     *             to the MI.
     *  @return Pointer to the previous media notification dispatcher set in
     *          this MI.  If there was no previous media notification dispatcher, 
     *          \p NULL is returned.
     */

     /// @brief Enable or disable media notifications for one/all resource(s).
   virtual OsStatus
   setNotificationsEnabled(bool enabled, 
                           const UtlString& resourceName = NULL) = 0;
     /**<
     *  Enable or disable media notifications for a given resource or all resources.
     *
     *  @NOTE If /p NULL is passed for resource name, then all resources 
     *        will have all notifications enabled/disabled
     *  @NOTE This is an asynchronous operation.  After calling this, it may
     *        take a bit of time before the new state takes effect.
     *  
     *  @param[in] enabled - Whether notification type is to be enabled or disabled.
     *  @param[in] resourceName - the name of the resource to have notifications 
     *             enabled/disabled on.
     *  @retval OS_SUCCESS if the initial sending of a message to enable/disable
     *          notifications succeeded.
     *  @retval OS_NOT_FOUND if there is no resource named /p resourceName.
     *  @retval OS_FAILED if some other failure in queuing the message occurred.
     */

    /// @brief Set the remote sip user agent for a connection.   
    virtual OsStatus setUserAgent(int         connectionId,
                                 const char* szUserAgent) = 0 ;

    /// @brief Get the remote sip user agent for a connection (if set)
    virtual const char* getUserAgent(int connectionId) = 0 ;
   
    virtual OsStatus setMediaData(const int data) = 0;

     /// @brief Set the secure RTP parameters.
    virtual OsStatus setSrtpParams(SdpSrtpParameters& srtpParameters) = 0;
     /**<
     *  @param[in] srtpParameters - the parameter block to pull requested
     *             srtp settings from.
     *  @retval     UNKNOWN - << TODO: Add useful return values here - i.e.
     *              failure codes to expect, etc. -- kkyzivat 20070801 >>
     */


     /// @brief Set the connection destination (target) for the designated
     ///        media connection.
   virtual OsStatus setConnectionDestination(int connectionId,
                                             const char* rtpAudioAddress, 
                                             int rtpAudioPort,
                                             int rtcpAudioPort,
                                             int videoType,
                                             const char* rtpVideoAddress,
                                             int rtpVideoPort,
                                             int rtcpVideoPort,
                                             int audioType) = 0 ;

     /// @brief Add an alternate Audio RTP connection destination for 
     ///        this media interface.
   virtual OsStatus addAudioRtpConnectionDestination(int connectionId,
                                                     int iPriority,
                                                     const char* candidateIp, 
                                                     int candidatePort) = 0 ;
     /**<
     * Alternerates are generally obtained from the SdpBody in the form
     * of candidate addresses.  When adding an alternate connection, the
     * implementation should use an ICE-like method to determine the 
     * best destination address.
     *
     * @param[in] connectionId - Connection Id for the call leg obtained from 
     *            createConnection
     * @param[in] iPriority - Relatively priority of the destination. 
     *            Higher numbers have greater priority.
     * @param[in] candidateIp - Target/Candidate IP Address
     * @param[in] candidatePort - Target/Candidate Port
     *  @retval    UNKNOWN - << TODO: Add useful return values here - i.e.
     *             failure codes to expect, etc. -- kkyzivat 20070801 >>
     */

     /// @brief Add an alternate Audio RTCP connection destination for 
     ///        this media interface.
   virtual OsStatus addAudioRtcpConnectionDestination(int connectionId,
                                                       int iPriority,
                                                       const char* candidateIp,
                                                       int candidatePort) = 0 ;
     /**<
     *  @see IMediaInterface::addAudioRtpConnectionDestination
     */

     /// @brief Add an alternate Video RTP connection destination for 
     ///        this media interface.
   virtual OsStatus addVideoRtpConnectionDestination(int connectionId,
                                                     int iPriority,
                                                     const char* candidateIp,
                                                     int candidatePort) = 0 ;
     /**<
     *  @see IMediaInterface::addAudioRtpConnectionDestination
     */

     /// @brief Add an alternate Video RTCP connection destination for 
     ///        this media interface.
   virtual OsStatus addVideoRtcpConnectionDestination(int connectionId,
                                                      int iPriority,
                                                      const char* candidateIp,
                                                      int candidatePort) = 0 ;
     /**<
     *  @see IMediaInterface::addAudioRtpConnectionDestination
     */

    virtual OsStatus addAudioArsConnectionDestination(int         connectionId,
                                                      int         iPriority,
                                                      const char* candidateIp, 
                                                      int         candidatePort) = 0 ;

    virtual OsStatus addVideoArsConnectionDestination(int         connectionId,
                                                      int         iPriority,
                                                      const char* candidateIp, 
                                                      int         candidatePort) = 0 ;

     /// @brief Start sending RTP using the specified codec list.
   virtual OsStatus startRtpSend(int connectionId, 
                                 int numCodecs,
                                 SdpCodec* sendCodec[]) = 0 ;
     /**<
     *  Generally, this codec list is the intersection between both parties.
     *
     *  @param[in] connectionId - Connection Id for the call leg obtained from 
     *             createConnection
     *  @param[in] numCodec Number of codecs supplied in the sendCodec array
     *  @param[in] sendCodec Array of codecs ordered in sending preference.
     *  @retval    UNKNOWN - << TODO: Add useful return values here - i.e.
     *             failure codes to expect, etc. -- kkyzivat 20070801 >>
     */ 

     /// @brief Start receiving RTP using the specified codec list.
   virtual OsStatus startRtpReceive(int connectionId,
                                    int numCodecs,
                                    SdpCodec* sendCodec[]) = 0;
     /**<
     *  Generally, this codec list is the intersection between both parties.
     *  The media processing subsystem should be prepared to receive any of 
     *  the specified payload type without additional signaling.
     *  For example, it is perfectly legal to switch between codecs on a whim 
     *  if multiple codecs are agreed upon.
     *
     *  @param[in] connectionId - Connection Id for the call leg obtained from 
     *             createConnection
     *  @param[in] numCodec - Number of codecs supplied in the sendCodec array
     *  @param[in] sendCodec - Array of receive codecs 
     *  @retval    UNKNOWN - << TODO: Add useful return values here - i.e.
     *             failure codes to expect, etc. -- kkyzivat 20070801 >>
     */

     /// @brief Enables RTP read notification.
   virtual OsStatus enableRtpReadNotification(int connectionId,
                                              UtlBoolean bEnable = TRUE) = 0;
     /**<
     *  Enables read notification through the ISocketEvent listener passed 
     *  in via createConnection.  This should be enabled immediately after 
     *  calling startRtpReceive.  It is automatically disabled as part of 
     *  stopRtpReceive.
     *
     *  @param[in] connectionId - the ID of the connection to enable RTP read
     *             notification from.
     *  @param[in] bEnable - Whether or not you want to enable or disable 
     *             read notification.
     *  @retval    UNKNOWN - << TODO: Add useful return values here - i.e.
     *             failure codes to expect, etc. -- kkyzivat 20070801 >>
     */ 

     /// @brief Stop sending RTP and RTCP data for the specified connection
   virtual OsStatus stopRtpSend(int connectionId) = 0 ;
     /**<
     *  @param[in] connectionId - Connection Id for the call leg obtained from 
     *             createConnection
     *  @retval    UNKNOWN - << TODO: Add useful return values here - i.e.
     *             failure codes to expect, etc. -- kkyzivat 20070801 >>
     */ 

     /// @brief Stop receiving RTP and RTCP data for the specified connection
   virtual OsStatus stopRtpReceive(int connectionId) = 0 ;
     /**<
     *  @param[in] connectionId - Connection Id for the call leg obtained from 
     *             createConnection
     *  @retval    UNKNOWN - << TODO: Add useful return values here - i.e.
     *             failure codes to expect, etc. -- kkyzivat 20070801 >>
     */

     /// @brief Delete the specified RTP or RTCP connetion.
   virtual OsStatus deleteConnection(int connectionId) = 0 ;
     /**<
     *  Delete the specified connection and free up any resources associated 
     *  with that connection.
     *
     *  @param[in] connectionId - Connection Id for the call leg obtained 
     *             from createConnection.
     *  @retval    UNKNOWN - << TODO: Add useful return values here - i.e.
     *             failure codes to expect, etc. -- kkyzivat 20070801 >>
     */

     /// @brief Start playing the specified tone for this call.
   virtual OsStatus startTone(int toneId, 
                              UtlBoolean local, 
                              UtlBoolean remote) = 0 ;
     /**<
     *  If the tone is a DTMF tone and the remote flag is set, the interface 
     *  should send out of band DTMF using RFC 2833.  Inband audio should be 
     *  sent to all connections.  If a previous tone was playing, calling 
     *  startTone should automatically stop existing tone.
     * 
     *  @param[in] toneId - The designated tone to play (TODO: make enum)
     *  @param[in] local - True indicates that sound should be played to 
     *             the local speaker (assuming call is in focus).
     *  @param[in] remote - True indicates that the sound should be played to 
     *             all remote parties.  
     *  @retval    UNKNOWN - << TODO: Add useful return values here - i.e.
     *             failure codes to expect, etc. -- kkyzivat 20070801 >>
     */

     /// @brief Stop playing all tones.
   virtual OsStatus stopTone() = 0 ;
     /**
     * Some tones/implementations may not support this.
     * For example, some DTMF playing implementations will only play DTMF 
     * for a fixed interval.
     *
     *  @retval    UNKNOWN - << TODO: Add useful return values here - i.e.
     *             failure codes to expect, etc. -- kkyzivat 20070801 >>
     */

   virtual OsStatus startChannelTone(int connectiondId,
                                     int toneId, 
                                     UtlBoolean local, 
                                     UtlBoolean remote) = 0 ;

   virtual OsStatus stopChannelTone(int connectiondId) = 0 ;


   virtual OsStatus recordChannelAudio(int connectionId,
                                       const char* szFile) = 0 ;

   virtual OsStatus stopRecordChannelAudio(int connectionId) = 0 ;

     /// @brief Play the specified audio URL to the call.
   virtual OsStatus playAudio(const char* url, 
                              UtlBoolean repeat,
                              UtlBoolean local, 
                              UtlBoolean remote,
                              UtlBoolean mixWithMic = false,
                              int downScaling = 100,
                              OsNotification *event = NULL) = 0 ;
     /**<
     *
     *  @param[in] url - Audio url to be played -- The sipX implementation is limited 
     *             to file paths (not file Urls).
     *  @param[in] repeat - If set, loop the audio file until stopAudio is called.
     *  @param[in] local - True indicates that sound should be played to the local 
     *             speaker (assuming call is in focus).
     *  @param[in] remote - True indicates that the sound should be played to all 
     *             remote parties.  
     *  @param[in] mixWithMic - True to mix with microphone or False to replace it.
     *  @param[in] downScaling - 100 for no down scaling (range from 0 to 100) 
     *
     *  @retval    UNKNOWN - << TODO: Add useful return values here - i.e.
     *             failure codes to expect, etc. -- kkyzivat 20070801 >>
     */

   virtual OsStatus playChannelAudio(int connectionId,
                                     const char* url, 
                                     UtlBoolean repeat,
                                     UtlBoolean local, 
                                     UtlBoolean remote,
                                     UtlBoolean mixWithMic = false,
                                     int downScaling = 100,
                                     OsNotification *event = NULL) = 0 ;

     /// @brief Play the specified audio buffer to the call. 
   virtual OsStatus playBuffer(char* buf, 
                               unsigned long bufSize,
                               uint32_t bufRate, 
                               int type, 
                               UtlBoolean repeat,
                               UtlBoolean local, 
                               UtlBoolean remote,
                               OsProtectedEvent* event = NULL,
                               UtlBoolean mixWithMic = false,
                               int downScaling = 100) = 0 ;
     /**<
     *
     *  @TODO This method should also specify the audio format (e.g. samples/per 
     *        second, etc.).
     *
     *  @retval    UNKNOWN - << TODO: Add useful return values here - i.e.
     *             failure codes to expect, etc. -- kkyzivat 20070801 >>
     * 
     *  @see IMediaInterface::playAudio
     */

     /// @brief Pause all playing URLs or buffers
   virtual OsStatus pauseAudio() = 0;
     /**<
     *  @TODO This method should also take an optional uniqueId representing
     *        a particular playing instance to pause, instead of all of them.
     *  @retval OS_SUCCESS if the asynchronous request to pause audio succeeded.
     *  @retval OS_NOT_FOUND if required underlying media resources are not found.
     */

     /// @brief Resume all paused URLs or buffers
   virtual OsStatus resumeAudio() = 0;
     /**<
     *  @TODO This method should also take an optional uniqueId representing
     *        a particular paused instance to resume, instead of all of them.
     *  @retval OS_SUCCESS if the asynchronous request to pause audio succeeded.
     *  @retval OS_NOT_FOUND if required underlying media resources are not found.
     */

     /// @brief Stop playing any URLs or buffers
   virtual OsStatus stopAudio()  = 0 ;
     /**<
     *
     *  @retval    UNKNOWN - << TODO: Add useful return values here - i.e.
     *             failure codes to expect, etc. -- kkyzivat 20070802 >>
     */

   virtual OsStatus stopChannelAudio(int connectionId) = 0 ;

   /**
    * Supplies the media layer with an outgoing RTP or RTCP data packet for
    * the current channel.   See IETF RFC 1889.
    * 
    * @param type The type of packet:  AUDIO or VIDEO, RTP or RTCP
    * @param pData The bytes of the RFC 1889 packet.
    * @param nData The size of the RFC 1889 packet, in bytes.
    */
   virtual void injectMediaPacket(const int channelId,
                                  const SIPX_MEDIA_PACKET_TYPE type,
                                  const char* const pData,
                                  const size_t len) = 0;

     /// @brief Give the focus of the local audio device to the associated call 
   virtual OsStatus giveFocus() = 0 ;
     /**<
     *  (for example, take this call off hold).
     *
     *  @retval    UNKNOWN - << TODO: Add useful return values here - i.e.
     *             failure codes to expect, etc. -- kkyzivat 20070802 >>
     */

     /// @brief Take this call out of focus for the local audio device 
   virtual OsStatus defocus() = 0 ;
     /**<
     *  (for example, put this call on hold).
     *
     *  @retval    UNKNOWN - << TODO: Add useful return values here - i.e.
     *             failure codes to expect, etc. -- kkyzivat 20070802 >>
     */

     /// @brief Create a simple player for this call to play a single stream
   virtual OsStatus createPlayer(MpStreamPlayer** ppPlayer, 
                                 const char* szStream, 
                                 int flags, 
                                 OsMsgQ *pMsgQ = NULL, 
                                 const char* szTarget = NULL) = 0;
     /**<
     *
     *  @retval    UNKNOWN - << TODO: Add useful return values here - i.e.
     *             failure codes to expect, etc. -- kkyzivat 20070802 >>
     * 
     *  @see CpCallManager::createPlayer
     */

     /// @brief Destroy a simple player in this call.
   virtual OsStatus destroyPlayer(MpStreamPlayer* pPlayer) = 0;
     /**<
     *
     *  @retval    UNKNOWN - << TODO: Add useful return values here - i.e.
     *             failure codes to expect, etc. -- kkyzivat 20070802 >>
     */

     /// @brief Create a single-buffered play list player for this call 
   virtual OsStatus createPlaylistPlayer(MpStreamPlaylistPlayer** ppPlayer, 
                                         OsMsgQ *pMsgQ = NULL, 
                                         const char* szTarget = NULL) = 0;
     /**<
     *
     *  @retval    UNKNOWN - << TODO: Add useful return values here - i.e.
     *             failure codes to expect, etc. -- kkyzivat 20070802 >>
     *
     *  @see CpCallManager::createPlayer
     */

     /// @brief Destroy a single-buffered play list player in this call.
   virtual OsStatus destroyPlaylistPlayer(MpStreamPlaylistPlayer* pPlayer) = 0;
     /**
     *
     *  @retval    UNKNOWN - << TODO: Add useful return values here - i.e.
     *             failure codes to expect, etc. -- kkyzivat 20070802 >>
     */

     /// @brief Create a double-buffered list player for this call 
   virtual OsStatus createQueuePlayer(MpStreamQueuePlayer** ppPlayer, 
                                      OsMsgQ *pMsgQ = NULL, 
                                      const char* szTarget = NULL) = 0;
     /**<
     *
     *  @retval    UNKNOWN - << TODO: Add useful return values here - i.e.
     *             failure codes to expect, etc. -- kkyzivat 20070802 >>
     *  
     *  @see CpCallManager::createPlayer
     */

   //! Destroy a double-buffered list player in this call.
   virtual OsStatus destroyQueuePlayer(MpStreamQueuePlayer* pPlayer) = 0;

   //! Set the CPU resource limit for the media connections in this call. 
   /*! This is used to limit the available codecs to only those within 
    * the designated CPU cost limit. 
    */
   virtual void setCodecCPULimit(int iLimit) = 0 ;

   //! Add a listener event to this call that will receive callback 
   //! or queued event notifications upon receipt of DTMF tone events 
   //! (RFC 2833).
   virtual void addToneListener(OsNotification *pListener, int connectionId) = 0;

   //! Remove the specified DTMF listener from this call.
   virtual void removeToneListener(int connectionId) = 0;

   //! Start recording audio for this call.
   virtual OsStatus ezRecord(int ms, 
                             int silenceLength, 
                             const char* fileName, 
                             double& duration, 
                             int& dtmfterm,
                             OsProtectedEvent* ev = NULL) = 0;

     /// @brief Record the microphone data to a file
   virtual OsStatus recordMic(int ms,
                              int silenceLength,
                              const char* fileName) = 0 ;
     /**<
     *  Record a fixed amount of audio from the microphone to a file.
     *  @note The flowgraph must be in focus for this to work properly.
     *
     *  @param[in] ms - The amount of time, in milliseconds, to record.
     *  @param[in] silenceLength - The amount of silence, in SECONDS, before
     *             recording is terminated.
     *  @param[in] fileName - The path and name of a file to record to.
     */

     /// Record the microphone data
   virtual OsStatus recordMic(int ms, UtlString* pAudioBuf) = 0;
     /**<
     *  Record audio from the microphone to a buffer passed in. Capacity of
     *  \a pAudioBuf is increased to hold full recording. Length of \a pAudioBuf
     *  is set to 0 initially and then increased with every recorded frame.
     *
     *  @note The flowgraph must be in focus for this to work properly.
     *
     *  @param[in] ms - The amount of time, in milliseconds, to record.
     *  @param[in] pAudioBuf - Audio buffer to record to.
     */

   //! Stop recording for this call.
   virtual OsStatus stopRecording() = 0;

   //! Set the preferred contact type for this media connection
   virtual void setContactType(int connectionId, SIPX_CONTACT_TYPE eType, SIPX_CONTACT_ID contactId) = 0 ;

   //! Rebuild the codec factory on the fly
   virtual OsStatus setAudioCodecBandwidth(int connectionId, int bandWidth) = 0;

   //! Rebuild codec factory with one video codec
   virtual OsStatus rebuildCodecFactory(int connectionId, 
                                        int audioBandwidth, 
                                        int videoBandwidth, 
                                        UtlString& videoCodec) = 0;


   //! Set connection bitrate on the fly
   virtual OsStatus setConnectionBitrate(int connectionId, int bitrate) = 0 ;

   //! Set connection framerate on the fly
   virtual OsStatus setConnectionFramerate(int connectionId, int framerate) = 0;

    /// Provide an invalid connectionId
   virtual int getInvalidConnectionId() = 0;

   virtual OsStatus setVideoWindowDisplay(int connectionId, const void* pDisplay) = 0;

   virtual OsStatus setSecurityAttributes(const void* security) = 0;

   virtual OsStatus generateVoiceQualityReport(int         connectionId,
                                               const char* callId,
                                               char*  szReport,
                                               size_t reprotSize) = 0 ;


   virtual void setConnectionTcpRole(const int connectionId,
                                     const RtpTcpRoles role) = 0;

/* ============================ ACCESSORS ================================= */

   /**
    * Get the port, address, and codec capabilities for the specified media 
    * connection.  The IMediaInterface implementation is responsible for 
    * managing port allocations.
    *  
    * @param connectionId Connection Id for the call leg obtained from 
    *        createConnection
    * @param rtpHostAddress IP address or hostname that should be advertised
    *        in SDP data.
    * @param rtpPort RTP port number that should be advertised in SDP.
    * @param rtcpPort RTCP port number that should be advertised in SDP.
    * @param supportedCodecs List of supported codecs.
    * @param srtParams supported SRTP parameters
    * @param bandWidth bandwidth limitation id
    */
   virtual OsStatus getCapabilities(int connectionId, 
                                    UtlSList* pAudioContacts,
                                    UtlSList* pVideoContacts,                                    
                                    SdpCodecList& supportedCodecs,
                                    SdpSrtpParameters& srtpParams,
                                    int bandWidth,
                                    int& videoBandwidth,
                                    int& videoFramerate) = 0;
    

     /// @brief Calculate the current cost for the current set of sending/receiving codecs.
   virtual int getCodecCPUCost() = 0 ;

     /// @brief Calculate the worst case cost for the current set of sending/receiving codecs.
   virtual int getCodecCPULimit() = 0 ;

     /// @brief Returns the sample rate of the flowgraph.
   virtual uint32_t getSamplesPerSec() = 0;

     /// @brief Returns the samples per frame of the flowgraph.
   virtual uint32_t getSamplesPerFrame() = 0;

     /// @brief Returns the flowgraph's message queue
   virtual OsMsgQ* getMsgQ() = 0 ;

     /// @brief Returns the Media Notification dispatcher this controls.
   virtual OsMsgDispatcher* getNotificationDispatcher() = 0;

     /// @brief Returns the primary codec for the connection
   virtual OsStatus getPrimaryCodec(int connectionId, 
                                    UtlString& audioCodec,
                                    UtlString& videoCodec,
                                    int* audiopPayloadType,
                                    int* videoPayloadType,
                                    bool& isEncrypted) = 0;

   virtual const void* getVideoWindowDisplay() = 0;

   virtual OsStatus getAudioEnergyLevels(int& iInputEnergyLevel,
                                         int& iOutputEnergyLevel) = 0;

   virtual OsStatus getAudioEnergyLevels(int connectionId,
                                         int& iInputEnergyLevel,
                                         int& iOutputEnergyLevel,
                                         int& nContributors,
                                         unsigned int* pContributorSRCIds,
                                         int* pContributorEngeryLevels)  = 0;

   virtual OsStatus getAudioRtpSourceIDs(int connectionId,
                                         unsigned int& uiSendingSSRC,
                                         unsigned int& uiReceivingSSRC) = 0;

   /**
    * Get Audio RTCP stats
    */
   virtual OsStatus getAudioRtcpStats(const int connectionId,
       SIPX_RTCP_STATS* const pStats) = 0;

   /**
    * Get Video RTCP stats
    */
   virtual OsStatus getVideoRtcpStats(const int connectionId,
       SIPX_RTCP_STATS* const pStats) = 0;

   /**
    * Get media device info
    */
   virtual OsStatus getMediaDeviceInfo(int connectionId,
                                       MediaDeviceInfo::MediaDeviceInfoType type,
                                       MediaDeviceInfo& info) = 0;

   /**
    * Get media connectivity info
    */
   virtual bool getMediaConnectivityInfo(MediaConnectivityInfo::MediaConnectivityInfoType type,
                                  MediaConnectivityInfo& rMediaConnectivityInfo) = 0;


   virtual OsStatus setRemoteVolumeScale(const int connectionId,
       const int scale = 100) = 0;

   //! Set a media property on the media interface
    /*
     * Media interfaces that wish to inter-operate should implement the following properties
     * and values:
     *
     * Property Name                  Property Values
     * =======================        ===============
     * "audioInput1.muteState"        "true", "false" for systems that may have a microphone for each conference or 2-way call
     * "audioInput1.device"           same value as szDevice in sipxAudioSetCallInputDevice
     * "audioOutput1.deviceType"      "speaker", "ringer" same as sipxAudioEnableSpeaker, but for specific conference or 2-way call
     * "audioOutput1.ringerDevice"    same value as szDevice in sipxAudioSetRingerOutputDevice 
     * "audioOutput1.speakerDevice"   same values as szDevice in sipxAudioSetCallOutputDevice
     * "audioOutput1.volume"          string value of iLevel in sipxAudioSetVolume
     */
   virtual OsStatus setMediaProperty(const UtlString& propertyName,
                                     const UtlString& propertyValue) = 0;

   //! Get a media property on the media interface
   virtual OsStatus getMediaProperty(const UtlString& propertyName,
                                     UtlString& propertyValue) = 0;

   //! Set a media property associated with a connection
   virtual OsStatus setMediaProperty(int connectionId,
                                     const UtlString& propertyName,
                                     const UtlString& propertyValue) = 0;

   //! Get a media property associated with a connection
   virtual OsStatus getMediaProperty(int connectionId,
                                     const UtlString& propertyName,
                                     UtlString& propertyValue) = 0;

     ///< Get the specific type of this media interface
   virtual UtlString getType() = 0;



/* ============================ INQUIRY =================================== */

    /// Query if connectionId is valid
   virtual UtlBoolean isConnectionIdValid(int connectionId) = 0;

   //! Query whether the specified media connection is enabled for 
   //! sending RTP.
   virtual UtlBoolean isSendingRtpAudio(int connectionId) = 0 ;

   //! Query whether the specified media connection is enabled for 
   //! sending RTP.
   virtual UtlBoolean isSendingRtpVideo(int connectionId) = 0 ;

   //! Query whether the specified media connection is enabled for
   //! sending RTP.
   virtual UtlBoolean isReceivingRtpAudio(int connectionId) = 0 ;

   //! Query whether the specified media connection is enabled for
   //! sending RTP.
   virtual UtlBoolean isReceivingRtpVideo(int connectionId) = 0 ;

   //! Query whether a new party can be added to this media interfaces
   virtual UtlBoolean canAddParty() = 0 ;

   //! Query whether the connection has started sending or receiving video
   virtual UtlBoolean isVideoInitialized(int connectionId) = 0 ;

   //! Query whether the connection has started sending or receiving audio
   virtual UtlBoolean isAudioInitialized(int connectionId) = 0 ;

   //! Query if the audio device is available.
   virtual UtlBoolean isAudioAvailable() = 0;

   //! Query if we are mixing a video conference
   virtual UtlBoolean isVideoConferencing() = 0 ;

   //! Query if ICE is enabled for this connection
   virtual bool isIceEnabled(int connectionId) = 0 ;

   virtual void* const getAudioEnginePtr() = 0;
   virtual void* const getVideoEnginePtr() = 0;

};

/* ============================ INLINE METHODS ============================ */

#endif  // _IMediaInterface_h_