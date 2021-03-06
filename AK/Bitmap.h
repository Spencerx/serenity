#pragma once

#include "StdLibExtras.h"
#include "Types.h"
#include "kmalloc.h"
#include "Assertions.h"

namespace AK {

class Bitmap {
public:
    // NOTE: A wrapping Bitmap won't try to free the wrapped data.
    static Bitmap wrap(byte* data, int size)
    {
        return Bitmap(data, size);
    }

    static Bitmap create(int size, bool default_value = 0)
    {
        return Bitmap(size, default_value);
    }

    ~Bitmap()
    {
        if (m_owned)
            kfree(m_data);
        m_data = nullptr;
    }

    int size() const { return m_size; }
    bool get(int index) const
    {
        ASSERT(index < m_size);
        return 0 != (m_data[index / 8] & (1u << (index % 8)));
    }
    void set(int index, bool value) const
    {
        ASSERT(index < m_size);
        if (value)
            m_data[index / 8] |= static_cast<byte>((1u << (index % 8)));
        else
            m_data[index / 8] &= static_cast<byte>(~(1u << (index % 8)));
    }

    byte* data() { return m_data; }
    const byte* data() const { return m_data; }

private:
    explicit Bitmap(int size, bool default_value)
        : m_size(size)
        , m_owned(true)
    {
        ASSERT(m_size != 0);
        int size_to_allocate = ceil_div(size, 8);
        m_data = reinterpret_cast<byte*>(kmalloc(size_to_allocate));
        memset(m_data, default_value ? 0xff : 0x00, size_to_allocate);
    }

    Bitmap(byte* data, int size)
        : m_data(data)
        , m_size(size)
        , m_owned(false)
    {
    }

    byte* m_data { nullptr };
    int m_size { 0 };
    bool m_owned { false };
};

}

using AK::Bitmap;

