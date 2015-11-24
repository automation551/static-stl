/******************************************************************************
The MIT License(MIT)

Embedded Template Library.
https://github.com/ETLCPP/etl

Copyright(c) 2015 jwellbelove

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

#include <UnitTest++/UnitTest++.h>

#include <iterator>
#include <string>
#include <vector>
#include <iomanip>
#include <stdint.h>

#include <sstl/pearson.h>

const size_t HASH_SIZE = 8;
typedef sstl::pearson<HASH_SIZE>::value_type hash_t;

//***************************************************************************
/// Pearson lookup table
/// \ingroup pearson
//***************************************************************************
namespace sstl
{
  extern const uint8_t PEARSON_LOOKUP[];
}

//***************************************************************************
// Comparison calculator based on Wikipedia example. https://en.wikipedia.org/wiki/Pearson_hashing
//***************************************************************************
template <typename TContainer>
hash_t Pearson_Compare(const TContainer& data)
{
  hash_t hash;
  hash.fill(0);

  for (size_t j = 0; j < HASH_SIZE; ++j)
  {
    uint8_t subhash = sstl::PEARSON_LOOKUP[(data[0] + j) % 256];

    for (size_t i = 1; i < data.size(); ++i)
    {
      subhash = sstl::PEARSON_LOOKUP[subhash ^ data[i]];
    }

    hash[j] = subhash;
  }

  return hash;
}

namespace sstl
{
namespace test
{
  SUITE(test_pearson)
  {
    //*************************************************************************
    TEST(test_pearson_constructor)
    {
      std::string data("123456789");

      hash_t compare = Pearson_Compare(data);
      hash_t hash    = sstl::pearson<HASH_SIZE>(data.begin(), data.end());

      CHECK(compare == hash);
    }

    //*************************************************************************
    TEST(test_pearson_add_values)
    {
      std::string data("123456789");

      sstl::pearson<HASH_SIZE> pearson_calculator;

      for (size_t i = 0; i < data.size(); ++i)
      {
        pearson_calculator.add(data[i]);
      }

      hash_t compare = Pearson_Compare(data);
      hash_t hash    = pearson_calculator;

      CHECK(compare == hash);
    }

    //*************************************************************************
    TEST(test_pearson_add_range)
    {
      std::string data("123456789");

      sstl::pearson<HASH_SIZE> pearson_calculator;

      pearson_calculator.add(data.begin(), data.end());

      hash_t compare = Pearson_Compare(data);
      hash_t hash    = pearson_calculator.value();

      CHECK(compare == hash);
    }

    //*************************************************************************
    TEST(test_pearson_add_range_endian)
    {
      std::vector<uint8_t>  data1 = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
      std::vector<uint32_t> data2 = { 0x04030201, 0x08070605 };
      std::vector<uint8_t>  data3 = { 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01 };

      hash_t hash1 = sstl::pearson<HASH_SIZE>(data1.begin(), data1.end());
      hash_t hash2 = sstl::pearson<HASH_SIZE>((uint8_t*)&data2[0], (uint8_t*)(&data2[0] + data2.size()));
      CHECK(hash1 == hash2);

      hash_t hash3 = sstl::pearson<HASH_SIZE>(data3.rbegin(), data3.rend());
      CHECK(hash1 == hash3);
    }
  };
}
}
