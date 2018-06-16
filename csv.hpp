/*******************************************************************************
*                                                                              *
* csvpp Header only CSV parsing library                                        *
*                                                                              *
* Copyright (C) 2018 Michele Mei <michele.mei@gmail.com>                       *
*                                                                              *
* csvpp is free software: you can redistribute it and/or modify                *
* it under the terms of the GNU General Public License as published by         *
* the Free Software Foundation, either version 3 of the License, or            *
* (at your option) any later version.                                          *
*                                                                              *
* G2D is distributed in the hope that it will be useful,                       *
* but WITHOUT ANY WARRANTY; without even the implied warranty of               *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                *
* GNU General Public License for more details.                                 *
*                                                                              *
* You can get a copy of of the GNU General Public License                      *
* at<http://www.gnu.org/licenses/>.                                            *
*                                                                              *
* Author: Michele Mei                                                          *
* E-Mail: michele.mei@gmail.com                                                *
*                                                                              *
*******************************************************************************/
#pragma once

// Version 0.1
#define CSVPP_MAJOR_VERSION 0
#define CSVPP_MINOR_VERSION 1

#include <string>
#include <vector>
#include <algorithm>

namespace michelemei {
    /// Returns next csv line in a stream
    ///
    /// @brief Similarly to `std:getline()` this function reads the string up
    /// to the delimiter in accordance with csv double-quote rule
    /// @param[in] input Input stream
    /// @param[out] str Output string, unlike `std::getline()` it not will be
    /// cleaned
    /// @param[in] delim Delimiter character
    /// @return If the reading is correctly done returns true, otherwise
    /// returns false, `str` output parameter value may be inconsistent.
    template<class _Elem, class _Traits, class _Alloc>
    static bool _getline_csv_impl(std::basic_istream<_Elem, _Traits>& input,
        std::basic_string<_Elem, _Traits, _Alloc>& str,
        const _Elem delim)
    {
        static std::basic_string<_Elem> buffer;
        if (std::getline(input, buffer, delim))
        {
            str.append(buffer);
            if (std::count(str.begin(), str.end(), input.widen('"')) % 2 == 1)
            {
                str += delim;
                return _getline_csv_impl(input, str, delim);
            }
            return true;
        }
        return false;
    }

    /// Returns next cvs line in a stream
    ///
    /// @brief A CSV line may not have a 1 to 1 matching in the file lines
    /// (as explained in 2.6 of RFC4180 (https://tools.ietf.org/html/rfc4180)).
    /// For this reason, a function is required that behaves like
    /// `std::getline` and respects CSV rules.
    /// @param[in] input Input stream
    /// @param[out] str Output string, unlike `std::getline()` it not will be
    /// cleaned
    /// @param[in] delim Delimiter character
    /// @return If the reading is correctly done returns true, otherwise
    /// returns false, `str` output parameter value may be inconsistent.
    template<class _Elem, class _Traits, class _Alloc>
    static bool _getline_csv(std::basic_istream<_Elem, _Traits>& input,
        std::basic_string<_Elem, _Traits, _Alloc>& str,
        const _Elem delim)
    {
        str.clear();
        return _getline_csv_impl(input, str, delim);
    }

    /// Erase double-quotes from the string as described in RFC4180 (2.5, 2.7)
    /// @param[in,out] `str` String to be cleaned
    /// @return Return `str` parameter
    template<class _Elem, class _Traits, class _Alloc>
    static std::basic_string<_Elem, _Traits, _Alloc>&
        _erase_dquote(std::basic_string<_Elem, _Traits, _Alloc>& str)
    {
        static _Elem double_quote =
            std::use_facet<std::ctype<_Elem>>(std::locale()).widen('"');

        if (str.size() > 1)
        {
            // optional double-quote field (RFC4180 2.5)
            if (str[0] == double_quote && str[str.length() - 1] == double_quote)
                str = str.substr(1, str.length() - 2);

            // replace double ""double-quote"" :-) (RFC4180 2.7)
            str.erase(std::unique(str.begin(), str.end(),
                [](_Elem lhs, _Elem rhs) -> bool
            {
                return lhs == double_quote && lhs == rhs;
            }),
                str.end());
        }
        return str;
    }

