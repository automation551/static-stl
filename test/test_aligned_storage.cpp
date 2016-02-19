/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#include <catch.hpp>
#include <cstddef>
#include <type_traits>
#include <memory>
#include <array>

#include <sstl/__internal/_aligned_storage.h>

namespace sstl
{
namespace test
{

template<class TIter>
void check_alignment(TIter begin, TIter end, size_t alignment_requirement)
{
   for(auto it = begin; it!=end; ++it)
   {
      auto& value = *it;
      size_t address = reinterpret_cast<size_t>(std::addressof(value));
      REQUIRE( (address % alignment_requirement == 0) );
   }
}

TEST_CASE("aligned_storage")
{
   SECTION("size")
   {
      static_assert(sizeof(_aligned_storage<1, 1>::type) == 1, "unexpected size");
      static_assert(sizeof(_aligned_storage<1, 2>::type) == 2, "unexpected size");
      static_assert(sizeof(_aligned_storage<1, 4>::type) == 4, "unexpected size");

      static_assert(sizeof(_aligned_storage<16, 16>::type) == 16, "unexpected size");
      static_assert(sizeof(_aligned_storage<16, 32>::type) == 32, "unexpected size");
      static_assert(sizeof(_aligned_storage<16, 64>::type) == 64, "unexpected size");
   }

   SECTION("result of std::alignment_of")
   {
      static_assert(std::alignment_of<_aligned_storage<1, 1>::type>::value == 1, "unexpected alignment");
      static_assert(std::alignment_of<_aligned_storage<1, 2>::type>::value == 2, "unexpected alignment");
      static_assert(std::alignment_of<_aligned_storage<1, 4>::type>::value == 4, "unexpected alignment");

      static_assert(std::alignment_of<_aligned_storage<16, 16>::type>::value == 16, "unexpected alignment");
      static_assert(std::alignment_of<_aligned_storage<16, 32>::type>::value == 32, "unexpected alignment");
      static_assert(std::alignment_of<_aligned_storage<16, 64>::type>::value == 64, "unexpected alignment");
   }

   SECTION("alignment on stack")
   {
      SECTION("size 1")
      {
         {
            std::array<_aligned_storage<1, 1>::type, 10> aligned_values;
            check_alignment(aligned_values.cbegin(), aligned_values.cend(), 1);
         }
         {
            std::array<_aligned_storage<1, 2>::type, 10> aligned_values;
            check_alignment(aligned_values.cbegin(), aligned_values.cend(), 1);
         }
         {
            std::array<_aligned_storage<1, 4>::type, 10> aligned_values;
            check_alignment(aligned_values.cbegin(), aligned_values.cend(), 1);
         }
      }
      SECTION("size 16")
      {
         {
            std::array<_aligned_storage<16, 16>::type, 10> aligned_values;
            check_alignment(aligned_values.cbegin(), aligned_values.cend(), 1);
         }
         {
            std::array<_aligned_storage<16, 32>::type, 10> aligned_values;
            check_alignment(aligned_values.cbegin(), aligned_values.cend(), 1);
         }
         {
            std::array<_aligned_storage<16, 64>::type, 10> aligned_values;
            check_alignment(aligned_values.cbegin(), aligned_values.cend(), 1);
         }
      }
   }

   SECTION("alignment in static section")
   {
      SECTION("size 1")
      {
         {
            static std::array<_aligned_storage<1, 1>::type, 10> aligned_values;
            check_alignment(aligned_values.cbegin(), aligned_values.cend(), 1);
         }
         {
            static std::array<_aligned_storage<1, 2>::type, 10> aligned_values;
            check_alignment(aligned_values.cbegin(), aligned_values.cend(), 1);
         }
         {
            static std::array<_aligned_storage<1, 4>::type, 10> aligned_values;
            check_alignment(aligned_values.cbegin(), aligned_values.cend(), 1);
         }
      }
      SECTION("size 16")
      {
         {
            static std::array<_aligned_storage<16, 16>::type, 10> aligned_values;
            check_alignment(aligned_values.cbegin(), aligned_values.cend(), 1);
         }
         {
            static std::array<_aligned_storage<16, 32>::type, 10> aligned_values;
            check_alignment(aligned_values.cbegin(), aligned_values.cend(), 1);
         }
         {
            static std::array<_aligned_storage<16, 64>::type, 10> aligned_values;
            check_alignment(aligned_values.cbegin(), aligned_values.cend(), 1);
         }
      }
   }
}

}
}
