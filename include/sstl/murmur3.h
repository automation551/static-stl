///\file

/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl

Copyright(c) 2014 jwellbelove

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef _SSTL_MURMUR3__
#define _SSTL_MURMUR3__

#include <cstdint>
#include <type_traits>

#include "sstl_assert.h"
#include "__internal/binary.h"

#if defined(COMPILER_KEIL)
#pragma diag_suppress 1300
#endif

///\defgroup murmur3 Murmur3 hash calculations
///\ingroup maths

namespace sstl
{
  //***************************************************************************
  /// Calculates the murmur3 hash.
  /// See https://en.wikipedia.org/wiki/MurmurHash for more details.
  ///\tparam ENDIANNESS The endianness of the calculation for input types larger than uint8_t. Default = endian::little.
  ///\ingroup murmur3
  //***************************************************************************
  template <typename THash>
  class murmur3
  {
  public:

    static_assert((std::is_same<THash, uint32_t>::value || std::is_same<THash, uint64_t>::value), "Only 32 & 64 bit types supported");

    typedef THash value_type;

    //*************************************************************************
    /// Default constructor.
    /// \param seed The seed value. Default = 0.
    //*************************************************************************
    murmur3(value_type seed = 0)
      : seed(seed)
    {
      reset();
    }

    //*************************************************************************
    /// Constructor from range.
    /// \param begin Start of the range.
    /// \param end   End of the range.
    /// \param seed  The seed value. Default = 0.
    //*************************************************************************
    template<typename TIterator>
    murmur3(TIterator begin, const TIterator end, value_type seed = 0)
      : seed(seed)
    {
      static_assert(sizeof(typename std::iterator_traits<TIterator>::value_type) == 1, "Incompatible type");

      reset();
      while (begin != end)
      {
        block |= (*begin++) << (block_fill_count * 8);

        if (++block_fill_count == FULL_BLOCK)
        {
          add_block();
          block_fill_count = 0;
          block = 0;
        }

        ++char_count;
      }
    }

    //*************************************************************************
    /// Resets the hash to the initial state.
    //*************************************************************************
    void reset()
    {
      hash             = seed;
      char_count       = 0;
      block            = 0;
      block_fill_count = 0;
      is_finalised     = false;
    }

    //*************************************************************************
    /// Adds a range.
    /// \param begin
    /// \param end
    //*************************************************************************
    template<typename TIterator>
    void add(TIterator begin, const TIterator end)
    {
      static_assert(sizeof(typename std::iterator_traits<TIterator>::value_type) == 1, "Incompatible type");
      sstl_assert(!is_finalised);

      while (begin != end)
      {
         block |= (*begin++) << (block_fill_count * 8);
         if (++block_fill_count == FULL_BLOCK)
         {
            add_block();
            block_fill_count = 0;
            block = 0;
         }
         ++char_count;
      }
    }

    //*************************************************************************
    /// Adds a uint8_t value.
    /// If the hash has already been finalised then a 'hash_finalised' error will be emitted.
    /// \param value The char to add to the hash.
    //*************************************************************************
    void add(uint8_t value)
    {
      sstl_assert(!is_finalised);

      block |= value << (block_fill_count * 8);
      if (++block_fill_count == FULL_BLOCK)
      {
         add_block();
         block_fill_count = 0;
         block = 0;
      }
      ++char_count;
    }

    //*************************************************************************
    /// Gets the hash value.
    //*************************************************************************
    value_type value()
    {
      finalise();
      return hash;
    }

    //*************************************************************************
    /// Conversion operator to value_type.
    //*************************************************************************
    operator value_type ()
    {
      return value();
    }

  private:

    //*************************************************************************
    /// Adds a filled block to the hash.
    //*************************************************************************
    void add_block()
    {
      block *= CONSTANT1;
      block = rotate_left(block, SHIFT1);
      block *= CONSTANT2;

      hash ^= block;
      hash = rotate_left(hash, SHIFT2);
      hash = (hash * MULTIPLY) + ADD;
    }

    //*************************************************************************
    /// Finalises the hash.
    //*************************************************************************
    void finalise()
    {
      if (!is_finalised)
      {
        block *= CONSTANT1;
        block = rotate_left(block, SHIFT1);
        block *= CONSTANT2;

        hash ^= block;
        hash ^= char_count;
        hash ^= (hash >> 16);
        hash *= 0x85EBCA6B;
        hash ^= (hash >> 13);
        hash *= 0xC2B2AE35;
        hash ^= (hash >> 16);

        is_finalised = true;
      }
    }

    bool       is_finalised;
    uint8_t    block_fill_count;
    size_t     char_count;
    value_type block;
    value_type hash;
    value_type seed;

    static const uint8_t    FULL_BLOCK = 4;
    static const value_type CONSTANT1  = 0xCC9E2D51;
    static const value_type CONSTANT2  = 0x1B873593;
    static const value_type SHIFT1     = 15;
    static const value_type SHIFT2     = 13;
    static const value_type MULTIPLY   = 5;
    static const value_type ADD        = 0xE6546B64;
  };
}

#endif
