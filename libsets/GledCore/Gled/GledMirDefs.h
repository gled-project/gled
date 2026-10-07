// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_GledMirDefs_H
#define GledCore_GledMirDefs_H

#include <Stones/ZMIR.h>
#include <Ephra/Saturn.h>

namespace gled {

#define SP_MIR(_lens_, _method_, ...) \
  { std::unique_ptr<ZMIR> mir(_lens_->S_##_method_(__VA_ARGS__)); \
    _lens_->GetSaturn()->PostMIR(mir); }

#define SP_MIR_BEAM(_rec_, _lens_, _method_, ...) \
  { std::unique_ptr<ZMIR> mir(_lens_->S_##_method_(__VA_ARGS__)); \
    mir->SetRecipient(_rec_); \
    _lens_->GetSaturn()->PostMIR(mir); }

#define SP_MIR_SATURN(_sat_, _lens_, _method_, ...) \
  { std::unique_ptr<ZMIR> mir(_lens_->S_##_method_(__VA_ARGS__)); \
    _sat_->PostMIR(mir); }

} // endnamespace gled

#endif
