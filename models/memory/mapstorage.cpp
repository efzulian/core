// vim : set fileencoding=utf-8 expandtab noai ts=4 sw=4 :
/// @addtogroup memory
/// @{
/// @file arraystorage.cpp
/// source file defining the implementation of the mapstorage model.
///
/// @date 2014-2014
/// @copyright All rights reserved.
///            Any reproduction, use, distribution or disclosure of this
///            program, without the express, prior written consent of the
///            authors is strictly prohibited.
/// @author Jan Wagner
///

#include "models/memory/mapstorage.h"

MapStorage::MapStorage(const uint32_t &size) {
}

MapStorage::~MapStorage() {
}

void MapStorage::write(const uint32_t &addr, const uint8_t &byte) {
  data[addr] = byte;
}

uint8_t MapStorage::read(const uint32_t &addr) {
  return data[addr];
}

void MapStorage::erase(const uint32_t &start, const uint32_t &end) {
  // Find or insert start address
  map_mem::iterator start_iter = data.find(start);
  if (start_iter == data.end()) {
    data.insert(std::make_pair(start, 0));
    start_iter = data.find(start);
  }

  // Find or insert end address
  map_mem::iterator end_iter = data.find(end);
  if (end_iter == data.end()) {
    data.insert(std::make_pair(end, 0));
    end_iter = data.find(end);
  }

  // Erase section
  data.erase(start_iter, end_iter);
}

void MapStorage::read_block(const uint32_t &addr, uint8_t *ptr, const uint32_t &len) {
  for (size_t i = 0; i < len; i++) {
    ptr[i] = read(addr + i);
  }
}

void MapStorage::write_block(const uint32_t &addr, uint8_t *ptr, const uint32_t &len) {
  for (size_t i = 0; i < len; i++) {
    write(addr + i, ptr[i]);
  }
}
/// @}
