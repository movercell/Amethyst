#pragma once

#include <memory>

//all if the glfw window macros
#define 	WINDOW_FOCUSED   0x00020001
#define 	WINDOW_ICONIFIED   0x00020002
#define 	WINDOW_RESIZABLE   0x00020003
#define 	WINDOW_VISIBLE   0x00020004
#define 	WINDOW_DECORATED   0x00020005
#define 	WINDOW_AUTO_ICONIFY   0x00020006
#define 	WINDOW_FLOATING   0x00020007
#define 	WINDOW_MAXIMIZED   0x00020008
#define 	WINDOW_CENTER_CURSOR   0x00020009
#define 	WINDOW_TRANSPARENT_FRAMEBUFFER   0x0002000A
#define 	WINDOW_HOVERED   0x0002000B
#define 	WINDOW_FOCUS_ON_SHOW   0x0002000C
#define 	WINDOW_RED_BITS   0x00021001
#define 	WINDOW_GREEN_BITS   0x00021002
#define 	WINDOW_BLUE_BITS   0x00021003
#define 	WINDOW_ALPHA_BITS   0x00021004
#define 	WINDOW_DEPTH_BITS   0x00021005
#define 	WINDOW_STENCIL_BITS   0x00021006
#define 	WINDOW_ACCUM_RED_BITS   0x00021007
#define 	WINDOW_ACCUM_GREEN_BITS   0x00021008
#define 	WINDOW_ACCUM_BLUE_BITS   0x00021009
#define 	WINDOW_ACCUM_ALPHA_BITS   0x0002100A
#define 	WINDOW_AUX_BUFFERS   0x0002100B
#define 	WINDOW_STEREO   0x0002100C
#define 	WINDOW_SAMPLES   0x0002100D
#define 	WINDOW_SRGB_CAPABLE   0x0002100E
#define 	WINDOW_REFRESH_RATE   0x0002100F
#define 	WINDOW_DOUBLEBUFFER   0x00021010
#define 	WINDOW_CLIENT_API   0x00022001
#define 	WINDOW_CONTEXT_VERSION_MAJOR   0x00022002
#define 	WINDOW_CONTEXT_VERSION_MINOR   0x00022003
#define 	WINDOW_CONTEXT_REVISION   0x00022004
#define 	WINDOW_CONTEXT_ROBUSTNESS   0x00022005
#define 	WINDOW_OPENGL_FORWARD_COMPAT   0x00022006
#define 	WINDOW_OPENGL_DEBUG_CONTEXT   0x00022007
#define 	WINDOW_OPENGL_PROFILE   0x00022008
#define 	WINDOW_CONTEXT_RELEASE_BEHAVIOR   0x00022009
#define 	WINDOW_CONTEXT_NO_ERROR   0x0002200A
#define 	WINDOW_CONTEXT_CREATION_API   0x0002200B
#define 	WINDOW_SCALE_TO_MONITOR   0x0002200C
#define 	WINDOW_COCOA_RETINA_FRAMEBUFFER   0x00023001
#define 	WINDOW_COCOA_FRAME_NAME   0x00023002
#define 	WINDOW_COCOA_GRAPHICS_SWITCHING   0x00023003
#define 	WINDOW_X11_CLASS_NAME   0x00024001

struct ____WindowData;

class Context;

/*!
*   \brief A window.
*/
class Window {
public:
    virtual ~Window();
    //! Re-creates the system window with the current parameters of this object.
    void Update();

protected:
    //!@private
    std::shared_ptr<Context> contextRef = nullptr;
    //!@private
    ____WindowData* contextData = nullptr;
    //!@private
    ____WindowData* data = nullptr;    
    //!@private
    Window() {}; //the compiler compains if this doesn't exist
};
