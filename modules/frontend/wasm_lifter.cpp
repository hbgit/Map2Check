/**
 * Copyright (C) 2026 Map2Check tool
 * SPDX-License-Identifier: GPL-2.0
 *
 * WasmLifter Implementation
 */

#include "wasm_lifter.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>

#include "utils/log.hpp"

namespace Map2Check {

WasmLifter::WasmLifter() = default;

WasmLifter::WasmLifter(const WasmLifterConfig& config) : config_(config) {}

WasmLifterResult WasmLifter::lift(const std::string& wasmPath) {
  Map2Check::Log::Info("WasmLifter: Lifting " + wasmPath);
  
  // Validate input
  std::ifstream wasmFile(wasmPath);
  if (!wasmFile.good()) {
    throw WasmLifterError("Cannot read WASM file: " + wasmPath);
  }
  wasmFile.close();
  
  // Generate temporary file paths
  std::string baseName = tempPath("wasm_lift");
  std::string cPath = baseName + ".c";
  std::string hPath = baseName + ".h";
  std::string bcPath = baseName + ".bc";
  std::string llPath = baseName + ".ll";
  
  try {
    // Step 1: wasm2c (.wasm → .c/.h)
    Map2Check::Log::Info("WasmLifter: Running wasm2c...");
    runWasm2c(wasmPath, cPath, hPath);
    
    // Step 2: clang (.c → .bc + .ll)
    Map2Check::Log::Info("WasmLifter: Running clang-16...");
    runClang(cPath, hPath, bcPath);
    runClangEmitLLVM(cPath, hPath, llPath);
    
    // Step 3: Extract metadata from LLVM IR
    WasmLifterResult result;
    result.bitcodePath = bcPath;
    result.llvmAssemblyPath = llPath;
    
    if (config_.keepIntermediate) {
      result.cSourcePath = cPath;
      result.headerPath = hPath;
    } else {
      // Clean up intermediate files
      std::remove(cPath.c_str());
      std::remove(hPath.c_str());
    }
    
    extractMetadata(llPath, result);
    
    Map2Check::Log::Info("WasmLifter: Lifting complete. Entry point: " + 
                         result.entryPointName);
    
    return result;
    
  } catch (const WasmLifterError& e) {
    // Clean up on failure
    std::remove(cPath.c_str());
    std::remove(hPath.c_str());
    std::remove(bcPath.c_str());
    std::remove(llPath.c_str());
    throw;
  }
}

std::string WasmLifter::liftToBitcode(const std::string& wasmPath) {
  WasmLifterResult result = lift(wasmPath);
  return result.bitcodePath;
}

void WasmLifter::runWasm2c(const std::string& wasmPath,
                           const std::string& outputCPath,
                           const std::string& outputHPath) {
  std::ostringstream cmd;
  cmd << config_.wasm2cPath 
      << " \"" << wasmPath << "\""
      << " -o \"" << outputCPath << "\"";
  
  execOrThrow(cmd.str(), "wasm2c failed to convert WASM to C");
  
  // wasm2c generates both .c and .h; verify they exist
  std::ifstream cFile(outputCPath);
  if (!cFile.good()) {
    throw WasmLifterError("wasm2c did not generate C file: " + outputCPath);
  }
  cFile.close();
}

void WasmLifter::runClang(const std::string& cPath,
                          const std::string& hPath,
                          const std::string& outputBCPath) {
  std::ostringstream cmd;
  cmd << config_.clangPath
      << " -c -emit-llvm"
      << " -I\"" << config_.wasmRtIncludePath << "\""
      << " -I\"" << hPath.substr(0, hPath.find_last_of("/")) << "\""
      << " -Wno-unused-command-line-argument";
  
  for (const auto& flag : config_.extraClangFlags) {
    cmd << " " << flag;
  }
  
  cmd << " \"" << cPath << "\""
      << " -o \"" << outputBCPath << "\"";
  
  execOrThrow(cmd.str(), "clang failed to compile wasm2c output to bitcode");
}

void WasmLifter::runClangEmitLLVM(const std::string& cPath,
                                  const std::string& hPath,
                                  const std::string& outputLLPath) {
  std::ostringstream cmd;
  cmd << config_.clangPath
      << " -S -emit-llvm"
      << " -I\"" << config_.wasmRtIncludePath << "\""
      << " -I\"" << hPath.substr(0, hPath.find_last_of("/")) << "\"";
  
  for (const auto& flag : config_.extraClangFlags) {
    cmd << " " << flag;
  }
  
  cmd << " \"" << cPath << "\""
      << " -o \"" << outputLLPath << "\"";
  
  execOrThrow(cmd.str(), "clang failed to compile wasm2c output to LLVM assembly");
}

void WasmLifter::extractMetadata(const std::string& llPath,
                                 WasmLifterResult& result) {
  std::ifstream llFile(llPath);
  if (!llFile.is_open()) {
    throw WasmLifterError("Cannot read LLVM assembly: " + llPath);
  }
  
  std::string line;
  std::regex exportRegex("define.*@w2c_([A-Za-z0-9_]+)\\(");
  while (std::getline(llFile, line)) {
    std::smatch match;

    // Look for entry point: w2c_*_0x5Fstart (mangled WASM start function)
    if (result.entryPointName.empty() &&
        line.find("_0x5Fstart") != std::string::npos &&
        line.find("@w2c_") != std::string::npos &&
        line.find("define") != std::string::npos) {
      size_t atPos = line.find("@w2c_");
      size_t endPos = line.find("(", atPos);
      if (atPos != std::string::npos && endPos != std::string::npos) {
        result.entryPointName = line.substr(atPos + 1, endPos - atPos - 1);
      }
    }

    // Look for exported functions (skip if it's the entry point)
    if (std::regex_search(line, match, exportRegex)) {
      std::string funcName = "w2c_" + std::string(match[1]);
      if (funcName != result.entryPointName) {
        result.exportedFunctions.push_back(funcName);
      }
    }
  }

  llFile.close();

  // Default entry point if not found
  if (result.entryPointName.empty()) {
    result.entryPointName = "w2c__start";
    Map2Check::Log::Warning("WasmLifter: Could not detect entry point, defaulting to w2c__start");
  }
}

void WasmLifter::execOrThrow(const std::string& cmd, const std::string& errorMsg) {
  Map2Check::Log::Debug("Executing: " + cmd);
  
  int ret = std::system(cmd.c_str());
  if (ret != 0) {
    std::ostringstream err;
    err << errorMsg << " (exit code: " << ret << ")";
    throw WasmLifterError(err.str());
  }
}

std::string WasmLifter::tempPath(const std::string& suffix) {
  std::string baseDir = config_.workingDir.empty() ? "/tmp" : config_.workingDir;
  
  // Generate unique filename using timestamp + random
  std::ostringstream path;
  path << baseDir << "/m2c_wasm_" << suffix << "_" 
       << std::time(nullptr) << "_" << rand();
  return path.str();
}

}  // namespace Map2Check
