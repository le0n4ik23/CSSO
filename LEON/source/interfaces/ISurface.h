#pragma once
#include "../util/util.h"
#include "../util/color.h"

class IPanel
{
public:
    const char* GetName(int vguiPanel)
    {
        return util::get_vfunc<36, const char*>(this, vguiPanel);
    }

    void SetMouseInputEnabled(unsigned int panel, bool state)
    {
        util::get_vfunc<32, void>(this, panel, state);
    }
};


namespace vgui
{

    class IImage;
    class Image;
    class Point;

    // handles
    typedef unsigned long HCursor;
    typedef unsigned long HTexture;
    typedef unsigned long HFont;

    struct Vertex_t
    {
        Vertex_t(){}
        Vertex_t(const Vector2& pos, const Vector2& coord = Vector2(0, 0))
        {
            m_Position = pos;
            m_TexCoord = coord;
        }
        void Init(const Vector2& pos, const Vector2& coord = Vector2(0, 0))
        {
            m_Position = pos;
            m_TexCoord = coord;
        }

        Vector2    m_Position;
        Vector2    m_TexCoord;
    };


    enum FontDrawType_t
    {
        // Use the "additive" value from the scheme file
        FONT_DRAW_DEFAULT = 0,

        // Overrides
        FONT_DRAW_NONADDITIVE,
        FONT_DRAW_ADDITIVE,

        FONT_DRAW_TYPE_COUNT = 2,
    };

    enum FontFlags_t
    {
        FONTFLAG_NONE,
        FONTFLAG_ITALIC = 0x001,
        FONTFLAG_UNDERLINE = 0x002,
        FONTFLAG_STRIKEOUT = 0x004,
        FONTFLAG_SYMBOL = 0x008,
        FONTFLAG_ANTIALIAS = 0x010,
        FONTFLAG_GAUSSIANBLUR = 0x020,
        FONTFLAG_ROTARY = 0x040,
        FONTFLAG_DROPSHADOW = 0x080,
        FONTFLAG_ADDITIVE = 0x100,
        FONTFLAG_OUTLINE = 0x200,
        FONTFLAG_CUSTOM = 0x400,		// custom generated font - never fall back to asian compatibility mode
        FONTFLAG_BITMAP = 0x800,		// compiled bitmap font - no fallbacks
    };

    // Refactor these two
    struct CharRenderInfo
    {
        // Text pos
        int                x, y;
        // Top left and bottom right
        // This is now a pointer to an array maintained by the surface, to avoid copying the data on the 360
        Vertex_t* verts;
        int                textureId;
        int                abcA;
        int                abcB;
        int                abcC;
        int                fontTall;
        HFont            currentFont;
        // In:
        FontDrawType_t    drawType;
        wchar_t            ch;

        // Out
        bool            valid;
        // In/Out (true by default)
        bool            shouldclip;
    };


    struct IntRect
    {
        int x0;
        int y0;
        int x1;
        int y1;
    };

    // Partly taken from Osiris
    class ISurface
    {
    public:
        static constexpr unsigned font{ 0x1d }; // builtin font from vgui_spew_fonts


        virtual void* un0() = 0;
        virtual void* un1() = 0;
        virtual void* un2() = 0;
        virtual void* un3() = 0;
        virtual void* un4() = 0;
        virtual void* un5() = 0;
        virtual void* un6() = 0;
        virtual void* un7() = 0;
        virtual void* un8() = 0;
        virtual void* un9() = 0;
        virtual void* un10() = 0;
        virtual void* un11() = 0;
        virtual void* un12() = 0;
        virtual void* un13() = 0;
        virtual void* un14() = 0;
        virtual void* un15() = 0;
        virtual void* un16() = 0;
        virtual void* un17() = 0;
        virtual void* un18() = 0;
        virtual void* un19() = 0;
        virtual void* un20() = 0;
        virtual void* un21() = 0;
        virtual void* un22() = 0;
        virtual void* un23() = 0;
        virtual void* un24() = 0;
        virtual void* un25() = 0;
        virtual void* un26() = 0;
        virtual void* un27() = 0;
        virtual void* un28() = 0;
        virtual void* un29() = 0;
        virtual void* un30() = 0;
        virtual void* un31() = 0;
        virtual void* un32() = 0;
        virtual void* un33() = 0;
        virtual void* un34() = 0;
        virtual void* un35() = 0;
        virtual void* un36() = 0;
        virtual void* un37() = 0;
        virtual void* un38() = 0;
        virtual void* un39() = 0;
        virtual void* un40() = 0;
        virtual void* un41() = 0;
        virtual void* un42() = 0;
        virtual void* un43() = 0;
        virtual void* un44() = 0;
        virtual void* un45() = 0;
        virtual void* un46() = 0;
        virtual void* un47() = 0;
        virtual void* un48() = 0;
        virtual void* un49() = 0;
        virtual void* un50() = 0;
        virtual void* un51() = 0;
        virtual void* un52() = 0;
        virtual void* un53() = 0;
        virtual void* un54() = 0;
        virtual void* un55() = 0;
        virtual void* un56() = 0;
        virtual void* un57() = 0;
        virtual void* un58() = 0;
        virtual void* un59() = 0;
        virtual void* un60() = 0;
        virtual void* un61() = 0;
        virtual void* un62() = 0;
        virtual void* un63() = 0;
        virtual void* un64() = 0;
        virtual void* un65() = 0;
        virtual void* un66() = 0;
        virtual void* un67() = 0;
        virtual void* un68() = 0;
        virtual void* un69() = 0;
        virtual void* un70() = 0;
        virtual void* un71() = 0;
        virtual void* un72() = 0;
        virtual void* un73() = 0;
        virtual void* un74() = 0;
    
