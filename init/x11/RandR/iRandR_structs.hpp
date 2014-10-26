/*!
 * \file x11/RandR/iRandR_structs.hpp
 * \brief \b Structs: \a internal::_crtc, \a internal::_outpu , \a internal::_mode , \a internal::_config
 */

#ifndef E_RANDR_STRUCTS_HPP
#define E_RANDR_STRUCTS_HPP

#include <X11/extensions/Xrandr.h>
#include <vector>
#include <list>
#include <string>

namespace e_engine {
namespace internal {   

/*!
 * \struct e_engine::internal::_crtc
 * \brief \c [Xlib::XRandR] stores information for a CRTC
 * 
 * \note The description is a copy and paste from http://cgit.freedesktop.org/xorg/proto/randrproto/tree/randrproto.txt
 * 
 * \sa iRandR ; _mode _output
 */
struct _crtc {
   RRCrtc                 id;                   //!< The unique \b CRTC id form Xlib

   Time                   timestamp;            //!< [not used]; 'timestamp' indicates when the configuration was last set.
   int                    posX;                 //!< 'posX' and 'posY' indicate the position of this CRTC within the screen region. They will be set to 0 when the CRTC is disabled.
   int                    posY;                 //!< 'posX' and 'posY' indicate the position of this CRTC within the screen region. They will be set to 0 when the CRTC is disabled.
   unsigned int           width;                //!< 'width' and 'height' indicate the size of the area within the screen presented by this CRTC. This may be different than the size of the mode due to rotation, the projective transform, and the Border property described below.  They will be set to 0 when the CRTC is disabled.
   unsigned int           height;               //!< 'width' and 'height' indicate the size of the area within the screen presented by this CRTC. This may be different than the size of the mode due to rotation, the projective transform, and the Border property described below.  They will be set to 0 when the CRTC is disabled.
   RRMode                 mode;                 //!< 'mode' indicates which mode is active, or None indicating that the CRTC has been disabled and is not displaying the screen contents.
   Rotation               rotation;             //!< 'rotation' indicates the active rotation. It is set to Rotate_0 when the CRTC is disabled.
   Rotation               rotations;            //!< 'rotations' contains the set of rotations and reflections supported by the CRTC.
   std::vector<RROutput>  outputs;              //!< 'outputs' is the list of outputs currently connected to this CRTC and is empty when the CRTC is disabled.
   std::vector<RROutput>  possibleOutputs;      //!< 'possibleOutputs' lists all of the outputs which may be connected to this CRTC.
};

/*!
 * \struct e_engine::internal::_output
 * \brief \c [Xlib::XRandR] stores information for a output (= display)
 * 
 * \note The description is a copy and paste from http://cgit.freedesktop.org/xorg/proto/randrproto/tree/randrproto.txt
 * 
 * \sa iRandR ; _crtc _mode
 */
struct _output {
   RROutput               id;                   //!< The unique \b CRTC id form Xlib

