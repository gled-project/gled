// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

// RSA keys and the shared secret of the authentication handshake.

#include "GKeyRSA.h"

#include <TBuffer.h>

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>

#include <memory>

using namespace gled;

namespace
{
  using upCtx_t = std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)>;

  Exc_t ssl_error(const Exc_t& eh)
  {
    unsigned long e = ERR_get_error();
    ERR_clear_error();
    if (e == 0) return eh + "unknown OpenSSL error";
    char buf[256];
    ERR_error_string_n(e, buf, sizeof(buf));
    return eh + buf;
  }

  Int_t bytes_left(TBuffer& b) { return b.BufferSize() - b.Length(); }
}

//==============================================================================

GKeyRSA::GKeyRSA() :
  bIsPrivate(false),
  pKey(nullptr)
{}

GKeyRSA::~GKeyRSA()
{
  EVP_PKEY_free(pKey);
}

void GKeyRSA::set_key(EVP_PKEY* key, bool priv)
{
  EVP_PKEY_free(pKey);
  pKey       = key;
  bIsPrivate = priv;
}

//==============================================================================

void GKeyRSA::ReadPubKey(const char* file)
{
  static const Exc_t _eh("GKeyRSA::ReadPubKey ");

  FILE* fp = fopen(file, "r");
  if (!fp) throw _eh + "file '" + file + "' not found";
  EVP_PKEY* key = PEM_read_PUBKEY(fp, nullptr, nullptr, nullptr);
  fclose(fp);
  if (!key) throw ssl_error(_eh + file + ": ");
  if (!EVP_PKEY_is_a(key, "RSA"))
  {
    EVP_PKEY_free(key);
    throw _eh + file + ": not an RSA key";
  }
  set_key(key, false);
}

void GKeyRSA::ReadPrivKey(const char* file)
{
  static const Exc_t _eh("GKeyRSA::ReadPrivKey ");

  FILE* fp = fopen(file, "r");
  if (!fp) throw _eh + "file '" + file + "' not found";
  EVP_PKEY* key = PEM_read_PrivateKey(fp, nullptr, nullptr, nullptr);
  fclose(fp);
  if (!key) throw ssl_error(_eh + file + ": ");
  if (!EVP_PKEY_is_a(key, "RSA"))
  {
    EVP_PKEY_free(key);
    throw _eh + file + ": not an RSA key";
  }
  set_key(key, true);
}

void GKeyRSA::ShareKey(const GKeyRSA& k)
{
  assert(k.pKey);
  EVP_PKEY_up_ref(k.pKey);
  set_key(k.pKey, k.bIsPrivate);
}

//==============================================================================

void GKeyRSA::GenerateSecret()
{
  static const Exc_t _eh("GKeyRSA::GenerateSecret ");

  // Fits RSA-OAEP with SHA-1 for keys from 512 bits on.
  mSecret.resize(32);
  if (RAND_bytes(mSecret.data(), mSecret.size()) != 1)
    throw ssl_error(_eh);
}

void GKeyRSA::SendSecret(TBuffer& b)
{
  static const Exc_t _eh("GKeyRSA::SendSecret ");

  assert(pKey);

  upCtx_t ctx(EVP_PKEY_CTX_new_from_pkey(nullptr, pKey, nullptr), EVP_PKEY_CTX_free);
  size_t  len = 0;
  if (!ctx || EVP_PKEY_encrypt_init(ctx.get()) <= 0 ||
      EVP_PKEY_CTX_set_rsa_padding(ctx.get(), RSA_PKCS1_OAEP_PADDING) <= 0 ||
      EVP_PKEY_encrypt(ctx.get(), nullptr, &len, mSecret.data(), mSecret.size()) <= 0)
    throw ssl_error(_eh);

  std::vector<unsigned char> enc(len);
  if (EVP_PKEY_encrypt(ctx.get(), enc.data(), &len, mSecret.data(), mSecret.size()) <= 0)
    throw ssl_error(_eh);

  b << (Int_t) len;
  b.WriteFastArray((char*) enc.data(), len);
}

void GKeyRSA::ReceiveSecret(TBuffer& b)
{
  static const Exc_t _eh("GKeyRSA::ReceiveSecret ");

  assert(pKey && bIsPrivate);

  Int_t lenmsg;
  b >> lenmsg;
  if (lenmsg <= 0 || lenmsg > EVP_PKEY_get_size(pKey) || lenmsg > bytes_left(b))
    throw _eh + "bad message length";
  std::vector<unsigned char> enc(lenmsg);
  b.ReadFastArray((char*) enc.data(), lenmsg);

  upCtx_t ctx(EVP_PKEY_CTX_new_from_pkey(nullptr, pKey, nullptr), EVP_PKEY_CTX_free);
  size_t  len = 0;
  if (!ctx || EVP_PKEY_decrypt_init(ctx.get()) <= 0 ||
      EVP_PKEY_CTX_set_rsa_padding(ctx.get(), RSA_PKCS1_OAEP_PADDING) <= 0 ||
      EVP_PKEY_decrypt(ctx.get(), nullptr, &len, enc.data(), enc.size()) <= 0)
    throw ssl_error(_eh);

  mSecret.resize(len);
  if (EVP_PKEY_decrypt(ctx.get(), mSecret.data(), &len, enc.data(), enc.size()) <= 0)
  {
    mSecret.clear();
    throw ssl_error(_eh);
  }
  mSecret.resize(len);
}

bool GKeyRSA::MatchSecrets(const GKeyRSA& a)
{
  return ! mSecret.empty() && mSecret == a.mSecret;
}

//==============================================================================

void GKeyRSA::StreamPubKey(TBuffer& b)
{
  static const Exc_t _eh("GKeyRSA::StreamPubKey ");

  // The key goes as DER SubjectPublicKeyInfo.
  if (b.IsReading())
  {
    Int_t len;
    b >> len;
    if (len <= 0 || len > bytes_left(b))
      throw _eh + "bad key length";
    std::vector<unsigned char> der(len);
    b.ReadFastArray((char*) der.data(), len);

    const unsigned char* p = der.data();
    EVP_PKEY* key = d2i_PUBKEY(nullptr, &p, len);
    if (!key) throw ssl_error(_eh);
    if (!EVP_PKEY_is_a(key, "RSA"))
    {
      EVP_PKEY_free(key);
      throw _eh + "not an RSA key";
    }
    set_key(key, false);
  }
  else
  {
    assert(pKey);

    unsigned char* der = nullptr;
    int len = i2d_PUBKEY(pKey, &der);
    if (len <= 0) throw ssl_error(_eh);
    b << (Int_t) len;
    b.WriteFastArray((char*) der, len);
    OPENSSL_free(der);
  }
}