        virtual void GetTextSize(DWORD font, const wchar_t* text, int& wide, int& tall) = 0;

        void UnlockCursor()
        {
            util::get_vfunc<61, void>(this);
        }

        void LockCursor()
        {
            util::get_vfunc<62, void>(this);
        }

        void SetDrawColor(int r, int g, int b, int a = 255)
        {
            util::get_vfunc<11, void>(this, r, g, b, a);
        }
        //void SetDrawColor(Color color)
        //{
        //    GetVirtualFunction< void(__thiscall*)(void*, int, int, int, int) >(this, 15)(this, color.r(), color.g(), color.b(), color.a());
        //}
        void DrawOutlinedRect(int x0, int y0, int x1, int y1)
        {
            util::get_vfunc<14, void>(this, x0, y0, x1, y1);
        }
        void DrawFilledRect(int x0, int y0, int x1, int y1)
        {
            util::get_vfunc<12, void>(this, x0, y0, x1, y1);
        }
        void DrawLine(int x0, int y0, int x1, int y1)
        {
            util::get_vfunc<15, void>(this, x0, y0, x1, y1);
        }
        void DrawOutlinedCircle(int x, int y, int r, int seg)
        {
            util::get_vfunc<103, void>(this, x, y, r, seg);
        }
        void SetTextFont(HFont font)
        {
            util::get_vfunc<17, void>(this, font);
        }
        void SetTextColor(int r, int g, int b, int a)
        {
            util::get_vfunc<19, void>(this, r, g, b, a);
        }

        void SetTextColor(Color col)
        {
            
            util::get_vfunc<19, void>(this, col.iR, col.iG, col.iB, col.iA);
        }

        void SetTextPosition(int x, int y)
        {
            util::get_vfunc<20, void>(this, x, y);
        }
        unsigned CreateFontS()
        {
            return util::get_vfunc<66, unsigned>(this);
        }
        bool SetFontGlyphSet(HFont font, const char* fontName, int size, int weight, int blur, int scanlines, int flags, int rangeMin = 0, int rangeMax = 0)
        {
            return util::get_vfunc<67, bool>(this, font, fontName, size, weight, blur, scanlines, flags, rangeMin, rangeMax);
        }
        void PrintText(std::wstring_view text, int drawType = 0)
        {
            util::get_vfunc<22, void>(this, text.data(), text.length(), drawType);
        }





        void SetupFont(HFont& font, const char* fontName, int size, int weight, int blur, int scanline, int flags)
        {
            if (!font)
            {
                font = this->CreateFontS();
                this->SetFontGlyphSet(font, fontName, size, weight, blur, scanline, flags);
            }
            else
            {
                this->SetFontGlyphSet(font, fontName, size, weight, blur, scanline, flags);
            }
            }
            void DrawTextSurface(HFont font, Vector3 screen, const char* text, Color col)
            {
                this->SetTextFont(font);
                this->SetTextColor(col);
                this->SetTextPosition((int)screen.x, (int)screen.y);
                this->PrintText(util::ansi2unicode(text));
            }


        enum ETextureFormat
        {
            eTextureFormat_RGBA,
            eTextureFormat_BGRA,
            eTextureFormat_BGRA_Opaque, // bgra format but alpha is always 255, CEF does this, we can use this fact for better perf on win32 gdi
        };

        // returns true if the surface supports minimize & maximize capabilities
        enum SurfaceFeature_e
        {
            ANTIALIASED_FONTS = 1,
            DROPSHADOW_FONTS = 2,
            ESCAPE_KEY = 3,
            OPENING_NEW_HTML_WINDOWS = 4,
            FRAME_MINIMIZE_MAXIMIZE = 5,
            OUTLINE_FONTS = 6,
            DIRECT_HWND_RENDER = 7,
        };

        // fonts
        // creates an empty handle to a vgui font.  windows fonts can be add to this via SetFontGlyphSet().
        virtual HFont CreateFont() = 0;

        // adds to the font
        enum EFontFlags
        {
            FONTFLAG_NONE,
            FONTFLAG_ITALIC = 0x001,
            FONTFLAG_UNDERLINE = 0x002,
            FONTFLAG_STRIKEOUT = 0x004,
            FONTFLAG_SYMBOL = 0x008,
            FONTFLAG_ANTIALIAS = 0x010,
            FONTFLAG_GAUSSIANBLUR = 0x020,
            FONTFLAG_ROTARY = 0x040,
            FONTFLAG_DROPSHADOW = 0x080,
            FONTFLAG_ADDITIVE = 0x100,
            FONTFLAG_OUTLINE = 0x200,
            FONTFLAG_CUSTOM = 0x400,        // custom generated font - never fall back to asian compatibility mode
            FONTFLAG_BITMAP = 0x800,        // compiled bitmap font - no fallbacks
        };


        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // !! WARNING! YOU MUST NOT ADD YOUR NEW METHOD HERE OR YOU WILL BREAK MODS !!
        // !! Add your new stuff to the bottom of IMatSystemSurface instead.        !!
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    };
}