   Time                   timestamp;            //!< [not used]; 'timestamp' indicates when the configuration was last set.
   RRCrtc                 crtc;                 //!< 'crtc' is the current source CRTC for video data, or Disabled ( = None ) if the output is not connected to any CRTC.
   std::string            name;                 //!< 'name' is a UTF-8 encoded string designed to be presented to the user to indicate which output this is. E.g. "S-Video" or "DVI".
   unsigned long          mm_width;             //!< 'widthInMillimeters' and 'heightInMillimeters' report the physical size of the displayed area. If unknown, or not really fixed (e.g., for a projector), these values are both zero.
   unsigned long          mm_height;            //!< 'widthInMillimeters' and 'heightInMillimeters' report the physical size of the displayed area. If unknown, or not really fixed (e.g., for a projector), these values are both zero.
   Connection             connection;           //!< 'connection' indicates whether the hardware was able to detect a device connected to this output. If the hardware cannot determine whether something is connected, it will set this to UnknownConnection. ==> 0 - Connected; 1 - Disconnected; 2 - Unknown
   SubpixelOrder          subpixel_order;       //!< 'subpixel_order' contains the resulting subpixel order of the connected device to allow correct subpixel rendering.
   std::vector<RRCrtc>    crtcs;                //!< 'crtcs' is the list of CRTCs that this output may be connected to. Attempting to connect this output to a different CRTC results in a Match error.
   std::vector<RROutput>  clones;               //!< 'clones' is the list of outputs which may be simultaneously connected to the same CRTC along with this output. Attempting to connect this output with an output not in the 'clones' list results in a Match error.
   std::vector<RRMode>    modes;                //!< 'modes' is the list of modes supported by this output. Attempting to connect this output to a CRTC not using one of these modes results in a Match error.
   int                    npreferred;           //!< The first 'npreferred' modes in 'modes' are preferred by the monitor in some way; for fixed-pixel devices, this would generally indicate which modes match the resolution of the output device.
};

/*!
 * \struct e_engine::internal::_mode
 * \brief \c [Xlib::XRandR] stores information for a mode for a CRTC 
 * 
 * An output mode specifies the complete CRTC timings for
 * a specific mode. The vertical and horizontal synchronization rates
 * can be computed given the dot clock and the h total/v total
 * values. If the dot clock is zero, then all of the timing
 * parameters and flags are not used, and must be zero as this
 * indicates that the timings are unknown or otherwise unused.
 * The name itself will be encoded separately in each usage.
 * 
 * \note The description is a copy and paste from http://cgit.freedesktop.org/xorg/proto/randrproto/tree/randrproto.txt
 * 
 * \sa iRandR ; _crtc _output
 */
struct _mode {
   RRMode                 id;                   //!< The unique \b mode id

   unsigned int           width;                //!< The width of the mode
   unsigned int           height;               //!< The height of the mode
   unsigned long          dotClock;             //!< The dotClock (more information: https://de.wikipedia.org/wiki/Pixeltakt)
   unsigned int           hSyncStart;           //!< This data is needed for calculating \a refresh and \a syncFreq
   unsigned int           hSyncEnd;             //!< This data is needed for calculating \a refresh and \a syncFreq
   unsigned int           hTotal;               //!< This data is needed for calculating \a refresh and \a syncFreq
   unsigned int           hSkew;                //!< This data is needed for calculating \a refresh and \a syncFreq
   unsigned int           vSyncStart;           //!< This data is needed for calculating \a refresh and \a syncFreq
   unsigned int           vSyncEnd;             //!< This data is needed for calculating \a refresh and \a syncFreq
   unsigned int           vTotal;               //!< This data is needed for calculating \a refresh and \a syncFreq
   std::string            name;                 //!< The name of the mode
   XRRModeFlags           modeFlags;            //!< Some mode flags

   double                 refresh;              //!< The refresh rate
   double                 syncFreq;             //!< The sync frequnece

   /*!
    * \brief Only used for the STL list method unique();
    * 
    * \returns Whether the objects are equal or not.
    */
   inline bool operator==( const _mode &_m ) const {
      if ( _m.id == id && _m.height == height && _m.width == width && _m.refresh == refresh && _m.syncFreq == syncFreq && _m.name == name )
         return true;

      return false;
   }

   /*!
    * \brief Only used for the STL list method sort();
    * 
    * \returns Whether the object is greater than the other.
    */
   inline bool operator<( const _mode &_m ) const {
      if ( width > _m.width )
         return true;
      else if ( width < _m.width )
         return false;

      if ( height > _m.height )
         return true;
      else if ( height < _m.height )
         return false;

      if ( refresh < _m.refresh )
         return true;
      else if ( refresh > _m.refresh )
         return false;

      return true;
   }
};

/*!
 * \struct e_engine::internal::_config
 * \brief Stores all necessary infomation to restore a specific RandR configuration
 */
struct _config {
   RROutput                    primary;   //!< The primary output

   std::vector<XRRCrtcGamma *> gamma;     //!< Holds gamma information for each CRTC
   std::vector<_crtc>          CRTCInfo;  //!< All important data to restore every CRTC
};

}
}

#endif // E_RANDR_STRUCTS_HPP


