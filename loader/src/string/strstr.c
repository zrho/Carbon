/**
 * Carbon Operating System
 * Copyright (C) 2011 Lukas Heidemann
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <api/types.h>
#include <api/string.h>

int8_t *strstr(const int8_t *haystack, const int8_t *needle)
{
    size_t str_pos, substr_pos;
    substr_pos = 0;
    
    if (0 == needle[0])
        return (int8_t *) haystack;
    
    for (str_pos = 0; 0 != haystack[str_pos]; ++str_pos) {
        if (needle[substr_pos] == haystack[str_pos]) {
            ++substr_pos;
            
            if (0 == needle[substr_pos])
                return (int8_t *)
                    (((uintptr_t) haystack) + str_pos - substr_pos + 1);
        } else {
            substr_pos = 0;
        }
    }
    
    return (int8_t *) 0;
}
