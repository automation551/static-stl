/*
Copyright © 2015 Kean Mariotti <kean.mariotti@gmail.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
*/

#include <catch.hpp>
#include <algorithm>
#include <sstl/__internal/_except.h>
#include <sstl/dequeng.h>

#include "utility.h"
#include "counted_type.h"

namespace sstl_test
{

using deque_int_base_t = sstl::dequeng<int>;
using deque_int_t = sstl::dequeng<int, 11>;
using deque_counted_type_t = sstl::dequeng<counted_type, 11>;

template<class T>
sstl::dequeng<T, 11> make_noncontiguous_deque(std::initializer_list<T> init)
{
   auto d = sstl::dequeng<T, 11>{};
   for(size_t i=0; i<d.capacity()-1; ++i)
   {
      d.push_back(0);
      d.pop_front();
   }
   for(auto& value : init)
   {
      d.push_back(value);
   }
   return d;
}

TEST_CASE("dequeng")
{
   SECTION("user cannot directly construct the base class")
   {
      #if !_sstl_is_gcc()
         REQUIRE(!std::is_default_constructible<deque_int_base_t>::value);
      #endif
      REQUIRE(!std::is_copy_constructible<deque_int_base_t>::value);
      REQUIRE(!std::is_move_constructible<deque_int_base_t>::value);
   }

   SECTION("user cannot directly destroy the base class")
   {
      #if !_is_msvc() //MSVC (VS2013) has a buggy implementation of std::is_destructible
      REQUIRE(!std::is_destructible<deque_int_base_t>::value);
      #endif
   }

   SECTION("default constructor")
   {
      auto d = deque_int_t();
      REQUIRE(d.empty());
   }

   SECTION("count constructor")
   {
      SECTION("contained values")
      {
         auto actual = deque_int_t(3);
         auto expected = deque_int_t{0, 0, 0};
         REQUIRE(actual == expected);
      }
      #if _sstl_has_exceptions()
      SECTION("exception handling")
      {
         counted_type::reset_counts();
         counted_type::throw_at_nth_copy_construction(3);
         REQUIRE_THROWS_AS(deque_counted_type_t(4), counted_type::copy_construction::exception);
         REQUIRE(counted_type::check{}.default_constructions(1).copy_constructions(2).destructions(3));
      }
      #endif
   }

   SECTION("range constructor")
   {
      SECTION("contained values")
      {
         auto init = {0, 1, 2, 3};
         auto d = deque_int_t(init.begin(), init.end());
         REQUIRE(are_containers_equal(d, init));
      }
      #if _sstl_has_exceptions()
      SECTION("exception handling")
      {
         auto init = std::initializer_list<counted_type>{0, 1, 2, 3};
         counted_type::reset_counts();
         counted_type::throw_at_nth_copy_construction(3);
         REQUIRE_THROWS_AS(deque_counted_type_t(init.begin(), init.end()), counted_type::copy_construction::exception);
         REQUIRE(counted_type::check{}.copy_constructions(2).destructions(2));
      }
      #endif
   }

   SECTION("copy constructor")
   {
      SECTION("contained values")
      {
         SECTION("same capacity")
         {
            auto rhs = deque_int_t{0, 1, 2, 3};
            auto lhs = rhs;
            REQUIRE(lhs == rhs);
         }
         SECTION("different capacity")
         {
            auto rhs = sstl::dequeng<int, 7>{0, 1, 2, 3};
            auto lhs = sstl::dequeng<int, 11>{ rhs };
            REQUIRE(lhs == rhs);
         }
         SECTION("non-contiguous values")
         {
            auto rhs = make_noncontiguous_deque({0, 1, 2, 3});
            auto lhs = deque_int_t{ rhs };
            REQUIRE(lhs == rhs);
         }
      }
      SECTION("number of operations")
      {
         auto rhs = deque_counted_type_t{0, 1, 2, 3};
         counted_type::reset_counts();
         auto lhs = rhs;
         REQUIRE(counted_type::check().copy_constructions(4));
      }
      #if _sstl_has_exceptions()
      SECTION("exception handling")
      {
         auto rhs = deque_counted_type_t{0, 1, 2, 3};
         counted_type::reset_counts();
         counted_type::throw_at_nth_copy_construction(3);
         REQUIRE_THROWS_AS(deque_counted_type_t{ rhs }, counted_type::copy_construction::exception);
      }
      #endif
   }

   SECTION("move constructor")
   {
      SECTION("contained values")
      {
         SECTION("same capacity")
         {
            auto rhs = deque_int_t{0, 1, 2, 3};
            auto lhs = deque_int_t{ std::move(rhs) };
            REQUIRE(lhs == (deque_int_t{0, 1, 2, 3}));
         }
         SECTION("different capacity")
         {
            auto rhs = sstl::dequeng<int, 11>{0, 1, 2, 3};
            auto lhs = sstl::dequeng<int, 7>{ std::move(rhs) };
            REQUIRE(lhs == (deque_int_t{0, 1, 2, 3}));
         }
         SECTION("non-contiguous values")
         {
            auto rhs = make_noncontiguous_deque({0, 1, 2, 3});
            auto lhs = deque_int_t{ std::move(rhs) };
            REQUIRE(lhs == (deque_int_t{0, 1, 2, 3}));
         }
      }
      SECTION("number of operations")
      {
         auto rhs = deque_counted_type_t{0, 1, 2, 3};
         counted_type::reset_counts();
         auto lhs = deque_counted_type_t{ std::move(rhs) };
         REQUIRE(counted_type::check{}.move_constructions(4).destructions(4));
      }
      SECTION("test moved-from state")
      {
         auto rhs = deque_int_t{0, 1, 2, 3};
         auto lhs = deque_int_t{ std::move(rhs) };
         REQUIRE(rhs.empty());

         rhs.push_back(10);
         rhs.push_back(11);
         REQUIRE(rhs == (deque_int_t{10, 11}));
      }
      #if _sstl_has_exceptions()
      SECTION("exception handling")
      {
         auto rhs = deque_counted_type_t{0, 1, 2, 3};
         counted_type::reset_counts();
         counted_type::throw_at_nth_move_construction(3);
         REQUIRE_THROWS_AS(deque_counted_type_t{ std::move(rhs) }, counted_type::move_construction::exception);
         REQUIRE(counted_type::check{}.move_constructions(2).destructions(6));
         REQUIRE(rhs.empty());
      }
      #endif
   }

   SECTION("initializer-list constructor")
   {
      SECTION("contained values")
      {
         auto actual = deque_int_t{0, 1, 2};
         auto expected = {0, 1, 2};
         REQUIRE(actual.size() == expected.size());
         REQUIRE(std::equal(actual.cbegin(), actual.cend(), expected.begin()));
      }
      #if _sstl_has_exceptions()
      SECTION("exception handling")
      {
         auto init = std::initializer_list<counted_type>{0, 1, 2, 3};
         counted_type::reset_counts();
         counted_type::throw_at_nth_copy_construction(3);
         REQUIRE_THROWS_AS(deque_counted_type_t{ init }, counted_type::copy_construction::exception);
         REQUIRE(counted_type::check{}.copy_constructions(2).destructions(2));
      }
      #endif
   }

   SECTION("destructor")
   {
      {
         auto d = deque_counted_type_t{0, 1, 2, 3};
         counted_type::reset_counts();
      }
      REQUIRE(counted_type::check().destructions(4));
   }

   SECTION("copy assignment operator")
   {
      auto rhs = deque_counted_type_t{0, 1, 2, 3, 4};
      auto expected_lhs = std::initializer_list<counted_type>{0, 1, 2, 3, 4};

      SECTION("contained values + number of operations")
      {
         SECTION("lhs is empty")
         {
            auto lhs = deque_counted_type_t{};
            counted_type::reset_counts();
            lhs = rhs;
            REQUIRE(are_containers_equal(lhs, expected_lhs));
            REQUIRE(counted_type::check{}.copy_constructions(5));
         }
         SECTION("lhs.size() < rhs.size()")
         {
            auto lhs = deque_counted_type_t{10, 11};
            counted_type::reset_counts();
            lhs = rhs;
            REQUIRE(are_containers_equal(lhs, expected_lhs));
            REQUIRE(counted_type::check{}.copy_assignments(2).copy_constructions(3));
         }
         SECTION("lhs.size() == rhs.size()")
         {
            auto lhs = deque_counted_type_t{10, 11, 12, 13, 14};
            counted_type::reset_counts();
            lhs = rhs;
            REQUIRE(are_containers_equal(lhs, expected_lhs));
            REQUIRE(counted_type::check{}.copy_assignments(5));
         }
         SECTION("lhs.size() > rhs.size()")
         {
            auto lhs = deque_counted_type_t{10, 11, 12, 13, 14, 15, 16};
            counted_type::reset_counts();
            lhs = rhs;
            REQUIRE(are_containers_equal(lhs, expected_lhs));
            REQUIRE(counted_type::check{}.copy_assignments(5).destructions(2));
         }
      }
      #if _sstl_has_exceptions()
      SECTION("exception handling")
      {
         SECTION("copy assignment throws")
         {
            auto lhs = deque_counted_type_t{10, 11, 12};
            counted_type::reset_counts();
            counted_type::throw_at_nth_copy_assignment(2);
            REQUIRE_THROWS_AS(lhs = rhs, counted_type::copy_assignment::exception);
            REQUIRE(counted_type::check().copy_assignments(1).destructions(3));
         }
         SECTION("copy constructor throws")
         {
            auto lhs = deque_counted_type_t{10, 11};
            counted_type::reset_counts();
            counted_type::throw_at_nth_copy_construction(2);
            REQUIRE_THROWS_AS(lhs = rhs, counted_type::copy_construction::exception);
            REQUIRE(counted_type::check().copy_assignments(2).copy_constructions(1).destructions(3));
         }
      }
      #endif
   }

   SECTION("non-member relative operators")
   {
      SECTION("lhs < rhs")
      {
         {
            auto lhs = deque_int_t{0, 1, 2};
            auto rhs = deque_int_t{0, 1, 2, 3};
            REQUIRE(!(lhs == rhs));
            //REQUIRE(lhs != rhs);
            //REQUIRE(lhs < rhs);
            //REQUIRE(lhs <= rhs);
            //REQUIRE(!(lhs > rhs));
            //REQUIRE(!(lhs >= rhs));
         }
         {
            auto lhs = deque_int_t{0, 1, 2, 3};
            auto rhs = deque_int_t{0, 1, 3, 3};
            REQUIRE(!(lhs == rhs));
            //REQUIRE(lhs != rhs);
            //REQUIRE(lhs < rhs);
            //REQUIRE(lhs <= rhs);
            //REQUIRE(!(lhs > rhs));
            //REQUIRE(!(lhs >= rhs));
         }
      }
      SECTION("lhs == rhs")
      {
         {
            auto lhs = deque_int_t{0, 1, 2};
            auto rhs = deque_int_t{0, 1, 2};
            REQUIRE(lhs == rhs);
            //REQUIRE(!(lhs != rhs));
            //REQUIRE(!(lhs < rhs));
            //REQUIRE(lhs <= rhs);
            //REQUIRE(!(lhs > rhs));
            //REQUIRE(lhs >= rhs);
         }
      }
      SECTION("lhs > rhs")
      {
         {
            auto lhs = deque_int_t{0, 1, 2, 3};
            auto rhs = deque_int_t{0, 1, 2};
            REQUIRE(!(lhs == rhs));
            //REQUIRE(lhs != rhs);
            //REQUIRE(!(lhs < rhs));
            //REQUIRE(!(lhs <= rhs));
            //REQUIRE(lhs > rhs);
            //REQUIRE(lhs >= rhs);
         }
         {
            auto lhs = deque_int_t{0, 1, 3, 3};
            auto rhs = deque_int_t{0, 1, 2, 3};
            REQUIRE(!(lhs == rhs));
            //REQUIRE(lhs != rhs);
            //REQUIRE(!(lhs < rhs));
            //REQUIRE(!(lhs <= rhs));
            //REQUIRE(lhs > rhs);
            //REQUIRE(lhs >= rhs);
         }
      }
   }
}

}
