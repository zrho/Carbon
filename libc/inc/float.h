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

#ifndef FLOAT_H
#define FLOAT_H

// Common
#define FLT_RADIX       2                   // b
#define DECIMAL_DIG     21                  // n
#define FLT_ROUNDS      (-1)                // TODO: Determine this!
#define FLT_EVAL_METHOD (-1)

// Float
#define FLT_MANT_DIG    24                  // p
#define FLT_DIG         6
#define FLT_MIN_EXP     (-125)              // e_min
#define FLT_MIN_10_EXP  (-37)
#define FLT_MAX_EXP     128                 // e_max
#define FLT_MAX_10_EXP  38
#define FLT_MAX         3.40282347E+38F
#define FLT_EPSILON     1.19209290E-07F
#define FLT_MIN         1.17549435E-38F

// Double
#define DBL_MANT_DIG    53
#define DBL_DIG         15
#define DBL_MIN_EXP     (-1024)
#define DBL_MIN_10_EXP  (-307)
#define DBL_MAX_EXP     1024
#define DBL_MAX_10_EXP  308
#define DBL_MAX         1.7976931348623157E+308
#define DBL_EPSILON     2.2204460492503131E-16
#define DBL_MIN         2.2250738585072014E-308

// Long Double
#define LDBL_MANT_DIG   64
#define LDBL_DIG        18
#define LDBL_MIN_EXP    (-16381)
#define LDBL_MIN_10_EXP (-4931)
#define LDBL_MAX_EXP    16384
#define LDBL_MAX_10_EXP 4932
#define LDBL_MAX        1.1897314953572317650E+4932L
#define LDBL_EPSILON    1.0842021724855044340E-19L
#define LDBL_MIN        3.3621031431120935063E-4932L

#endif
