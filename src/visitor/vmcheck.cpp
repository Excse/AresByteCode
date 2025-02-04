// TODO: Make the error messages better

#include "../../include/vmcheck.h"

#include <iostream>

void ares::VMCheck::visitClass(ares::ClassInfo &classInfo) {
    if (classInfo.m_MagicNumber != 0xCAFEBABE) {
        std::cerr << "The magic number doesn't match \"0xCAFEBABE\"." << std::endl;
        abort();
    }

    if (classInfo.m_ClassVersion == ClassInfo::UNDEFINED) {
        std::cerr << "Couldn't set the class file version because it is an undefined value."
                  << std::endl;
        abort();
    }

    if (classInfo.m_ClassVersion > ClassInfo::VERSION_12
        && (classInfo.m_MinorVersion != 0 && classInfo.m_MinorVersion != 65535)) {
        std::cerr << "All Java 12 class files need a minor version of 0 or 65535." << std::endl;
        abort();
    }

    for (auto &constantPoolInfo : classInfo.m_ConstantPool) {
        if (constantPoolInfo == nullptr)
            continue;

        VMCheck::visitClassCPInfo(classInfo, *constantPoolInfo);
    }

    if (classInfo.hasAccessFlag(ClassInfo::INTERFACE)) {
        if (!classInfo.hasAccessFlag(ClassInfo::ABSTRACT)
            || classInfo.hasAccessFlag(ClassInfo::FINAL)
            || classInfo.hasAccessFlag(ClassInfo::SUPER)
            || classInfo.hasAccessFlag(ClassInfo::ENUM)
            || classInfo.hasAccessFlag(ClassInfo::MODULE)) {
            std::cerr << "The class file has invalid interface access flags." << std::endl;
            abort();
        }
    } else if (classInfo.hasAccessFlag(ClassInfo::ANNOTATION)) {
        if (!classInfo.hasAccessFlag(ClassInfo::INTERFACE)
            || classInfo.hasAccessFlag(ClassInfo::ABSTRACT)
            || classInfo.hasAccessFlag(ClassInfo::FINAL)) {
            std::cerr << "The class file has invalid annotation access flags." << std::endl;
            abort();
        }
    } else if (classInfo.hasAccessFlag(ClassInfo::MODULE)) {
        if (classInfo.hasAccessFlag(ClassInfo::ABSTRACT)
            || classInfo.hasAccessFlag(ClassInfo::FINAL)) {
            std::cerr << "The class file has invalid module access flags." << std::endl;
            abort();
        }
    }

    if (!classInfo.isIndexValid(classInfo.m_ThisClass)) {
        std::cerr << "The \"this class\" index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto thisClass = classInfo.m_ConstantPool[classInfo.m_ThisClass - 1];
    if (thisClass->m_Tag != ConstantPoolInfo::CLASS) {
        std::cerr << "The \"this class\" is not a class constant pool info." << std::endl;
        abort();
    }

    if (classInfo.m_SuperClass != 0) {
        if (!classInfo.isIndexValid(classInfo.m_SuperClass)) {
            std::cerr << "The \"super class\" index is not a valid constant pool index."
                      << std::endl;
            abort();
        }

        auto superClass = classInfo.m_ConstantPool[classInfo.m_SuperClass - 1];
        if (superClass->m_Tag != ConstantPoolInfo::CLASS) {
            std::cerr << "The \"super class\" is not a class constant pool info." << std::endl;
            abort();
        }
    }

    for (auto &interface : classInfo.m_Interfaces)
        VMCheck::visitClassInterface(classInfo, interface);

    for (auto &field : classInfo.m_Fields)
        VMCheck::visitClassField(classInfo, *field);

    for (auto &method : classInfo.m_Methods)
        VMCheck::visitClassMethod(classInfo, *method);

    for (auto &attribute : classInfo.m_Attributes)
        VMCheck::visitClassAttribute(classInfo, *attribute);
}

void ares::VMCheck::visitClassInterface(ares::ClassInfo &classInfo,
                                        uint16_t interface) {
    if (!classInfo.isIndexValid(interface)) {
        std::cerr << "The interface index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto constantPoolInfo = classInfo.m_ConstantPool[interface - 1];
    if (constantPoolInfo->m_Tag != ConstantPoolInfo::CLASS) {
        std::cerr << "The interface is not a class constant pool info." << std::endl;
        abort();
    }
}

void ares::VMCheck::visitClassField(ares::ClassInfo &classInfo,
                                    ares::FieldInfo &fieldInfo) {
    if (fieldInfo.hasAccessFlag(FieldInfo::PUBLIC)) {
        if (fieldInfo.hasAccessFlag(FieldInfo::PRIVATE)
            || fieldInfo.hasAccessFlag(FieldInfo::PROTECTED)) {
            std::cerr << "The field has invalid public access flags." << std::endl;
            abort();
        }
    } else if (fieldInfo.hasAccessFlag(FieldInfo::PRIVATE)) {
        if (fieldInfo.hasAccessFlag(FieldInfo::PUBLIC)
            || fieldInfo.hasAccessFlag(FieldInfo::PROTECTED)) {
            std::cerr << "The field has invalid private access flags." << std::endl;
            abort();
        }
    } else if (fieldInfo.hasAccessFlag(FieldInfo::PROTECTED)) {
        if (fieldInfo.hasAccessFlag(FieldInfo::PUBLIC)
            || fieldInfo.hasAccessFlag(FieldInfo::PRIVATE)) {
            std::cerr << "The field has invalid protected access flags." << std::endl;
            abort();
        }
    }

    if (classInfo.hasAccessFlag(ClassInfo::INTERFACE)) {
        if (!fieldInfo.hasAccessFlag(FieldInfo::PUBLIC)
            || !fieldInfo.hasAccessFlag(FieldInfo::STATIC)
            || !fieldInfo.hasAccessFlag(FieldInfo::FINAL)) {
            std::cerr << "Fields of interfaces need to have public, static and final access "
                         "modifier set." << std::endl;
            abort();
        }
    }

    if (!classInfo.isIndexValid(fieldInfo.m_NameIndex)) {
        std::cerr << "The name index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto fieldName = classInfo.m_ConstantPool[fieldInfo.m_NameIndex - 1];
    if (fieldName->m_Tag != ConstantPoolInfo::UTF_8) {
        std::cerr << "The name is not a utf8 class pool info." << std::endl;
        abort();
    }

    if (!classInfo.isIndexValid(fieldInfo.m_DescriptorIndex)) {
        std::cerr << "The descriptor index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto fieldDescriptor = classInfo.m_ConstantPool[fieldInfo.m_DescriptorIndex - 1];
    if (fieldDescriptor->m_Tag != ConstantPoolInfo::UTF_8) {
        std::cerr << "The descriptor is not a utf8 class pool info." << std::endl;
        abort();
    }

    for (auto &attribute : fieldInfo.m_Attributes)
        VMCheck::visitFieldAttribute(classInfo, fieldInfo, *attribute);
}

// TODO: Do checks for the descriptor_index in
//  https://docs.oracle.com/javase/specs/jvms/se15/html/jvms-4.html#jvms-4.6
void ares::VMCheck::visitClassMethod(ares::ClassInfo &classInfo,
                                     ares::MethodInfo &methodInfo) {
    if (methodInfo.hasAccessFlag(MethodInfo::PUBLIC)) {
        if (methodInfo.hasAccessFlag(MethodInfo::PRIVATE)
            || methodInfo.hasAccessFlag(MethodInfo::PROTECTED)) {
            std::cerr << "The method has invalid public access flags." << std::endl;
            abort();
        }
    } else if (methodInfo.hasAccessFlag(MethodInfo::PRIVATE)) {
        if (methodInfo.hasAccessFlag(MethodInfo::PUBLIC)
            || methodInfo.hasAccessFlag(MethodInfo::PROTECTED)) {
            std::cerr << "The method has invalid private access flags." << std::endl;
            abort();
        }
    } else if (methodInfo.hasAccessFlag(MethodInfo::PROTECTED)) {
        if (methodInfo.hasAccessFlag(MethodInfo::PUBLIC)
            || methodInfo.hasAccessFlag(MethodInfo::PRIVATE)) {
            std::cerr << "The method has invalid protected access flags." << std::endl;
            abort();
        }
    }

    if (classInfo.hasAccessFlag(ClassInfo::INTERFACE)) {
        if (methodInfo.hasAccessFlag(MethodInfo::PROTECTED)
            || methodInfo.hasAccessFlag(MethodInfo::FINAL)
            || methodInfo.hasAccessFlag(MethodInfo::SYNCHRONIZED)
            || methodInfo.hasAccessFlag(MethodInfo::NATIVE)) {
            std::cerr << "The access flags for an interface methods are invalid." << std::endl;
            abort();
        }

        if (classInfo.m_ClassVersion < ClassInfo::VERSION_8) {
            if (!methodInfo.hasAccessFlag(MethodInfo::PUBLIC)
                || !methodInfo.hasAccessFlag(MethodInfo::ABSTRACT)) {
                std::cerr << "The access flags for an interface methods are invalid."
                          << std::endl;
                abort();
            }
        } else if (classInfo.m_ClassVersion >= ClassInfo::VERSION_8) {
            if (methodInfo.hasAccessFlag(MethodInfo::PUBLIC)
                && methodInfo.hasAccessFlag(MethodInfo::PRIVATE)) {
                std::cerr << "The access flags for an interface methods are invalid."
                          << std::endl;
                abort();
            }
        }
    }

    if (methodInfo.hasAccessFlag(MethodInfo::ABSTRACT)) {
        if (methodInfo.hasAccessFlag(MethodInfo::PRIVATE)
            || methodInfo.hasAccessFlag(MethodInfo::STATIC)
            || methodInfo.hasAccessFlag(MethodInfo::FINAL)
            || methodInfo.hasAccessFlag(MethodInfo::SYNCHRONIZED)
            || methodInfo.hasAccessFlag(MethodInfo::NATIVE)
            || methodInfo.hasAccessFlag(MethodInfo::STRICT)) {
            std::cerr << "The access flags for an interface methods are invalid."
                      << std::endl;
            abort();
        }
    }

    if (!classInfo.isIndexValid(methodInfo.m_NameIndex)) {
        std::cerr << "The name index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto methodName = classInfo.m_ConstantPool[methodInfo.m_NameIndex - 1];
    if (methodName->m_Tag != ConstantPoolInfo::UTF_8) {
        std::cerr << "The name index is not a utf8 constant pool info." << std::endl;
        abort();
    }

    std::string name;
    name.assign((char *) methodName->m_Info.utf8Info.m_Bytes, methodName->m_Info.utf8Info.m_Length);
    if (name == "<init>") {
        if (methodInfo.hasAccessFlag(MethodInfo::ABSTRACT)
            || methodInfo.hasAccessFlag(MethodInfo::NATIVE)
            || methodInfo.hasAccessFlag(MethodInfo::BRIDGE)
            || methodInfo.hasAccessFlag(MethodInfo::SYNCHRONIZED)
            || methodInfo.hasAccessFlag(MethodInfo::FINAL)
            || methodInfo.hasAccessFlag(MethodInfo::STATIC)) {
            std::cerr << "The access flags for an interface methods are invalid."
                      << std::endl;
            abort();
        }
    }

    if (!classInfo.isIndexValid(methodInfo.m_DescriptorIndex)) {
        std::cerr << "The descriptor index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto methodDescriptor = classInfo.m_ConstantPool[methodInfo.m_DescriptorIndex - 1];
    if (methodDescriptor->m_Tag != ConstantPoolInfo::UTF_8) {
        std::cerr << "The name index is not a utf8 constant pool info." << std::endl;
        abort();
    }

    for (auto &attribute : methodInfo.m_Attributes)
        VMCheck::visitMethodAttribute(classInfo, methodInfo, *attribute);
}

void ares::VMCheck::visitClassAttribute(ares::ClassInfo &classInfo,
                                        ares::AttributeInfo &attributeInfo) {
    if (!classInfo.isIndexValid(attributeInfo.m_AttributeNameIndex)) {
        std::cerr << "The name index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto attributeName = classInfo.m_ConstantPool[attributeInfo.m_AttributeNameIndex - 1];
    if (attributeName->m_Tag != ConstantPoolInfo::UTF_8) {
        std::cerr << "The name index is not a utf8 class pool info." << std::endl;
        abort();
    }
}

void ares::VMCheck::visitFieldAttribute(ares::ClassInfo &classInfo,
                                        ares::FieldInfo &fieldInfo,
                                        ares::AttributeInfo &attributeInfo) {
    VMCheck::visitClassAttribute(classInfo, attributeInfo);
}

void ares::VMCheck::visitMethodAttribute(ares::ClassInfo &classInfo,
                                         ares::MethodInfo &methodInfo,
                                         ares::AttributeInfo &attributeInfo) {
    VMCheck::visitClassAttribute(classInfo, attributeInfo);
}

void ares::VMCheck::visitClassCPInfo(ares::ClassInfo &classInfo,
                                     ares::ConstantPoolInfo &constantPoolInfo) {
    switch (constantPoolInfo.m_Tag) {
        case ConstantPoolInfo::CLASS:
            VMCheck::visitClassInfo(classInfo, constantPoolInfo.m_Info.classInfo);
            break;
        case ConstantPoolInfo::FIELD_REF:
        case ConstantPoolInfo::METHOD_REF:
        case ConstantPoolInfo::INTERFACE_METHOD_REF:
            VMCheck::visitFieldMethodInfo(classInfo, constantPoolInfo.m_Info.fieldMethodInfo);
            break;
        case ConstantPoolInfo::NAME_AND_TYPE:
            VMCheck::visitNameAndTypeInfo(classInfo, constantPoolInfo.m_Info.nameAndTypeInfo);
            break;
        case ConstantPoolInfo::STRING:
            VMCheck::visitStringInfo(classInfo, constantPoolInfo.m_Info.stringInfo);
            break;
        case ConstantPoolInfo::METHOD_TYPE:
            VMCheck::visitMethodTypeInfo(classInfo, constantPoolInfo.m_Info.methodTypeInfo);
            break;
        case ConstantPoolInfo::METHOD_HANDLE:
            VMCheck::visitMethodHandleInfo(classInfo, constantPoolInfo.m_Info.methodHandleInfo);
            break;
        case ConstantPoolInfo::DYNAMIC:
        case ConstantPoolInfo::INVOKE_DYNAMIC:
            VMCheck::visitDynamicInfo(classInfo, constantPoolInfo.m_Info.dynamicInfo);
            break;
        case ConstantPoolInfo::MODULE:
        case ConstantPoolInfo::PACKAGE:
            VMCheck::visitModulePackageInfo(classInfo, constantPoolInfo.m_Info.modulePackageInfo);
            break;
        case ConstantPoolInfo::UTF_8:
        case ConstantPoolInfo::INTEGER:
        case ConstantPoolInfo::FLOAT:
        case ConstantPoolInfo::LONG:
        case ConstantPoolInfo::DOUBLE:
            break;
        default:
            std::cerr << "Case for constant pool tag \"" << (int) constantPoolInfo.m_Tag
                      << "\" not implemented yet." << std::endl;
            abort();
    }
}

void ares::VMCheck::visitClassInfo(ares::ClassInfo &classInfo,
                                   ares::ConstantInfo::ClassInfo &info) {
    if (!classInfo.isIndexValid(info.m_NameIndex)) {
        std::cout << "The name index is not a valid constant pool index." << std::endl;
        abort();
    }
}

void ares::VMCheck::visitFieldMethodInfo(ares::ClassInfo &classInfo,
                                         ares::ConstantInfo::FieldMethodInfo &info) {
    if (!classInfo.isIndexValid(info.m_ClassIndex)) {
        std::cerr << "The class index is not a valid constant pool index." << std::endl;
        abort();
    }

    if (!classInfo.isIndexValid(info.m_NameAndTypeIndex)) {
        std::cerr << "The name and type index is not a valid constant pool index." << std::endl;
        abort();
    }
}

void ares::VMCheck::visitNameAndTypeInfo(ares::ClassInfo &classInfo,
                                         ares::ConstantInfo::NameAndTypeInfo &info) {
    if (!classInfo.isIndexValid(info.m_NameIndex)) {
        std::cerr << "The name index is not a valid constant pool index." << std::endl;
        abort();
    }

    if (!classInfo.isIndexValid(info.m_DescriptorIndex)) {
        std::cerr << "The descriptor index is not a valid constant pool index." << std::endl;
        abort();
    }
}

void ares::VMCheck::visitStringInfo(ares::ClassInfo &classInfo,
                                    ares::ConstantInfo::StringInfo &info) {
    if (!classInfo.isIndexValid(info.m_StringIndex)) {
        std::cerr << "The string index is not a valid constant pool index." << std::endl;
        abort();
    }
}

void ares::VMCheck::visitMethodTypeInfo(ares::ClassInfo &classInfo,
                                        ares::ConstantInfo::MethodTypeInfo &info) {
    if (!classInfo.isIndexValid(info.m_DescriptorIndex)) {
        std::cerr << "The descriptor index is not a valid constant pool index" << std::endl;
        abort();
    }
}

void ares::VMCheck::visitMethodHandleInfo(ares::ClassInfo &classInfo,
                                          ares::ConstantInfo::MethodHandleInfo &info) {
    if (info.m_ReferenceKind < 1 || info.m_ReferenceKind > 9) {
        std::cerr << "The reference kind is not in range of 0 to 9." << std::endl;
        abort();
    }

    if (!classInfo.isIndexValid(info.m_ReferenceIndex)) {
        std::cerr << "The reference index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto constantPoolInfo = classInfo.m_ConstantPool[info.m_ReferenceIndex - 1];
    auto referenceKind = info.m_ReferenceKind;

    if (referenceKind == ConstantInfo::MethodHandleKind::GetField
        || referenceKind == ConstantInfo::MethodHandleKind::GetStatic
        || referenceKind == ConstantInfo::MethodHandleKind::PutField
        || referenceKind == ConstantInfo::MethodHandleKind::PutStatic) {
        if (constantPoolInfo->m_Tag != ConstantPoolInfo::FIELD_REF) {
            std::cerr << "The reference index of the method handle needs to be a field ref."
                      << std::endl;
            abort();
        }
    } else if (referenceKind == ConstantInfo::MethodHandleKind::InvokeVirtual
               || referenceKind == ConstantInfo::MethodHandleKind::NewInvokeSpecial) {
        if (constantPoolInfo->m_Tag != ConstantPoolInfo::FIELD_REF) {
            std::cerr << "The reference index of the method handle needs to be a method ref."
                      << std::endl;
            abort();
        }
    } else if (referenceKind == ConstantInfo::MethodHandleKind::InvokeStatic
               || referenceKind == ConstantInfo::MethodHandleKind::InvokeSpecial) {
        if (classInfo.m_ClassVersion < ClassInfo::VERSION_8
            && constantPoolInfo->m_Tag != ConstantPoolInfo::METHOD_REF) {
            std::cerr << "The reference index of the method handle needs to be a method ref."
                      << std::endl;
            abort();
        } else if (constantPoolInfo->m_Tag != ConstantPoolInfo::METHOD_REF
                   && constantPoolInfo->m_Tag != ConstantPoolInfo::INTERFACE_METHOD_REF) {
            std::cerr << "The reference index of the method handle needs to be a method ref or "
                         "interface method ref." << std::endl;
            abort();
        }
    } else if (referenceKind == ConstantInfo::MethodHandleKind::InvokeInterface) {
        if (constantPoolInfo->m_Tag != ConstantPoolInfo::INTERFACE_METHOD_REF) {
            std::cerr << "The reference index of the method handle needs to be a interface method "
                         "ref." << std::endl;
            abort();
        }
    }

    if (referenceKind == ConstantInfo::MethodHandleKind::InvokeVirtual
        || referenceKind == ConstantInfo::MethodHandleKind::InvokeStatic
        || referenceKind == ConstantInfo::MethodHandleKind::InvokeSpecial
        || referenceKind == ConstantInfo::MethodHandleKind::InvokeInterface
        || referenceKind == ConstantInfo::MethodHandleKind::NewInvokeSpecial) {
        auto nameAndType = classInfo.m_ConstantPool[
                constantPoolInfo->m_Info.fieldMethodInfo.m_NameAndTypeIndex - 1];
        auto nameUTF8 = classInfo.m_ConstantPool[
                nameAndType->m_Info.nameAndTypeInfo.m_NameIndex - 1];

        std::string name;
        name.assign((char *) nameUTF8->m_Info.utf8Info.m_Bytes, nameUTF8->m_Info.utf8Info.m_Length);

        if (referenceKind == ConstantInfo::MethodHandleKind::NewInvokeSpecial) {
            if (name != "<init>") {
                std::cerr << "The name of the method ref must be \"<init>\"" << std::endl;
                abort();
            }
        } else {
            if (name == "<init>" || name == "<clinit>") {
                std::cerr << R"(The name of the method ref can't be "<init>" or "<clinit>".)"
                          << std::endl;
                abort();
            }
        }
    }
}

// TODO: Check if the bootstrap method index if correct.
void ares::VMCheck::visitDynamicInfo(ares::ClassInfo &classInfo,
                                     ares::ConstantInfo::DynamicInfo &info) {
    if (!classInfo.isIndexValid(info.m_NameAndTypeIndex)) {
        std::cerr << "The name and type index is not a valid constant pool index." << std::endl;
        abort();
    }
}

void ares::VMCheck::visitModulePackageInfo(ares::ClassInfo &classInfo,
                                           ares::ConstantInfo::ModulePackageInfo &info) {
    if (!classInfo.isIndexValid(info.m_NameIndex)) {
        std::cerr << "The name index is not a valid constant pool index." << std::endl;
        abort();
    }
}

//==============================================================================
// BSD 3-Clause License
//
// Copyright (c) 2025, Timo Behrend
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//==============================================================================