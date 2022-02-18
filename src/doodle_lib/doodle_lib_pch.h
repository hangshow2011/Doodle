﻿//
// Created by TD on 2021/6/17.
//

#pragma once

#include <ciso646>
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/apply_permutation.hpp>
#include <boost/algorithm/clamp.hpp>
#include <boost/algorithm/find_backward.hpp>
#include <boost/algorithm/find_not.hpp>
#include <boost/algorithm/gather.hpp>
#include <boost/algorithm/hex.hpp>
// #include <boost/algorithm/is_palindrome.hpp>
#include <boost/algorithm/is_partitioned_until.hpp>
#include <boost/algorithm/minmax.hpp>
#include <boost/algorithm/minmax_element.hpp>
#include <boost/algorithm/sort_subrange.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string_regex.hpp>
#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/algorithm/cxx11/copy_if.hpp>
#include <boost/algorithm/cxx11/copy_n.hpp>
#include <boost/algorithm/cxx11/find_if_not.hpp>
#include <boost/algorithm/cxx11/iota.hpp>
#include <boost/algorithm/cxx11/is_partitioned.hpp>
#include <boost/algorithm/cxx11/is_permutation.hpp>
#include <boost/algorithm/cxx11/is_sorted.hpp>
#include <boost/algorithm/cxx11/none_of.hpp>
#include <boost/algorithm/cxx11/one_of.hpp>
#include <boost/algorithm/cxx11/partition_copy.hpp>
#include <boost/algorithm/cxx11/partition_point.hpp>
#include <boost/algorithm/cxx14/equal.hpp>
#include <boost/algorithm/cxx14/is_permutation.hpp>
#include <boost/algorithm/cxx14/mismatch.hpp>
#include <boost/algorithm/cxx17/exclusive_scan.hpp>
#include <boost/algorithm/cxx17/for_each_n.hpp>
#include <boost/algorithm/cxx17/inclusive_scan.hpp>
#include <boost/algorithm/cxx17/reduce.hpp>
#include <boost/algorithm/cxx17/transform_exclusive_scan.hpp>
#include <boost/algorithm/cxx17/transform_inclusive_scan.hpp>
#include <boost/algorithm/cxx17/transform_reduce.hpp>
#include <boost/algorithm/searching/boyer_moore.hpp>
#include <boost/algorithm/searching/boyer_moore_horspool.hpp>
#include <boost/algorithm/searching/knuth_morris_pratt.hpp>
#include <boost/algorithm/searching/detail/bm_traits.hpp>
#include <boost/algorithm/searching/detail/debugging.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/compare.hpp>
#include <boost/algorithm/string/concept.hpp>
#include <boost/algorithm/string/config.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string/find_format.hpp>
#include <boost/algorithm/string/find_iterator.hpp>
#include <boost/algorithm/string/finder.hpp>
#include <boost/algorithm/string/formatter.hpp>
#include <boost/algorithm/string/iter_find.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/predicate_facade.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/algorithm/string/regex_find_format.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/sequence_traits.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/std_containers_traits.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/yes_no_type.hpp>
#include <boost/algorithm/string/std/list_traits.hpp>
// #include <boost/algorithm/string/std/rope_traits.hpp>
// #include <boost/algorithm/string/std/slist_traits.hpp>
#include <boost/algorithm/string/std/string_traits.hpp>

#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/algorithm_ext.hpp>
#include <boost/range/irange.hpp>
#include <boost/range.hpp>

#include <range/v3/action.hpp>
#include <range/v3/algorithm.hpp>
#include <range/v3/functional.hpp>
#include <range/v3/iterator.hpp>
#include <range/v3/numeric.hpp>
#include <range/v3/range.hpp>
#include <range/v3/utility.hpp>
#include <range/v3/view.hpp>

#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <chrono>
#include <codecvt>
#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <regex>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <unordered_set>
#include <variant>
#include <vector>
#pragma warning(disable : 4251)
#pragma warning(disable : 4275)

#include <date/date.h>
#include <doodle_lib/doodle_macro.h>
#include <doodle_lib/lib_warp/boost_locale_warp.h>
#include <doodle_lib/lib_warp/boost_uuid_warp.h>
#include <doodle_lib/lib_warp/cmrcWarp.h>
#include <doodle_lib/lib_warp/sqlppWarp.h>
#include <doodle_lib/lib_warp/std_warp.h>
#include <doodle_lib_export.h>

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fmt/chrono.h>
#include <fmt/compile.h>
#include <fmt/color.h>
#include <fmt/os.h>
#include <fmt/ostream.h>
#include <fmt/printf.h>
#include <fmt/xchar.h>

#include <boost/numeric/conversion/cast.hpp>
#include <entt/entt.hpp>
