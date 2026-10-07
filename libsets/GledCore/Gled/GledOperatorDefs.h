// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_GledOperatorDefs_H
#define GledCore_GledOperatorDefs_H

#include <Gled/GledMirDefs.h>

namespace gled {

#define OP_EXE_OR_SP_MIR(_lens_, _method_, ...) { \
    if(op_arg->fMultix) { _lens_->_method_(__VA_ARGS__); } \
    else                { SP_MIR(_lens_, _method_, __VA_ARGS__) }}

#define OP_EXE_OR_SP_MIR_SATURN(_sat_, _lens_, _method_, ...) { \
    if(op_arg->fMultix) { _lens_->_method_(__VA_ARGS__); } \
    else                { SP_MIR_SATURN(_sat_, _lens_, _method_, __VA_ARGS__) }}

} // endnamespace gled

#endif
