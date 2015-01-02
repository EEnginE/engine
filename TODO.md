```
     _____     ____
    |_   _|__ |  _ \  ___
      | |/ _ \| | | |/ _ \
      | | (_) | |_| | (_) |
      |_|\___/|____/ \___/
'
```

# TODO Debian/Ubuntu gcc compatibility

## Descripton:

Linking error with gcc(!) under Debian/Ubuntu. Maybe a bug in GCC? Are the compiler options correct?
Works with llvm under Debian!

### Error code Debian shared:

    ../../lib/libengine.so.0.1: undefined reference to `e_engine::eCMDColor::OFF`
    ../../lib/libengine.so.0.1: undefined reference to `e_engine::eCMDColor::BOLD`
    collect2: error: ld returned 1 exit status

### Error code Ubuntu shared:

    Linking CXX executable ../../bin/engine_test1
    ../../lib/libengine.so.0.1: undefined reference to `e_engine::eCMDColor::OFF`
    ../../lib/libengine.so.0.1: undefined reference to `e_engine::eCMDColor::BOLD`
    collect2: ld gab 1 als Ende-Status zurück

### Error code Ubuntu static:

    Linking CXX executable ../../bin/engine_test1
    ../../lib/libengine.so.0.1: undefined reference to `e_engine::eCMDColor::OFF`
    ../../lib/libengine.so.0.1: undefined reference to `e_engine::eCMDColor::BOLD`
    collect2: ld gab 1 als Ende-Status zurück

## Editor:

Free

# TODO Clean Dependencies

## Description:

Maybe link some libraries static or provide shared objects of GLEW/Boost(?) and so on.

## Editor:

Free

# TODO Comment the Code

## Description:

Lots of functions are not or too less commented.

## Editor:

Free

# TODO List all CMake Options in README

## Description:

Lots of CMake options are undocumented.

## Editor:

Silas

# DONE Convert README and ToDo

## Description:

Convert README and ToDo in a Markdown.

## Editor:

Silas

# TODO Fix code warnings

    [init/basicClasses/iDisplayBasic.hpp:30]: (style) struct or union member 'res::width' is never used
    [init/basicClasses/iDisplayBasic.hpp:31]: (style) struct or union member 'res::height' is never used
    [init/basicClasses/iDisplayBasic.hpp:37]: (style) struct or union member 'mode_basic::prefered' is never used
    [init/basicClasses/iDisplayBasic.hpp:39]: (style) struct or union member 'mode_basic::width' is never used
    [init/basicClasses/iDisplayBasic.hpp:40]: (style) struct or union member 'mode_basic::height' is never used
    [init/basicClasses/iDisplayBasic.hpp:41]: (style) struct or union member 'mode_basic::rate' is never used
    [init/iEventInfo.hpp:26]: (style) struct or union member 'iEventInfo::type' is never used
    [init/windows/iDisplays.cpp:85]: (style) The scope of the variable 'lMinDiffToPref_D' can be reduced
    [init/windows/iDisplays.cpp:91]: (style) The scope of the variable 'lRatePref_D' can be reduced
    [init/x11/RandR/iRandR_structs.hpp:105] -> [init/x11/RandR/iRandR_structs.hpp:105]: (style) Same expression on both sides of '=='.
    [init/x11/RandR/iRandR_structs.hpp:29]: (style) struct or union member '_crtc::posX' is never used
    [init/x11/RandR/iRandR_structs.hpp:30]: (style) struct or union member '_crtc::posY' is never used
    [init/x11/RandR/iRandR_structs.hpp:54]: (style) struct or union member '_output::mm_width' is never used
    [init/x11/RandR/iRandR_structs.hpp:55]: (style) struct or union member '_output::mm_height' is never used
    [init/x11/RandR/iRandR_structs.hpp:61]: (style) struct or union member '_output::npreferred' is never used
    [init/x11/iContext.cpp:311]: (style) struct or union member 'MwmHints::functions' is never used
    [init/x11/iContext.cpp:313]: (style) struct or union member 'MwmHints::input_mode' is never used
    [init/x11/iContext.cpp:314]: (style) struct or union member 'MwmHints::status' is never used
    [render/rNormalObject.cpp:19]: (warning) Member variable 'rNormalObject::vNeedUpdateMatrix_B' is not initialized in the constructor.
    [render/shader/rShader_structs.hpp:14]: (style) struct or union member 'atributeObject::index' is never used
    [utils/log/uLog.cpp:319]: (style) Unused variable: lErrorType_STR
    [utils/log/uLog_resources.hpp:190]: (warning) Member variable '__DATA_RAW__::vBasicColor_C' is not initialized in the constructor.
    [utils/log/uLog_resources.hpp:190]: (warning) Member variable '__DATA_RAW__::vBold_B' is not initialized in the constructor.
    [utils/log/uLog_resources.hpp:207]: (warning) Member variable '__DATA_CONF__::vColumns_uI' is not initialized in the constructor.
    [utils/log/uLog.cpp:333]: (style) Exception should be caught by reference.
    [utils/sha/uSHA_2_selftest.cpp:62]: (style) Unused variable: lResult_str
    [utils/uConfig.cpp:42]: (warning) Member variable '__uConfig_FBA::FBA_DRAW_TO_WINDOW' is not initialized in the constructor.
    [utils/uConfig.cpp:42]: (warning) Member variable '__uConfig_FBA::FBA_ACCELERATION' is not initialized in the constructor.
    [utils/uConfig.cpp:42]: (warning) Member variable '__uConfig_FBA::FBA_OGL_SUPPORTED' is not initialized in the constructor.