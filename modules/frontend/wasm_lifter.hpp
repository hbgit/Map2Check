/**
 * Copyright (C) 2026 Map2Check tool
 * SPDX-License-Identifier: GPL-2.0
 *
 * WasmLifter — Lift WebAssembly binaries to LLVM IR
 *
 * Pipeline: .wasm → wasm2c → .c/.h → clang-16 → .bc (LLVM IR)
 */

#ifndef WASM_LIFTER_HPP
#define WASM_LIFTER_HPP

#include <string>
#include <vector>
#include <stdexcept>

namespace Map2Check {

/**
 * Exception thrown when lifting fails
 */
class WasmLifterError : public std::runtime_error {
public:
  explicit WasmLifterError(const std::string& msg) : std::runtime_error(msg) {}
};

/**
 * Configuration for the WASM lifter
 */
struct WasmLifterConfig {
  /** Path to wasm2c binary (default: "wasm2c") */
  std::string wasm2cPath = "wasm2c";
  
  /** Path to clang-16 binary (default: "/usr/bin/clang-16") */
  std::string clangPath = "/usr/bin/clang-16";
  
  /** Include path for wasm-rt.h (default: "/usr/include") */
  std::string wasmRtIncludePath = "/usr/include";
  
  /** Target triple for clang (default: "x86_64-pc-linux-gnu") */
  std::string targetTriple = "x86_64-pc-linux-gnu";
  
  /** Additional clang flags */
  std::vector<std::string> extraClangFlags;
  
  /** Whether to keep intermediate .c/.h files */
  bool keepIntermediate = false;
  
  /** Working directory for temporary files */
  std::string workingDir;
};

/**
 * Result of a lifting operation
 */
struct WasmLifterResult {
  /** Path to the generated LLVM bitcode (.bc) file */
  std::string bitcodePath;
  
  /** Path to the generated LLVM assembly (.ll) file */
  std::string llvmAssemblyPath;
  
  /** Path to the generated C file (if keepIntermediate) */
  std::string cSourcePath;
  
  /** Path to the generated header file (if keepIntermediate) */
  std::string headerPath;
  
  /** Name of the entry point function in the lifted IR (e.g., "w2c__start") */
  std::string entryPointName;
  
  /** List of exported functions */
  std::vector<std::string> exportedFunctions;
};

/**
 * WebAssembly Lifter
 * 
 * Converts a .wasm binary to LLVM IR using wasm2c + clang.
 * 
 * Usage:
 *   WasmLifter lifter;
 *   WasmLifterResult result = lifter.lift("/path/to/module.wasm");
 *   // result.bitcodePath now contains the LLVM bitcode
 */
class WasmLifter {
public:
  WasmLifter();
  explicit WasmLifter(const WasmLifterConfig& config);
  
  /**
   * Lift a WASM binary to LLVM IR
   * 
   * @param wasmPath Path to the .wasm file
   * @return Result containing paths to generated files
   * @throws WasmLifterError if any step fails
   */
  WasmLifterResult lift(const std::string& wasmPath);
  
  /**
   * Lift a WASM binary to LLVM IR and return the bitcode path
   * 
   * @param wasmPath Path to the .wasm file
   * @return Path to the generated .bc file
   * @throws WasmLifterError if any step fails
   */
  std::string liftToBitcode(const std::string& wasmPath);
  
  /**
   * Get the current configuration
   */
  const WasmLifterConfig& getConfig() const { return config_; }
  
  /**
   * Update configuration
   */
  void setConfig(const WasmLifterConfig& config) { config_ = config; }

private:
  WasmLifterConfig config_;
  
  /**
   * Execute wasm2c to convert .wasm to .c/.h
   */
  void runWasm2c(const std::string& wasmPath, 
                 const std::string& outputCPath,
                 const std::string& outputHPath);
  
  /**
   * Execute clang to compile .c to LLVM bitcode
   */
  void runClang(const std::string& cPath,
                const std::string& hPath,
                const std::string& outputBCPath);
  
  /**
   * Execute clang to compile .c to LLVM assembly (.ll)
   */
  void runClangEmitLLVM(const std::string& cPath,
                        const std::string& hPath,
                        const std::string& outputLLPath);
  
  /**
   * Extract entry point and exported functions from the lifted IR
   */
  void extractMetadata(const std::string& llPath,
                       WasmLifterResult& result);
  
  /**
   * Execute a shell command and check return code
   */
  void execOrThrow(const std::string& cmd, const std::string& errorMsg);
  
  /**
   * Generate a temporary file path
   */
  std::string tempPath(const std::string& suffix);
};

}  // namespace Map2Check

#endif  // WASM_LIFTER_HPP
