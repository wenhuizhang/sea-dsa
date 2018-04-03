#ifndef SEA_DSA_FIELD_TYPE
#define SEA_DSA_FIELD_TYPE

#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/raw_ostream.h"

namespace sea_dsa {

llvm::Type *GetFirstPrimitiveTy(llvm::Type *Ty);

class FieldType {
  llvm::Type *m_ty = nullptr;
  bool m_isOpaque = false;
  bool m_NOT_IMPLEMENTED = false;

  FieldType() : m_NOT_IMPLEMENTED(true) {}
public:
  static FieldType NotImplemented() { return {}; }

  explicit FieldType(llvm::Type *Ty, bool IsOpaque = false)
      : m_isOpaque(IsOpaque) {
    if (Ty)
      m_ty = GetFirstPrimitiveTy(Ty);
  }

  FieldType(const FieldType &) = default;
  FieldType &operator=(const FieldType &) = default;

  bool isData() const { return !m_ty->isPointerTy(); }
  bool isPointer() const { return m_ty->isPointerTy(); }
  bool isNull() const { return !m_ty; }
  bool isOpaque() const { return m_isOpaque; }

  bool operator==(const FieldType &RHS) const {
    return m_ty == RHS.m_ty && m_isOpaque == RHS.m_isOpaque;
  }

  FieldType ptrOf() const {
    assert(!isOpaque());
    assert(!isNull());
    auto *PtrTy = llvm::PointerType::get(m_ty, 0);
    return FieldType{PtrTy, false};
  }

  FieldType elemOf() const {
    assert(!isOpaque());
    assert(!isNull());
    assert(isPointer());

    auto *NewTy = m_ty->getPointerElementType();
    return FieldType{NewTy, false};
  }

  void dump(llvm::raw_ostream &OS = llvm::errs()) const {
    if (m_NOT_IMPLEMENTED) {
      OS << "TODO";
      return;
    }

    if (isOpaque()) {
      OS << "opaque";
      return;
    }

    if (isNull()) {
      OS << "null";
      return;
    }

    m_ty->print(OS, false);
  }

  friend llvm::raw_ostream &operator<<(llvm::raw_ostream &OS,
                                       const FieldType &FTy) {
    FTy.dump(OS);
    return OS;
  }
};

} // namespace sea_dsa

#endif // SEA_DSA_FIELD_TYPE