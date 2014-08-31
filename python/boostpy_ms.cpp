//////////////////////////////////////////////////////////////////////////////
/// Copyright (C) 2014 Gefu Tang <tanggefu@gmail.com>. All Rights Reserved.
///
/// This file is part of LSHBOX.
///
/// LSHBOX is free software: you can redistribute it and/or modify it under
/// the terms of the GNU General Public License as published by the Free
/// Software Foundation, either version 3 of the License, or(at your option)
/// any later version.
///
/// LSHBOX is distributed in the hope that it will be useful, but WITHOUT
/// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
/// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
/// more details.
///
/// You should have received a copy of the GNU General Public License along
/// with LSHBOX. If not, see <http://www.gnu.org/licenses/>.
///
/// @version 0.1
/// @author Gefu Tang & Zhifeng Xiao
/// @date 2014.6.30
//////////////////////////////////////////////////////////////////////////////
#define MS_WIN64
#define BOOST_PYTHON_SOURCE
#include <libs/python/src/numeric.cpp>
#include <libs/python/src/list.cpp>
#include <libs/python/src/long.cpp>
#include <libs/python/src/dict.cpp>
#include <libs/python/src/tuple.cpp>
#include <libs/python/src/str.cpp>
#include <libs/python/src/slice.cpp>
#include <libs/python/src/converter/from_python.cpp>
#include <libs/python/src/converter/registry.cpp>
#include <libs/python/src/converter/type_id.cpp>
#include <libs/python/src/object/enum.cpp>
#include <libs/python/src/object/class.cpp>
#include <libs/python/src/object/function.cpp>
#include <libs/python/src/object/inheritance.cpp>
#include <libs/python/src/object/life_support.cpp>
#include <libs/python/src/object/pickle_support.cpp>
#include <libs/python/src/errors.cpp>
#include <libs/python/src/module.cpp>
#include <libs/python/src/converter/builtin_converters.cpp>
#include <libs/python/src/converter/arg_to_python_base.cpp>
#include <libs/python/src/object/iterator.cpp>
#include <libs/python/src/object/stl_iterator.cpp>
#include <libs/python/src/object_protocol.cpp>
#include <libs/python/src/object_operators.cpp>
#include <libs/python/src/wrapper.cpp>
#include <libs/python/src/import.cpp>
#include <libs/python/src/exec.cpp>
#include <libs/python/src/object/function_doc_signature.cpp>