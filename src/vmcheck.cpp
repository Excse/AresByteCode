#include "vmcheck.h"

#include <iostream>

void ares::VMCheck::visit_class(ClassInfo &classInfo) {
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

        VMCheck::visit_classpool_info(classInfo, *constantPoolInfo);
    }

    if (classInfo.has_access_flags(ClassInfo::INTERFACE)) {
        if (!classInfo.has_access_flags(ClassInfo::ABSTRACT)
            || classInfo.has_access_flags(ClassInfo::FINAL)
            || classInfo.has_access_flags(ClassInfo::SUPER)
            || classInfo.has_access_flags(ClassInfo::ENUM)
            || classInfo.has_access_flags(ClassInfo::MODULE)) {
            std::cerr << "The class file has invalid interface access flags." << std::endl;
            abort();
        }
    } else if (classInfo.has_access_flags(ClassInfo::ANNOTATION)) {
        if (!classInfo.has_access_flags(ClassInfo::INTERFACE)
            || classInfo.has_access_flags(ClassInfo::ABSTRACT)
            || classInfo.has_access_flags(ClassInfo::FINAL)) {
            std::cerr << "The class file has invalid annotation access flags." << std::endl;
            abort();
        }
    } else if (classInfo.has_access_flags(ClassInfo::MODULE)) {
        if (classInfo.has_access_flags(ClassInfo::ABSTRACT)
            || classInfo.has_access_flags(ClassInfo::FINAL)) {
            std::cerr << "The class file has invalid module access flags." << std::endl;
            abort();
        }
    }

    if (!classInfo.is_valid_index(classInfo.m_ThisClass)) {
        std::cerr << "The \"this class\" index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto thisClass = classInfo.m_ConstantPool[classInfo.m_ThisClass - 1];
    if (thisClass->m_Tag != ConstantPoolInfo::CLASS) {
        std::cerr << "The \"this class\" is not a class constant pool info." << std::endl;
        abort();
    }

    if (classInfo.m_SuperClass != 0) {
        if (!classInfo.is_valid_index(classInfo.m_SuperClass)) {
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
        VMCheck::visit_class_interface(classInfo, interface);

    for (auto &field : classInfo.m_Fields)
        VMCheck::visit_class_field(classInfo, *field);

    for (auto &method : classInfo.m_Methods)
        VMCheck::visit_class_method(classInfo, *method);

    for (auto &attribute : classInfo.m_Attributes)
        VMCheck::visit_class_attribute(classInfo, *attribute);
}

void ares::VMCheck::visit_class_interface(ClassInfo &classInfo,
                                          uint16_t interface) {
    if (!classInfo.is_valid_index(interface)) {
        std::cerr << "The interface index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto constantPoolInfo = classInfo.m_ConstantPool[interface - 1];
    if (constantPoolInfo->m_Tag != ConstantPoolInfo::CLASS) {
        std::cerr << "The interface is not a class constant pool info." << std::endl;
        abort();
    }
}

void ares::VMCheck::visit_class_field(ClassInfo &classInfo,
                                      FieldInfo &fieldInfo) {
    if (fieldInfo.has_access_flags(FieldInfo::PUBLIC)) {
        if (fieldInfo.has_access_flags(FieldInfo::PRIVATE)
            || fieldInfo.has_access_flags(FieldInfo::PROTECTED)) {
            std::cerr << "The field has invalid public access flags." << std::endl;
            abort();
        }
    } else if (fieldInfo.has_access_flags(FieldInfo::PRIVATE)) {
        if (fieldInfo.has_access_flags(FieldInfo::PUBLIC)
            || fieldInfo.has_access_flags(FieldInfo::PROTECTED)) {
            std::cerr << "The field has invalid private access flags." << std::endl;
            abort();
        }
    } else if (fieldInfo.has_access_flags(FieldInfo::PROTECTED)) {
        if (fieldInfo.has_access_flags(FieldInfo::PUBLIC)
            || fieldInfo.has_access_flags(FieldInfo::PRIVATE)) {
            std::cerr << "The field has invalid protected access flags." << std::endl;
            abort();
        }
    }

    if (classInfo.has_access_flags(ClassInfo::INTERFACE)) {
        if (!fieldInfo.has_access_flags(FieldInfo::PUBLIC)
            || !fieldInfo.has_access_flags(FieldInfo::STATIC)
            || !fieldInfo.has_access_flags(FieldInfo::FINAL)) {
            std::cerr << "Fields of interfaces need to have public, static and final access "
                         "modifier set." << std::endl;
            abort();
        }
    }

    if (!classInfo.is_valid_index(fieldInfo.m_NameIndex)) {
        std::cerr << "The name index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto fieldName = classInfo.m_ConstantPool[fieldInfo.m_NameIndex - 1];
    if (fieldName->m_Tag != ConstantPoolInfo::UTF_8) {
        std::cerr << "The name is not a utf8 class pool info." << std::endl;
        abort();
    }

    if (!classInfo.is_valid_index(fieldInfo.m_DescriptorIndex)) {
        std::cerr << "The descriptor index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto fieldDescriptor = classInfo.m_ConstantPool[fieldInfo.m_DescriptorIndex - 1];
    if (fieldDescriptor->m_Tag != ConstantPoolInfo::UTF_8) {
        std::cerr << "The descriptor is not a utf8 class pool info." << std::endl;
        abort();
    }

    for (auto &attribute : fieldInfo.m_Attributes)
        VMCheck::visit_field_attribute(classInfo, fieldInfo, *attribute);
}

// TODO: Do checks for the descriptor_index in
//  https://docs.oracle.com/javase/specs/jvms/se15/html/jvms-4.html#jvms-4.6
void ares::VMCheck::visit_class_method(ClassInfo &classInfo,
                                       MethodInfo &methodInfo) {
    if (methodInfo.has_access_flags(MethodInfo::PUBLIC)) {
        if (methodInfo.has_access_flags(MethodInfo::PRIVATE)
            || methodInfo.has_access_flags(MethodInfo::PROTECTED)) {
            std::cerr << "The method has invalid public access flags." << std::endl;
            abort();
        }
    } else if (methodInfo.has_access_flags(MethodInfo::PRIVATE)) {
        if (methodInfo.has_access_flags(MethodInfo::PUBLIC)
            || methodInfo.has_access_flags(MethodInfo::PROTECTED)) {
            std::cerr << "The method has invalid private access flags." << std::endl;
            abort();
        }
    } else if (methodInfo.has_access_flags(MethodInfo::PROTECTED)) {
        if (methodInfo.has_access_flags(MethodInfo::PUBLIC)
            || methodInfo.has_access_flags(MethodInfo::PRIVATE)) {
            std::cerr << "The method has invalid protected access flags." << std::endl;
            abort();
        }
    }

    if (classInfo.has_access_flags(ClassInfo::INTERFACE)) {
        if (methodInfo.has_access_flags(MethodInfo::PROTECTED)
            || methodInfo.has_access_flags(MethodInfo::FINAL)
            || methodInfo.has_access_flags(MethodInfo::SYNCHRONIZED)
            || methodInfo.has_access_flags(MethodInfo::NATIVE)) {
            std::cerr << "The access flags for an interface methods are invalid." << std::endl;
            abort();
        }

        if (classInfo.m_ClassVersion < ClassInfo::VERSION_8) {
            if (!methodInfo.has_access_flags(MethodInfo::PUBLIC)
                || !methodInfo.has_access_flags(MethodInfo::ABSTRACT)) {
                std::cerr << "The access flags for an interface methods are invalid."
                          << std::endl;
                abort();
            }
        } else if (classInfo.m_ClassVersion >= ClassInfo::VERSION_8) {
            if (methodInfo.has_access_flags(MethodInfo::PUBLIC)
                && methodInfo.has_access_flags(MethodInfo::PRIVATE)) {
                std::cerr << "The access flags for an interface methods are invalid."
                          << std::endl;
                abort();
            }
        }
    }

    if (methodInfo.has_access_flags(MethodInfo::ABSTRACT)) {
        if (methodInfo.has_access_flags(MethodInfo::PRIVATE)
            || methodInfo.has_access_flags(MethodInfo::STATIC)
            || methodInfo.has_access_flags(MethodInfo::FINAL)
            || methodInfo.has_access_flags(MethodInfo::SYNCHRONIZED)
            || methodInfo.has_access_flags(MethodInfo::NATIVE)
            || methodInfo.has_access_flags(MethodInfo::STRICT)) {
            std::cerr << "The access flags for an interface methods are invalid."
                      << std::endl;
            abort();
        }
    }

    if (!classInfo.is_valid_index(methodInfo.m_NameIndex)) {
        std::cerr << "The name index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto methodName = classInfo.m_ConstantPool[methodInfo.m_NameIndex - 1];
    if (methodName->m_Tag != ConstantPoolInfo::UTF_8) {
        std::cerr << "The name index is not a utf8 constant pool info." << std::endl;
        abort();
    }

    std::string name;
    name.assign((char *) methodName->m_Info.utf8_info.m_Bytes, methodName->m_Info.utf8_info.m_Length);
    if (name == "<init>") {
        if (methodInfo.has_access_flags(MethodInfo::ABSTRACT)
            || methodInfo.has_access_flags(MethodInfo::NATIVE)
            || methodInfo.has_access_flags(MethodInfo::BRIDGE)
            || methodInfo.has_access_flags(MethodInfo::SYNCHRONIZED)
            || methodInfo.has_access_flags(MethodInfo::FINAL)
            || methodInfo.has_access_flags(MethodInfo::STATIC)) {
            std::cerr << "The access flags for an interface methods are invalid."
                      << std::endl;
            abort();
        }
    }

    if (!classInfo.is_valid_index(methodInfo.m_DescriptorIndex)) {
        std::cerr << "The descriptor index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto methodDescriptor = classInfo.m_ConstantPool[methodInfo.m_DescriptorIndex - 1];
    if (methodDescriptor->m_Tag != ConstantPoolInfo::UTF_8) {
        std::cerr << "The name index is not a utf8 constant pool info." << std::endl;
        abort();
    }

    for (auto &attribute : methodInfo.m_Attributes)
        VMCheck::visit_method_attribute(classInfo, methodInfo, *attribute);
}

void ares::VMCheck::visit_class_attribute(ClassInfo &classInfo,
                                          AttributeInfo &attributeInfo) {
    if (!classInfo.is_valid_index(attributeInfo.m_AttributeNameIndex)) {
        std::cerr << "The name index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto attributeName = classInfo.m_ConstantPool[attributeInfo.m_AttributeNameIndex - 1];
    if (attributeName->m_Tag != ConstantPoolInfo::UTF_8) {
        std::cerr << "The name index is not a utf8 class pool info." << std::endl;
        abort();
    }
}

void ares::VMCheck::visit_field_attribute(ClassInfo &classInfo,
                                          FieldInfo &,
                                          AttributeInfo &attributeInfo) {
    VMCheck::visit_class_attribute(classInfo, attributeInfo);
}

void ares::VMCheck::visit_method_attribute(ClassInfo &classInfo,
                                           MethodInfo &,
                                           AttributeInfo &attributeInfo) {
    VMCheck::visit_class_attribute(classInfo, attributeInfo);
}

void ares::VMCheck::visit_classpool_info(ClassInfo &classInfo,
                                         ConstantPoolInfo &constantPoolInfo) {
    switch (constantPoolInfo.m_Tag) {
        case ConstantPoolInfo::CLASS:VMCheck::visit_class_info(classInfo, constantPoolInfo.m_Info.class_info);
            break;
        case ConstantPoolInfo::FIELD_REF:
        case ConstantPoolInfo::METHOD_REF:
        case ConstantPoolInfo::INTERFACE_METHOD_REF:
            VMCheck::visit_field_method_info(classInfo, constantPoolInfo.m_Info.field_method_info);
            break;
        case ConstantPoolInfo::NAME_AND_TYPE:
            VMCheck::visit_name_and_type_info(classInfo, constantPoolInfo.m_Info.name_and_type_info);
            break;
        case ConstantPoolInfo::STRING:VMCheck::visit_string_info(classInfo, constantPoolInfo.m_Info.string_info);
            break;
        case ConstantPoolInfo::METHOD_TYPE:
            VMCheck::visit_method_type_info(classInfo, constantPoolInfo.m_Info.method_type_info);
            break;
        case ConstantPoolInfo::METHOD_HANDLE:
            VMCheck::visit_method_handle_info(classInfo, constantPoolInfo.m_Info.method_handle_info);
            break;
        case ConstantPoolInfo::DYNAMIC:
        case ConstantPoolInfo::INVOKE_DYNAMIC:
            VMCheck::visit_dynamic_info(classInfo, constantPoolInfo.m_Info.dynamic_info);
            break;
        case ConstantPoolInfo::MODULE:
        case ConstantPoolInfo::PACKAGE:
            VMCheck::visit_module_package_info(classInfo, constantPoolInfo.m_Info.module_package_info);
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

void ares::VMCheck::visit_class_info(ares::ClassInfo &classInfo,
                                     ares::ConstantInfo::ClassInfo &info) {
    if (!classInfo.is_valid_index(info.m_NameIndex)) {
        std::cout << "The name index is not a valid constant pool index." << std::endl;
        abort();
    }
}

void ares::VMCheck::visit_field_method_info(ares::ClassInfo &classInfo,
                                            ares::ConstantInfo::FieldMethodInfo &info) {
    if (!classInfo.is_valid_index(info.m_ClassIndex)) {
        std::cerr << "The class index is not a valid constant pool index." << std::endl;
        abort();
    }

    if (!classInfo.is_valid_index(info.m_NameAndTypeIndex)) {
        std::cerr << "The name and type index is not a valid constant pool index." << std::endl;
        abort();
    }
}

void ares::VMCheck::visit_name_and_type_info(ClassInfo &classInfo,
                                             ConstantInfo::NameAndTypeInfo &info) {
    if (!classInfo.is_valid_index(info.m_NameIndex)) {
        std::cerr << "The name index is not a valid constant pool index." << std::endl;
        abort();
    }

    if (!classInfo.is_valid_index(info.m_DescriptorIndex)) {
        std::cerr << "The descriptor index is not a valid constant pool index." << std::endl;
        abort();
    }
}

void ares::VMCheck::visit_string_info(ClassInfo &classInfo,
                                      ConstantInfo::StringInfo &info) {
    if (!classInfo.is_valid_index(info.m_StringIndex)) {
        std::cerr << "The string index is not a valid constant pool index." << std::endl;
        abort();
    }
}

void ares::VMCheck::visit_method_type_info(ClassInfo &classInfo,
                                           ConstantInfo::MethodTypeInfo &info) {
    if (!classInfo.is_valid_index(info.m_DescriptorIndex)) {
        std::cerr << "The descriptor index is not a valid constant pool index" << std::endl;
        abort();
    }
}

void ares::VMCheck::visit_method_handle_info(ClassInfo &classInfo,
                                             ConstantInfo::MethodHandleInfo &info) {
    if (info.m_ReferenceKind < 1 || info.m_ReferenceKind > 9) {
        std::cerr << "The reference kind is not in range of 0 to 9." << std::endl;
        abort();
    }

    if (!classInfo.is_valid_index(info.m_ReferenceIndex)) {
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
            constantPoolInfo->m_Info.field_method_info.m_NameAndTypeIndex - 1];
        auto nameUTF8 = classInfo.m_ConstantPool[
            nameAndType->m_Info.name_and_type_info.m_NameIndex - 1];

        std::string name;
        name.assign((char *) nameUTF8->m_Info.utf8_info.m_Bytes, nameUTF8->m_Info.utf8_info.m_Length);

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
void ares::VMCheck::visit_dynamic_info(ClassInfo &classInfo,
                                       ConstantInfo::DynamicInfo &info) {
    if (!classInfo.is_valid_index(info.m_NameAndTypeIndex)) {
        std::cerr << "The name and type index is not a valid constant pool index." << std::endl;
        abort();
    }
}

void ares::VMCheck::visit_module_package_info(ClassInfo &classInfo,
                                              ConstantInfo::ModulePackageInfo &info) {
    if (!classInfo.is_valid_index(info.m_NameIndex)) {
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