#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include <cstddef>
#include <string>

using namespace llvm;

#define DEBUG_TYPE "ppprofiler"

ALWAYS_ENABLED_STATISTIC(NumOfFunc, "Number of instrumented functions.");

namespace {
struct PPProfilerOptions {
  bool InstrumentEnter = true;
  bool IntrumentExit = true;
  int InstrumentWithNumArgs = -1;
  std::vector<std::string> OnlyFunctions;
};

class PPProfilerIRPass : public llvm::PassInfoMixin<PPProfilerIRPass> {
public:
  explicit PPProfilerIRPass(PPProfilerOptions Options = {}) : Options(std::move(Options)) {}
  llvm::PreservedAnalyses run(llvm::Module &M, llvm::ModuleAnalysisManager &AM);

private:
  PPProfilerOptions Options;
  bool shouldInstrument(const llvm::Function &F) const;
  void instrument(llvm::Function &F, llvm::Function *EnterFn, llvm::Function *ExitFn);
};
} // namespace

bool PPProfilerIRPass::shouldInstrument(const llvm::Function &F) const {
  if (F.isDeclaration() || !F.hasName())
    return false;

  const auto NumArgs = F.arg_size();
  if (Options.InstrumentWithNumArgs >= 0)
    if (NumArgs != size_t(Options.InstrumentWithNumArgs))
      return false;

  if (Options.OnlyFunctions.empty())
    return true;

  const std::string &Name = F.getName().str();
  if (std::find(Options.OnlyFunctions.begin(), Options.OnlyFunctions.end(), Name) == Options.OnlyFunctions.end())
    return false;
  return true;
}

void PPProfilerIRPass::instrument(llvm::Function &F, Function *EnterFn, Function *ExitFn) {
  ++NumOfFunc;

  // Set the insertion point to begin of first block.
  IRBuilder<> Builder(&*F.getEntryBlock().begin());

  // Create global constant for the function name.
  GlobalVariable *FnName = Builder.CreateGlobalString(F.getName());

  // Call the EnterFn at function entry.
  if (Options.InstrumentEnter)
    Builder.CreateCall(EnterFn->getFunctionType(), EnterFn, {FnName});

  // Find all Ret instructions, and call ExitFn before.
  if (Options.IntrumentExit)
    for (BasicBlock &BB : F) {
      for (Instruction &Inst : BB) {
        if (Inst.getOpcode() == Instruction::Ret) {
          Builder.SetInsertPoint(&Inst);
          Builder.CreateCall(ExitFn->getFunctionType(), ExitFn, {FnName});
        }
      }
    }
}

PreservedAnalyses PPProfilerIRPass::run(Module &M, ModuleAnalysisManager &AM) {
  // Do not instrument the runtime functions.
  if (M.getFunction("__ppp_enter") || M.getFunction("__ppp_exit")) {
    // no transformations
    return PreservedAnalyses::all();
  }

  // Create the function type and functions.
  Type *VoidTy = Type::getVoidTy(M.getContext());
  PointerType *PtrTy = PointerType::getUnqual(M.getContext());
  FunctionType *EnterExitFty = FunctionType::get(VoidTy, {PtrTy}, false);
  Function *EnterFn = Function::Create(EnterExitFty, GlobalValue::ExternalLinkage, "__ppp_enter", M);
  Function *ExitFn = Function::Create(EnterExitFty, GlobalValue::ExternalLinkage, "__ppp_exit", M);

  // Call enter function.
  for (auto &F : M.functions()) {
    if (shouldInstrument(F))
      instrument(F, EnterFn, ExitFn);
  }
  return PreservedAnalyses::none();
}

void RegisterCB(PassBuilder &PB) {
  PB.registerPipelineParsingCallback([](StringRef Name, ModulePassManager &MPM, ArrayRef<PassBuilder::PipelineElement>) {
    if (Name.consume_front("ppprofiler")) {
      PPProfilerOptions Options;
      if (Name.consume_front("<")) {
        Name = Name.rtrim(">");
        SmallVector<StringRef, 4> Args;
        Name.split(Args, ';');

        for (auto &Arg : Args) {
          Arg = Arg.trim();
          if (Arg.consume_front("no_enter")) {
            Options.InstrumentEnter = false;
          } else if (Arg.consume_front("no_exit")) {
            Options.IntrumentExit = false;
          } else if (Arg.consume_front("only=")) {
            Options.OnlyFunctions.push_back(Arg.str());
          } else if (Arg.consume_front("with_args_num=")) {
            Options.InstrumentWithNumArgs = std::stoi(Arg.str());
          }
        }
      }
      MPM.addPass(PPProfilerIRPass(std::move(Options)));
      return true;
    }
    return false;
  });
  PB.registerPipelineStartEPCallback([](ModulePassManager &PM, OptimizationLevel Level) { PM.addPass(PPProfilerIRPass()); });
}

llvm::PassPluginLibraryInfo getPPProfilerPluginInfo() { return {LLVM_PLUGIN_API_VERSION, "PPProfiler", "v0.1", RegisterCB}; }

#ifndef LLVM_PPPROFILER_LINK_INTO_TOOLS
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() { return getPPProfilerPluginInfo(); }
#endif
