/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

/**********************************************************************
 * SHA-256, for the Test-Comp test-suite <programhash> field.
 *
 * Map2Check already had SHA-1 (gen_crypto_hash.hpp, via Boost), which the
 * pipeline uses to name scratch directories. The exchange format defines
 * programhash as SHA-256, so the two coexist rather than one replacing the
 * other.
 *
 * Implemented here rather than pulled in: OpenSSL is not currently linked, and
 * shelling out to sha256sum would make a metadata field depend on an external
 * binary being on PATH inside whatever container the tool runs in.
 ***********************************************************************/

#ifndef MODULES_FRONTEND_UTILS_SHA256_HPP_
#define MODULES_FRONTEND_UTILS_SHA256_HPP_

#include <string>

namespace Map2Check {

/** SHA-256 of an in-memory buffer, as 64 lowercase hex characters.
 * Binary-safe: embedded NUL bytes are hashed like any other byte. */
std::string sha256Hex(const std::string& data);

/** SHA-256 of a file's contents. Returns "" if the file cannot be read.
 * Hashes contents, never the path: <programhash> identifies the program, and
 * a hash of the filename would silently agree across different programs that
 * happen to share a name. */
std::string sha256File(const std::string& path);

}  // namespace Map2Check

#endif  // MODULES_FRONTEND_UTILS_SHA256_HPP_
