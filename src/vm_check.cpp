#include "vm_check.h"

#include <iostream>

void ares::VMCheck::visit_class(ClassInfo &classInfo) {
    if (classInfo.magic_number != 0xCAFEBABE) {
        std::cerr << "The magic number doesn't match \"0xCAFEBABE\"." << std::endl;
        abort();
    }

    if (classInfo.class_version == ClassInfo::UNDEFINED) {
        std::cerr << "Couldn't set the class file version because it is an undefined value."
                  << std::endl;
        abort();
    }

    if (classInfo.class_version > ClassInfo::VERSION_12
        && (classInfo.minor_version != 0 && classInfo.minor_version != 65535)) {
        std::cerr << "All Java 12 class files need a minor version of 0 or 65535." << std::endl;
        abort();
    }

    for (auto &constantPoolInfo : classInfo.constant_pool) {
        if (constantPoolInfo == nullptr)
            continue;

        VMCheck::visit_classpool_info(classInfo, *constantPoolInfo);
    }

    if (classInfo.has_access_flag(ClassInfo::INTERFACE)) {
        if (!classInfo.has_access_flag(ClassInfo::ABSTRACT)
            || classInfo.has_access_flag(ClassInfo::FINAL)
            || classInfo.has_access_flag(ClassInfo::SUPER)
            || classInfo.has_access_flag(ClassInfo::ENUM)
            || classInfo.has_access_flag(ClassInfo::MODULE)) {
            std::cerr << "The class file has invalid interface access flags." << std::endl;
            abort();
        }
    } else if (classInfo.has_access_flag(ClassInfo::ANNOTATION)) {
        if (!classInfo.has_access_flag(ClassInfo::INTERFACE)
            || classInfo.has_access_flag(ClassInfo::ABSTRACT)
            || classInfo.has_access_flag(ClassInfo::FINAL)) {
            std::cerr << "The class file has invalid annotation access flags." << std::endl;
            abort();
        }
    } else if (classInfo.has_access_flag(ClassInfo::MODULE)) {
        if (classInfo.has_access_flag(ClassInfo::ABSTRACT)
            || classInfo.has_access_flag(ClassInfo::FINAL)) {
            std::cerr << "The class file has invalid module access flags." << std::endl;
            abort();
        }
    }

    if (!classInfo.is_valid_index(classInfo.this_class)) {
        std::cerr << "The \"this class\" index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto thisClass = classInfo.constant_pool[classInfo.this_class - 1];
    if (thisClass->tag != ConstantPoolInfo::CLASS) {
        std::cerr << "The \"this class\" is not a class constant pool info." << std::endl;
        abort();
    }

    if (classInfo.super_class != 0) {
        if (!classInfo.is_valid_index(classInfo.super_class)) {
            std::cerr << "The \"super class\" index is not a valid constant pool index."
                      << std::endl;
            abort();
        }

        auto superClass = classInfo.constant_pool[classInfo.super_class - 1];
        if (superClass->tag != ConstantPoolInfo::CLASS) {
            std::cerr << "The \"super class\" is not a class constant pool info." << std::endl;
            abort();
        }
    }

    for (auto &interface : classInfo.interfaces)
        VMCheck::visit_class_interface(classInfo, interface);

    for (auto &field : classInfo.fields)
        VMCheck::visit_class_field(classInfo, *field);

    for (auto &method : classInfo.methods)
        VMCheck::visit_class_method(classInfo, *method);

    for (auto &attribute : classInfo.attributes)
        VMCheck::visit_class_attribute(classInfo, *attribute);
}

void ares::VMCheck::visit_class_interface(ClassInfo &classInfo,
                                          uint16_t interface) {
    if (!classInfo.is_valid_index(interface)) {
        std::cerr << "The interface index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto constantPoolInfo = classInfo.constant_pool[interface - 1];
    if (constantPoolInfo->tag != ConstantPoolInfo::CLASS) {
        std::cerr << "The interface is not a class constant pool info." << std::endl;
        abort();
    }
}

void ares::VMCheck::visit_class_field(ClassInfo &classInfo,
                                      FieldInfo &fieldInfo) {
    if (fieldInfo.has_access_flag(FieldInfo::PUBLIC)) {
        if (fieldInfo.has_access_flag(FieldInfo::PRIVATE)
            || fieldInfo.has_access_flag(FieldInfo::PROTECTED)) {
            std::cerr << "The field has invalid public access flags." << std::endl;
            abort();
        }
    } else if (fieldInfo.has_access_flag(FieldInfo::PRIVATE)) {
        if (fieldInfo.has_access_flag(FieldInfo::PUBLIC)
            || fieldInfo.has_access_flag(FieldInfo::PROTECTED)) {
            std::cerr << "The field has invalid private access flags." << std::endl;
            abort();
        }
    } else if (fieldInfo.has_access_flag(FieldInfo::PROTECTED)) {
        if (fieldInfo.has_access_flag(FieldInfo::PUBLIC)
            || fieldInfo.has_access_flag(FieldInfo::PRIVATE)) {
            std::cerr << "The field has invalid protected access flags." << std::endl;
            abort();
        }
    }

    if (classInfo.has_access_flag(ClassInfo::INTERFACE)) {
        if (!fieldInfo.has_access_flag(FieldInfo::PUBLIC)
            || !fieldInfo.has_access_flag(FieldInfo::STATIC)
            || !fieldInfo.has_access_flag(FieldInfo::FINAL)) {
            std::cerr << "Fields of interfaces need to have public, static and final access "
                         "modifier set." << std::endl;
            abort();
        }
    }

    if (!classInfo.is_valid_index(fieldInfo.name_index)) {
        std::cerr << "The name index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto fieldName = classInfo.constant_pool[fieldInfo.name_index - 1];
    if (fieldName->tag != ConstantPoolInfo::UTF_8) {
        std::cerr << "The name is not a utf8 class pool info." << std::endl;
        abort();
    }

    if (!classInfo.is_valid_index(fieldInfo.descriptor_index)) {
        std::cerr << "The descriptor index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto fieldDescriptor = classInfo.constant_pool[fieldInfo.descriptor_index - 1];
    if (fieldDescriptor->tag != ConstantPoolInfo::UTF_8) {
        std::cerr << "The descriptor is not a utf8 class pool info." << std::endl;
        abort();
    }

    for (auto &attribute : fieldInfo.attributes)
        VMCheck::visit_field_attribute(classInfo, fieldInfo, *attribute);
}

// TODO: Do checks for the descriptor_index in
//  https://docs.oracle.com/javase/specs/jvms/se15/html/jvms-4.html#jvms-4.6
void ares::VMCheck::visit_class_method(ClassInfo &classInfo,
                                       MethodInfo &methodInfo) {
    if (methodInfo.has_access_flag(MethodInfo::PUBLIC)) {
        if (methodInfo.has_access_flag(MethodInfo::PRIVATE)
            || methodInfo.has_access_flag(MethodInfo::PROTECTED)) {
            std::cerr << "The method has invalid public access flags." << std::endl;
            abort();
        }
    } else if (methodInfo.has_access_flag(MethodInfo::PRIVATE)) {
        if (methodInfo.has_access_flag(MethodInfo::PUBLIC)
            || methodInfo.has_access_flag(MethodInfo::PROTECTED)) {
            std::cerr << "The method has invalid private access flags." << std::endl;
            abort();
        }
    } else if (methodInfo.has_access_flag(MethodInfo::PROTECTED)) {
        if (methodInfo.has_access_flag(MethodInfo::PUBLIC)
            || methodInfo.has_access_flag(MethodInfo::PRIVATE)) {
            std::cerr << "The method has invalid protected access flags." << std::endl;
            abort();
        }
    }

    if (classInfo.has_access_flag(ClassInfo::INTERFACE)) {
        if (methodInfo.has_access_flag(MethodInfo::PROTECTED)
            || methodInfo.has_access_flag(MethodInfo::FINAL)
            || methodInfo.has_access_flag(MethodInfo::SYNCHRONIZED)
            || methodInfo.has_access_flag(MethodInfo::NATIVE)) {
            std::cerr << "The access flags for an interface methods are invalid." << std::endl;
            abort();
        }

        if (classInfo.class_version < ClassInfo::VERSION_8) {
            if (!methodInfo.has_access_flag(MethodInfo::PUBLIC)
                || !methodInfo.has_access_flag(MethodInfo::ABSTRACT)) {
                std::cerr << "The access flags for an interface methods are invalid."
                          << std::endl;
                abort();
            }
        } else if (classInfo.class_version >= ClassInfo::VERSION_8) {
            if (methodInfo.has_access_flag(MethodInfo::PUBLIC)
                && methodInfo.has_access_flag(MethodInfo::PRIVATE)) {
                std::cerr << "The access flags for an interface methods are invalid."
                          << std::endl;
                abort();
            }
        }
    }

    if (methodInfo.has_access_flag(MethodInfo::ABSTRACT)) {
        if (methodInfo.has_access_flag(MethodInfo::PRIVATE)
            || methodInfo.has_access_flag(MethodInfo::STATIC)
            || methodInfo.has_access_flag(MethodInfo::FINAL)
            || methodInfo.has_access_flag(MethodInfo::SYNCHRONIZED)
            || methodInfo.has_access_flag(MethodInfo::NATIVE)
            || methodInfo.has_access_flag(MethodInfo::STRICT)) {
            std::cerr << "The access flags for an interface methods are invalid."
                      << std::endl;
            abort();
        }
    }

    if (!classInfo.is_valid_index(methodInfo.name_index)) {
        std::cerr << "The name index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto methodName = classInfo.constant_pool[methodInfo.name_index - 1];
    if (methodName->tag != ConstantPoolInfo::UTF_8) {
        std::cerr << "The name index is not a utf8 constant pool info." << std::endl;
        abort();
    }

    std::string name;
    name.assign((char *) methodName->info.utf8_info.bytes, methodName->info.utf8_info.length);
    if (name == "<init>") {
        if (methodInfo.has_access_flag(MethodInfo::ABSTRACT)
            || methodInfo.has_access_flag(MethodInfo::NATIVE)
            || methodInfo.has_access_flag(MethodInfo::BRIDGE)
            || methodInfo.has_access_flag(MethodInfo::SYNCHRONIZED)
            || methodInfo.has_access_flag(MethodInfo::FINAL)
            || methodInfo.has_access_flag(MethodInfo::STATIC)) {
            std::cerr << "The access flags for an interface methods are invalid."
                      << std::endl;
            abort();
        }
    }

    if (!classInfo.is_valid_index(methodInfo.descriptor_index)) {
        std::cerr << "The descriptor index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto methodDescriptor = classInfo.constant_pool[methodInfo.descriptor_index - 1];
    if (methodDescriptor->tag != ConstantPoolInfo::UTF_8) {
        std::cerr << "The name index is not a utf8 constant pool info." << std::endl;
        abort();
    }

    for (auto &attribute : methodInfo.attributes)
        VMCheck::visit_method_attribute(classInfo, methodInfo, *attribute);
}

void ares::VMCheck::visit_class_attribute(ClassInfo &classInfo,
                                          AttributeInfo &attributeInfo) {
    if (!classInfo.is_valid_index(attributeInfo.attribute_name_index)) {
        std::cerr << "The name index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto attributeName = classInfo.constant_pool[attributeInfo.attribute_name_index - 1];
    if (attributeName->tag != ConstantPoolInfo::UTF_8) {
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
    switch (constantPoolInfo.tag) {
        case ConstantPoolInfo::CLASS:VMCheck::visit_class_info(classInfo, constantPoolInfo.info.class_info);
            break;
        case ConstantPoolInfo::FIELD_REF:
        case ConstantPoolInfo::METHOD_REF:
        case ConstantPoolInfo::INTERFACE_METHOD_REF:
            VMCheck::visit_field_method_info(classInfo, constantPoolInfo.info.field_method_info);
            break;
        case ConstantPoolInfo::NAME_AND_TYPE:
            VMCheck::visit_name_and_type_info(classInfo, constantPoolInfo.info.name_and_type_info);
            break;
        case ConstantPoolInfo::STRING:VMCheck::visit_string_info(classInfo, constantPoolInfo.info.string_info);
            break;
        case ConstantPoolInfo::METHOD_TYPE:
            VMCheck::visit_method_type_info(classInfo, constantPoolInfo.info.method_type_info);
            break;
        case ConstantPoolInfo::METHOD_HANDLE:
            VMCheck::visit_method_handle_info(classInfo, constantPoolInfo.info.method_handle_info);
            break;
        case ConstantPoolInfo::DYNAMIC:
        case ConstantPoolInfo::INVOKE_DYNAMIC:
            VMCheck::visit_dynamic_info(classInfo, constantPoolInfo.info.dynamic_info);
            break;
        case ConstantPoolInfo::MODULE:
        case ConstantPoolInfo::PACKAGE:
            VMCheck::visit_module_package_info(classInfo, constantPoolInfo.info.module_package_info);
            break;
        case ConstantPoolInfo::UTF_8:
        case ConstantPoolInfo::INTEGER:
        case ConstantPoolInfo::FLOAT:
        case ConstantPoolInfo::LONG:
        case ConstantPoolInfo::DOUBLE:
            break;
        default:
            std::cerr << "Case for constant pool tag \"" << (int) constantPoolInfo.tag
                      << "\" not implemented yet." << std::endl;
            abort();
    }
}

void ares::VMCheck::visit_class_info(ares::ClassInfo &classInfo,
                                     ares::ConstantInfo::ClassInfo &info) {
    if (!classInfo.is_valid_index(info.name_index)) {
        std::cout << "The name index is not a valid constant pool index." << std::endl;
        abort();
    }
}

void ares::VMCheck::visit_field_method_info(ares::ClassInfo &classInfo,
                                            ares::ConstantInfo::FieldMethodInfo &info) {
    if (!classInfo.is_valid_index(info.class_index)) {
        std::cerr << "The class index is not a valid constant pool index." << std::endl;
        abort();
    }

    if (!classInfo.is_valid_index(info.name_and_type_index)) {
        std::cerr << "The name and type index is not a valid constant pool index." << std::endl;
        abort();
    }
}

void ares::VMCheck::visit_name_and_type_info(ClassInfo &classInfo,
                                             ConstantInfo::NameAndTypeInfo &info) {
    if (!classInfo.is_valid_index(info.name_index)) {
        std::cerr << "The name index is not a valid constant pool index." << std::endl;
        abort();
    }

    if (!classInfo.is_valid_index(info.descriptor_index)) {
        std::cerr << "The descriptor index is not a valid constant pool index." << std::endl;
        abort();
    }
}

void ares::VMCheck::visit_string_info(ClassInfo &classInfo,
                                      ConstantInfo::StringInfo &info) {
    if (!classInfo.is_valid_index(info.string_index)) {
        std::cerr << "The string index is not a valid constant pool index." << std::endl;
        abort();
    }
}

void ares::VMCheck::visit_method_type_info(ClassInfo &classInfo,
                                           ConstantInfo::MethodTypeInfo &info) {
    if (!classInfo.is_valid_index(info.descriptor_index)) {
        std::cerr << "The descriptor index is not a valid constant pool index" << std::endl;
        abort();
    }
}

void ares::VMCheck::visit_method_handle_info(ClassInfo &classInfo,
                                             ConstantInfo::MethodHandleInfo &info) {
    if (info.reference_kind < 1 || info.reference_kind > 9) {
        std::cerr << "The reference kind is not in range of 0 to 9." << std::endl;
        abort();
    }

    if (!classInfo.is_valid_index(info.reference_index)) {
        std::cerr << "The reference index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto constantPoolInfo = classInfo.constant_pool[info.reference_index - 1];
    auto referenceKind = info.reference_kind;

    if (referenceKind == ConstantInfo::MethodHandleKind::GetField
        || referenceKind == ConstantInfo::MethodHandleKind::GetStatic
        || referenceKind == ConstantInfo::MethodHandleKind::PutField
        || referenceKind == ConstantInfo::MethodHandleKind::PutStatic) {
        if (constantPoolInfo->tag != ConstantPoolInfo::FIELD_REF) {
            std::cerr << "The reference index of the method handle needs to be a field ref."
                      << std::endl;
            abort();
        }
    } else if (referenceKind == ConstantInfo::MethodHandleKind::InvokeVirtual
               || referenceKind == ConstantInfo::MethodHandleKind::NewInvokeSpecial) {
        if (constantPoolInfo->tag != ConstantPoolInfo::FIELD_REF) {
            std::cerr << "The reference index of the method handle needs to be a method ref."
                      << std::endl;
            abort();
        }
    } else if (referenceKind == ConstantInfo::MethodHandleKind::InvokeStatic
               || referenceKind == ConstantInfo::MethodHandleKind::InvokeSpecial) {
        if (classInfo.class_version < ClassInfo::VERSION_8
            && constantPoolInfo->tag != ConstantPoolInfo::METHOD_REF) {
            std::cerr << "The reference index of the method handle needs to be a method ref."
                      << std::endl;
            abort();
        } else if (constantPoolInfo->tag != ConstantPoolInfo::METHOD_REF
                   && constantPoolInfo->tag != ConstantPoolInfo::INTERFACE_METHOD_REF) {
            std::cerr << "The reference index of the method handle needs to be a method ref or "
                         "interface method ref." << std::endl;
            abort();
        }
    } else if (referenceKind == ConstantInfo::MethodHandleKind::InvokeInterface) {
        if (constantPoolInfo->tag != ConstantPoolInfo::INTERFACE_METHOD_REF) {
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
        auto nameAndType = classInfo.constant_pool[
            constantPoolInfo->info.field_method_info.name_and_type_index - 1];
        auto nameUTF8 = classInfo.constant_pool[
            nameAndType->info.name_and_type_info.name_index - 1];

        std::string name;
        name.assign((char *) nameUTF8->info.utf8_info.bytes, nameUTF8->info.utf8_info.length);

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
    if (!classInfo.is_valid_index(info.name_and_type_index)) {
        std::cerr << "The name and type index is not a valid constant pool index." << std::endl;
        abort();
    }
}

void ares::VMCheck::visit_module_package_info(ClassInfo &classInfo,
                                              ConstantInfo::ModulePackageInfo &info) {
    if (!classInfo.is_valid_index(info.name_index)) {
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