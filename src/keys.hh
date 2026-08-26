#ifndef keys_hh_INCLUDED
#define keys_hh_INCLUDED

#include "coord.hh"
#include "flags.hh"
#include "hash.hh"
#include "meta.hh"
#include "optional.hh"
#include "unicode.hh"
#include "vector.hh"

#include <cstdint>
#include <bit>

namespace Kakoune
{

struct Key
{
    enum class MouseButton
    {
        Left,
        Middle,
        Right
    };
    enum class Modifiers : uint32_t
    {
        None    = 0,
        Control = 1 << 0,
        Alt     = 1 << 1,
        Shift   = 1 << 2,

        MousePress     = 1 << 3,
        MouseRelease   = 1 << 4,
        MousePos       = 1 << 5,
        MouseButtonMask= 0b11u << 6,

        Scroll     = 1 << 8,
        Resize     = 1 << 9,
        MenuSelect = 1 << 10,

        VScrollMask = 0xFFu << 16,
        HScrollMask = 0xFFu << 24,
    };
    enum NamedKey : Codepoint
    {
        // use UTF-16 surrogate pairs range
        Backspace = 0xD800,
        Delete,
        Escape,
        Return,
        Up,
        Down,
        Left,
        Right,
        PageUp,
        PageDown,
        Home,
        End,
        Insert,
        Tab,
        Space,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        FocusIn,
        FocusOut,
        Invalid,
    };

    Modifiers modifiers = {};
    Codepoint key = {};

    constexpr Key(Modifiers modifiers, Codepoint key)
        : modifiers(modifiers), key(key) {}

    constexpr Key(Codepoint key)
        : modifiers(Modifiers::None), key(key) {}

    constexpr Key() = default;

    constexpr uint64_t val() const { return (uint64_t)modifiers << 32 | key; }

    constexpr bool operator==(Key other) const { return val() == other.val(); }
    constexpr auto operator<=>(Key other) const { return val() <=> other.val(); }

    constexpr DisplayCoord coord() const { return {(int)((uint32_t) (key & 0xFFFF0000) >> 16), (int)(key & 0x0000FFFF)}; }

    constexpr static Modifiers mouse_button_modifier(MouseButton button) {
        return Key::Modifiers{((uint32_t)button << 6) & (uint32_t)Modifiers::MouseButtonMask};
    }

    constexpr MouseButton mouse_button() {
        return (MouseButton)(((uint32_t)modifiers & (uint32_t)Modifiers::MouseButtonMask) >> 6);
    }

    constexpr static Modifiers scroll_modifier(int8_t hscroll, int8_t vscroll) {
        auto const hscroll_modifier = (uint32_t)std::bit_cast<uint8_t>(hscroll) << 24;
        auto const vscroll_modifier = (uint32_t)std::bit_cast<uint8_t>(vscroll) << 16;
        return Modifiers{hscroll_modifier | vscroll_modifier | (uint32_t)Modifiers::Scroll};
    }

    constexpr int8_t hscroll_amount() {
        uint8_t hscroll_modifier = ((uint32_t)modifiers & (uint32_t)Modifiers::HScrollMask) >> 24;
        return std::bit_cast<int8_t>(hscroll_modifier);
    }

    constexpr int8_t vscroll_amount() {
        uint8_t hscroll_modifier = ((uint32_t)modifiers & (uint32_t)Modifiers::VScrollMask) >> 16;
        return std::bit_cast<int8_t>(hscroll_modifier);
    }

    Optional<Codepoint> codepoint() const;
};

constexpr bool with_bit_ops(Meta::Type<Key::Modifiers>) { return true; }

using KeyList = Vector<Key, MemoryDomain::Mapping>;

class String;
class StringView;

KeyList parse_keys(StringView str);
String  to_string(Key key);
StringView to_string(Key::MouseButton button);
Key::MouseButton str_to_button(StringView str);

constexpr Key shift(Key key)
{
    return { key.modifiers | Key::Modifiers::Shift, key.key };
}
constexpr Key alt(Key key)
{
    return { key.modifiers | Key::Modifiers::Alt, key.key };
}
constexpr Key ctrl(Key key)
{
    return { key.modifiers | Key::Modifiers::Control, key.key };
}

constexpr Codepoint encode_coord(DisplayCoord coord) { return (Codepoint)(((int)coord.line << 16) | ((int)coord.column & 0x0000FFFF)); }

constexpr Key resize(DisplayCoord dim) { return { Key::Modifiers::Resize, encode_coord(dim) }; }

constexpr size_t hash_value(const Key& key) { return hash_values(key.modifiers, key.key); }

}

#endif // keys_hh_INCLUDED
