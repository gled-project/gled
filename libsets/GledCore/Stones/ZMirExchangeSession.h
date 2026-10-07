// Copyright (C) Matevz Tadel.
// This file is part of Gled.
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GledCore_ZMirExchangeSession_H
#define GledCore_ZMirExchangeSession_H

#include <Gled/GledTypes.h>
#include <Gled/GMutex.h>

namespace gled {

template<class T>
class ZMES_map : private std::unordered_map<UInt_t, void*> {

  typedef std::unordered_map<UInt_t, void*>           base;
  typedef std::unordered_map<UInt_t, void*>::iterator iter;

  GMutex	m_mutex;
  UInt_t	m_key;
public:
  ZMES_map() : m_key(0) {}

  UInt_t insert(T* md) {
    m_mutex.Lock();
    UInt_t key = ++m_key;
    base::insert(std::pair<UInt_t,void*>(key, md));
    m_mutex.Unlock();
    return key;
  }

  void insert(UInt_t key, T* md) {
    m_mutex.Lock();
    base::insert(std::pair<UInt_t,void*>(key, md));
    m_mutex.Unlock();
  }

  T* retrieve(UInt_t key) {
    m_mutex.Lock();
    T* ret = 0;
    iter i = base::find(key);
    if(i != base::end()) ret = static_cast<T*>(i->second);
    m_mutex.Unlock();
    return ret;
  }

  void remove(UInt_t key) {
    m_mutex.Lock();
    iter i = base::find(key);
    if(i != base::end()) { delete static_cast<T*>(i->second); base::erase(i); }
    m_mutex.Unlock();
  }
};

#endif
} // endnamespace gled