    /// Parse a single csv line
    ///
    /// @param[in,out] input Input stream
    /// @param[in] delim Delimiter character
    /// @param[in] binder Binder callback function
    template<class _Elem, class _Traits, typename _Binder>
    static size_t _parse_csv_line(std::basic_istream<_Elem, _Traits>& input,
        const _Elem delim, _Binder&& binder)
    {
        static std::basic_string<_Elem> token;
        size_t token_counter = 0;

        while (_getline_csv(input, token, delim))
            binder(token_counter++, std::move(_erase_dquote(token)));

        return token_counter;
    }

    template<class _Elem, class _Traits>
    using _csv_line = std::vector<std::basic_string<_Elem, _Traits>>;

    // csv_line shortcut
    using csv_line = _csv_line<std::string::value_type, std::string::traits_type>;
    using wcsv_line = _csv_line<std::wstring::value_type, std::wstring::traits_type>;

    /// Parse a csv line
    ///
    /// @brief Parse csv line and returns a csv_line containing fields
    /// @input Input stream
    /// @delim Delimiter character
    template<class _Elem, class _Traits>
    static _csv_line<_Elem, _Traits>
        parse_csv_line(std::basic_istream<_Elem, _Traits>& input,
            _Elem delim = 0)
    {
        using iss = std::basic_istringstream<_Elem, _Traits>;

        if (delim == 0)
            delim = discover_delimiter(input);

        _csv_line<_Elem, _Traits> fields;
        std::basic_string<_Elem> line;
        if (_getline_csv(input, line, input.widen('\n')))
        {
            _parse_csv_line(iss(line), delim,
                [&fields](size_t, std::basic_string<_Elem>&& value)
            {
                fields.emplace_back(move(value));
            });
        }
        return fields; // RVO
    }

    /// Try to discover delimiter
    ///
    /// @brief It's just counts the occurrence in the next line (,without
    /// extract it)
    /// @return Delimiter character, 0x00 if no character occurrence has been
    /// found
    template<class _Elem, class _Traits>
    static const _Elem discover_delimiter(
        std::basic_istream<_Elem, _Traits>& input,
        std::initializer_list<_Elem>&& delimiters = {
            std::use_facet<std::ctype<_Elem>>(std::locale()).widen(','), // CSV
            std::use_facet<std::ctype<_Elem>>(std::locale()).widen('\t'),// TSV
            std::use_facet<std::ctype<_Elem>>(std::locale()).widen(';')
        })
    {
        auto current_position = input.tellg();

        std::basic_string<_Elem> line;
        std::getline(input, line);

        _Elem best = 0;
        size_t best_count = 0;
        for (const auto& d : delimiters)
        {
            size_t d_count = std::count(line.begin(), line.end(), d);
            if (d_count > best_count)
            {
                best = d;
                best_count = d_count;
            }
        }

        input.seekg(current_position, std::ios_base::beg);
        return best;
    }

    template<class _Elem, class _Traits, typename _Packer>
    static void parse_csv(std::basic_istream<_Elem, _Traits>& input,
        const _Elem delim, _Packer&& packer)
    {
        using iss = std::basic_istringstream<_Elem, _Traits>;
        static std::basic_string<_Elem> buffer;
        do
        {
            if (!_getline_csv(input, buffer, input.widen('\n')))
            {
                return;
            }
        } while (packer(parse_csv_line(iss(buffer), delim)));
    }

    template<class _Elem, class _Traits, typename _Packer>
    static void parse_csv(std::basic_istream<_Elem, _Traits>& input,
        _Packer&& packer)
    {
        parse_csv(input, discover_delimiter(input), packer);
    }
}