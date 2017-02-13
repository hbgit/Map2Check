//===- llvm-build generated file --------------------------------*- C++ -*-===//
//
// Component Library Depenedency Table
//
// Automatically generated file, do not edit!
//
//===----------------------------------------------------------------------===//

struct AvailableComponent {
  /// The name of the component.
  const char *Name;

  /// The name of the library for this component (or NULL).
  const char *Library;

  /// Whether the component is installed.
  bool IsInstalled;

  /// The list of libraries required when linking this component.
  const char *RequiredLibraries[15];
} AvailableComponents[55] = {
  { "all", nullptr, true, { "interpreter", "debuginfocodeview", "lineeditor", "engine", "passes", "nativecodegen", "orcjit", "tablegen", "gtest_main", "libdriver", "mirparser", "all-targets", "symbolize", "lto" } },
  { "all-targets", nullptr, true, { "x86" } },
  { "analysis", "libLLVMAnalysis.a", true, { "core", "support" } },
  { "asmparser", "libLLVMAsmParser.a", true, { "core", "support" } },
  { "asmprinter", "libLLVMAsmPrinter.a", true, { "analysis", "codegen", "core", "mc", "mcparser", "support", "target", "transformutils" } },
  { "bitreader", "libLLVMBitReader.a", true, { "core", "support" } },
  { "bitwriter", "libLLVMBitWriter.a", true, { "core", "support" } },
  { "codegen", "libLLVMCodeGen.a", true, { "analysis", "bitreader", "bitwriter", "core", "instrumentation", "mc", "scalaropts", "support", "target", "transformutils" } },
  { "core", "libLLVMCore.a", true, { "support" } },
  { "debuginfocodeview", "libLLVMDebugInfoCodeView.a", true, { "support" } },
  { "debuginfodwarf", "libLLVMDebugInfoDWARF.a", true, { "object", "support" } },
  { "debuginfopdb", "libLLVMDebugInfoPDB.a", true, { "object", "support" } },
  { "engine", nullptr, true, { "mcjit", "native" } },
  { "executionengine", "libLLVMExecutionEngine.a", true, { "core", "mc", "object", "runtimedyld", "support", "target" } },
  { "gtest", "libgtest.a", false, { "support" } },
  { "gtest_main", "libgtest_main.a", false, { "gtest" } },
  { "instcombine", "libLLVMInstCombine.a", true, { "analysis", "core", "support", "transformutils" } },
  { "instrumentation", "libLLVMInstrumentation.a", true, { "analysis", "core", "mc", "support", "transformutils", "profiledata" } },
  { "interpreter", "libLLVMInterpreter.a", true, { "codegen", "core", "executionengine", "support" } },
  { "ipo", "libLLVMipo.a", true, { "analysis", "core", "instcombine", "irreader", "linker", "object", "profiledata", "scalaropts", "support", "transformutils", "vectorize" } },
  { "irreader", "libLLVMIRReader.a", true, { "asmparser", "bitreader", "core", "support" } },
  { "libdriver", "libLLVMLibDriver.a", true, { "object", "option", "support" } },
  { "lineeditor", "libLLVMLineEditor.a", true, { "support" } },
  { "linker", "libLLVMLinker.a", true, { "core", "support", "transformutils" } },
  { "lto", "libLLVMLTO.a", true, { "analysis", "bitreader", "bitwriter", "codegen", "core", "ipo", "instcombine", "linker", "mc", "objcarcopts", "object", "scalaropts", "support", "target" } },
  { "mc", "libLLVMMC.a", true, { "support" } },
  { "mcdisassembler", "libLLVMMCDisassembler.a", true, { "mc", "support" } },
  { "mcjit", "libLLVMMCJIT.a", true, { "core", "executionengine", "object", "runtimedyld", "support", "target" } },
  { "mcparser", "libLLVMMCParser.a", true, { "mc", "support" } },
  { "mirparser", "libLLVMMIRParser.a", true, { "asmparser", "codegen", "core", "mc", "support", "target" } },
  { "native", nullptr, true, { "x86" } },
  { "nativecodegen", nullptr, true, { "x86codegen" } },
  { "objcarcopts", "libLLVMObjCARCOpts.a", true, { "analysis", "core", "support", "transformutils" } },
  { "object", "libLLVMObject.a", true, { "bitreader", "core", "mc", "mcparser", "support" } },
  { "option", "libLLVMOption.a", true, { "support" } },
  { "orcjit", "libLLVMOrcJIT.a", true, { "core", "executionengine", "object", "runtimedyld", "support", "transformutils" } },
  { "passes", "libLLVMPasses.a", true, { "analysis", "core", "ipo", "instcombine", "scalaropts", "support", "transformutils", "vectorize" } },
  { "profiledata", "libLLVMProfileData.a", true, { "core", "support", "object" } },
  { "runtimedyld", "libLLVMRuntimeDyld.a", true, { "mc", "object", "support" } },
  { "scalaropts", "libLLVMScalarOpts.a", true, { "analysis", "core", "instcombine", "support", "transformutils" } },
  { "selectiondag", "libLLVMSelectionDAG.a", true, { "analysis", "codegen", "core", "mc", "support", "target", "transformutils" } },
  { "support", "libLLVMSupport.a", true, {  } },
  { "symbolize", "libLLVMSymbolize.a", true, { "debuginfodwarf", "debuginfopdb", "object", "support" } },
  { "tablegen", "libLLVMTableGen.a", true, { "support" } },
  { "target", "libLLVMTarget.a", true, { "analysis", "core", "mc", "support" } },
  { "transformutils", "libLLVMTransformUtils.a", true, { "analysis", "core", "support" } },
  { "vectorize", "libLLVMVectorize.a", true, { "analysis", "core", "support", "transformutils" } },
  { "x86", nullptr, true, { "x86utils", "x86asmprinter", "x86info", "x86desc", "x86codegen", "x86asmparser", "x86disassembler" } },
  { "x86asmparser", "libLLVMX86AsmParser.a", true, { "mc", "mcparser", "support", "x86desc", "x86info" } },
  { "x86asmprinter", "libLLVMX86AsmPrinter.a", true, { "mc", "support", "x86utils" } },
  { "x86codegen", "libLLVMX86CodeGen.a", true, { "analysis", "asmprinter", "codegen", "core", "mc", "selectiondag", "support", "target", "x86asmprinter", "x86desc", "x86info", "x86utils" } },
  { "x86desc", "libLLVMX86Desc.a", true, { "mc", "mcdisassembler", "object", "support", "x86asmprinter", "x86info" } },
  { "x86disassembler", "libLLVMX86Disassembler.a", true, { "mcdisassembler", "support", "x86info" } },
  { "x86info", "libLLVMX86Info.a", true, { "support" } },
  { "x86utils", "libLLVMX86Utils.a", true, { "core", "support" } },
};
