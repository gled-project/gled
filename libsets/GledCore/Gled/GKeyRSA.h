// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_GKeyRSA_H
#define GledCore_GKeyRSA_H

#include <Gled/GledTypes.h>

#include <vector>

class TBuffer;

typedef struct evp_pkey_st EVP_PKEY;

namespace gled {

class GKeyRSA {

private:
  std::vector<unsigned char> mSecret;

  void set_key(EVP_PKEY* key, bool priv);

protected:
  Bool_t	bIsPrivate;	// X{G}

  EVP_PKEY*	pKey;

public:
  GKeyRSA();
  GKeyRSA(const GKeyRSA&) = delete;
  GKeyRSA& operator=(const GKeyRSA&) = delete;
  virtual ~GKeyRSA();

  void ReadPubKey(const char* file);
  void ReadPrivKey(const char* file);
  void ShareKey(const GKeyRSA& k);

  void GenerateSecret();
  void SendSecret(TBuffer& b);
  void ReceiveSecret(TBuffer& b);
  bool MatchSecrets(const GKeyRSA& a);

  void StreamPubKey(TBuffer& b);

#include "GKeyRSA.h7"

  ClassDef(GKeyRSA, 0);
}; // endclass GKeyRSA

} // endnamespace gled

#endif
