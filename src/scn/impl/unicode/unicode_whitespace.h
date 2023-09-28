// Copyright 2017 Elias Kosunen
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This file is a part of scnlib:
//     https://github.com/eliaskosunen/scnlib

#pragma once

#include <scn/impl/unicode/unicode.h>
#include <scn/impl/util/ascii_ctype.h>

namespace scn {
    SCN_BEGIN_NAMESPACE

    namespace impl {
        constexpr bool is_cp_space(code_point cp) SCN_NOEXCEPT
        {
            // Pattern_White_Space property
            return (static_cast<unsigned>(cp) >= 0x09 &&
                    static_cast<unsigned>(cp) <= 0x0d) ||
                   cp == code_point{0x20} ||
                   cp == code_point{0x85} ||    // NEXT LINE (NEL)
                   cp == code_point{0x200e} ||  // LEFT-TO-RIGHT MARK
                   cp == code_point{0x200f} ||  // RIGHT-TO-LEFT MARK
                   cp == code_point{0x2028} ||  // LINE SEPARATOR
                   cp == code_point{0x2029};    // PARAGRAPH SEPARATOR
        }

        template <typename CharT>
        inline auto is_first_char_space(std::basic_string_view<CharT> str)
            -> iterator_value_result<
                ranges::iterator_t<std::basic_string_view<CharT>>,
                bool>
        {
            // TODO: optimize
            SCN_EXPECT(!str.empty());
            auto cp = get_next_code_point(str);
            if (!cp) {
                return {ranges::next(str.begin()), false};
            }

            return {cp->iterator, is_cp_space(cp->value)};
        }
    }  // namespace impl

    SCN_END_NAMESPACE
}  // namespace scn
