//
// Created by timo on 15.11.20.
//

// TODO: Make the error messages better

#include "vmcheck.h"

#include <iostream>

void ares::VMCheck::visitClass(ares::ClassInfo &classFile) {
    if (classFile.m_MagicNumber != 0xCAFEBABE) {
        std::cerr << "The magic number doesn't match \"0xCAFEBABE\"." << std::endl;
        abort();
    }

    if (classFile.m_ClassVersion == ClassInfo::UNDEFINED) {
        std::cerr << "Couldn't set the class file version because it is an undefined value."
                  << std::endl;
        abort();
    }

    if (classFile.m_ClassVersion > ClassInfo::VERSION_12
        && (classFile.m_MinorVersion != 0 && classFile.m_MinorVersion != 65535)) {
        std::cerr << "All Java 12 class files need a minor version of 0 or 65535." << std::endl;
        abort();
    }

    for (auto &constantPoolInfo : classFile.m_ConstantPool) {
        if (constantPoolInfo == nullptr)
            continue;

        VMCheck::visitClassCPInfo(classFile, *constantPoolInfo);
    }

    if (classFile.hasAccessFlag(ClassInfo::INTERFACE)) {
        if (!classFile.hasAccessFlag(ClassInfo::ABSTRACT)
            || classFile.hasAccessFlag(ClassInfo::FINAL)
            || classFile.hasAccessFlag(ClassInfo::SUPER)
            || classFile.hasAccessFlag(ClassInfo::ENUM)
            || classFile.hasAccessFlag(ClassInfo::MODULE)) {
            std::cerr << "The class file has invalid interface access flags." << std::endl;
            abort();
        }
    } else if (classFile.hasAccessFlag(ClassInfo::ANNOTATION)) {
        if (!classFile.hasAccessFlag(ClassInfo::INTERFACE)
            || classFile.hasAccessFlag(ClassInfo::ABSTRACT)
            || classFile.hasAccessFlag(ClassInfo::FINAL)) {
            std::cerr << "The class file has invalid annotation access flags." << std::endl;
            abort();
        }
    } else if (classFile.hasAccessFlag(ClassInfo::MODULE)) {
        if (classFile.hasAccessFlag(ClassInfo::ABSTRACT)
            || classFile.hasAccessFlag(ClassInfo::FINAL)) {
            std::cerr << "The class file has invalid module access flags." << std::endl;
            abort();
        }
    }

    if (!classFile.isIndexValid(classFile.m_ThisClass)) {
        std::cerr << "The \"this class\" index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto thisClass = classFile.m_ConstantPool[classFile.m_ThisClass - 1];
    if (thisClass->m_Tag != ConstantPoolInfo::CLASS) {
        std::cerr << "The \"this class\" is not a class constant pool info." << std::endl;
        abort();
    }

    if (classFile.m_SuperClass != 0) {
        if (!classFile.isIndexValid(classFile.m_SuperClass)) {
            std::cerr << "The \"super class\" index is not a valid constant pool index."
                      << std::endl;
            abort();
        }

        auto superClass = classFile.m_ConstantPool[classFile.m_SuperClass - 1];
        if (superClass->m_Tag != ConstantPoolInfo::CLASS) {
            std::cerr << "The \"super class\" is not a class constant pool info." << std::endl;
            abort();
        }
    }

    for (auto &interface : classFile.m_Interfaces)
        VMCheck::visitClassInterface(classFile, interface);

    for (auto &field : classFile.m_Fields)
        VMCheck::visitClassField(classFile, *field);

    for (auto &method : classFile.m_Methods)
        VMCheck::visitClassMethod(classFile, *method);

    for (auto &attribute : classFile.m_Attributes)
        VMCheck::visitClassAttribute(classFile, *attribute);
}

void ares::VMCheck::visitClassInterface(ares::ClassInfo &classFile,
                                        uint16_t interface) {
    if (!classFile.isIndexValid(interface)) {
        std::cerr << "The interface index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto constantPoolInfo = classFile.m_ConstantPool[interface - 1];
    if (constantPoolInfo->m_Tag != ConstantPoolInfo::CLASS) {
        std::cerr << "The interface is not a class constant pool info." << std::endl;
        abort();
    }
}

void ares::VMCheck::visitClassField(ares::ClassInfo &classFile,
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

    if (classFile.hasAccessFlag(ClassInfo::INTERFACE)) {
        if (!fieldInfo.hasAccessFlag(FieldInfo::PUBLIC)
            || !fieldInfo.hasAccessFlag(FieldInfo::STATIC)
            || !fieldInfo.hasAccessFlag(FieldInfo::FINAL)) {
            std::cerr << "Fields of interfaces need to have public, static and final access "
                         "modifier set." << std::endl;
            abort();
        }
    }

    if (!classFile.isIndexValid(fieldInfo.m_NameIndex)) {
        std::cerr << "The name index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto fieldName = classFile.m_ConstantPool[fieldInfo.m_NameIndex - 1];
    if (fieldName->m_Tag != ConstantPoolInfo::UTF_8) {
        std::cerr << "The name is not a utf8 class pool info." << std::endl;
        abort();
    }

    if (!classFile.isIndexValid(fieldInfo.m_DescriptorIndex)) {
        std::cerr << "The descriptor index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto fieldDescriptor = classFile.m_ConstantPool[fieldInfo.m_DescriptorIndex - 1];
    if (fieldDescriptor->m_Tag != ConstantPoolInfo::UTF_8) {
        std::cerr << "The descriptor is not a utf8 class pool info." << std::endl;
        abort();
    }

    for (auto &attribute : fieldInfo.m_Attributes)
        VMCheck::visitFieldAttribute(classFile, fieldInfo, *attribute);
}

// TODO: Do checks for the descriptor_index in
//  https://docs.oracle.com/javase/specs/jvms/se15/html/jvms-4.html#jvms-4.6
void ares::VMCheck::visitClassMethod(ares::ClassInfo &classFile,
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

    if (classFile.hasAccessFlag(ClassInfo::INTERFACE)) {
        if (methodInfo.hasAccessFlag(MethodInfo::PROTECTED)
            || methodInfo.hasAccessFlag(MethodInfo::FINAL)
            || methodInfo.hasAccessFlag(MethodInfo::SYNCHRONIZED)
            || methodInfo.hasAccessFlag(MethodInfo::NATIVE)) {
            std::cerr << "The access flags for an interface methods are invalid." << std::endl;
            abort();
        }

        if (classFile.m_ClassVersion < ClassInfo::VERSION_8) {
            if (!methodInfo.hasAccessFlag(MethodInfo::PUBLIC)
                || !methodInfo.hasAccessFlag(MethodInfo::ABSTRACT)) {
                std::cerr << "The access flags for an interface methods are invalid."
                          << std::endl;
                abort();
            }
        } else if (classFile.m_ClassVersion >= ClassInfo::VERSION_8) {
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

    if (!classFile.isIndexValid(methodInfo.m_NameIndex)) {
        std::cerr << "The name index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto methodName = classFile.m_ConstantPool[methodInfo.m_NameIndex - 1];
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

    if (!classFile.isIndexValid(methodInfo.m_DescriptorIndex)) {
        std::cerr << "The descriptor index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto methodDescriptor = classFile.m_ConstantPool[methodInfo.m_DescriptorIndex - 1];
    if (methodDescriptor->m_Tag != ConstantPoolInfo::UTF_8) {
        std::cerr << "The name index is not a utf8 constant pool info." << std::endl;
        abort();
    }

    for (auto &attribute : methodInfo.m_Attributes)
        VMCheck::visitMethodAttribute(classFile, methodInfo, *attribute);
}

void ares::VMCheck::visitClassAttribute(ares::ClassInfo &classFile,
                                        ares::AttributeInfo &attributeInfo) {
    if (!classFile.isIndexValid(attributeInfo.m_AttributeNameIndex)) {
        std::cerr << "The name index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto attributeName = classFile.m_ConstantPool[attributeInfo.m_AttributeNameIndex - 1];
    if (attributeName->m_Tag != ConstantPoolInfo::UTF_8) {
        std::cerr << "The name index is not a utf8 class pool info." << std::endl;
        abort();
    }
}

void ares::VMCheck::visitFieldAttribute(ares::ClassInfo &classFile,
                                        ares::FieldInfo &fieldInfo,
                                        ares::AttributeInfo &attributeInfo) {
    VMCheck::visitClassAttribute(classFile, attributeInfo);
}

void ares::VMCheck::visitMethodAttribute(ares::ClassInfo &classFile,
                                         ares::MethodInfo &methodInfo,
                                         ares::AttributeInfo &attributeInfo) {
    VMCheck::visitClassAttribute(classFile, attributeInfo);
}

void ares::VMCheck::visitClassCPInfo(ares::ClassInfo &classFile,
                                     ares::ConstantPoolInfo &constantPoolInfo) {
    switch (constantPoolInfo.m_Tag) {
        case ConstantPoolInfo::CLASS:
            VMCheck::visitClassInfo(classFile, constantPoolInfo.m_Info.classInfo);
            break;
        case ConstantPoolInfo::FIELD_REF:
        case ConstantPoolInfo::METHOD_REF:
        case ConstantPoolInfo::INTERFACE_METHOD_REF:
            VMCheck::visitFieldMethodInfo(classFile, constantPoolInfo.m_Info.fieldMethodInfo);
            break;
        case ConstantPoolInfo::NAME_AND_TYPE:
            VMCheck::visitNameAndTypeInfo(classFile, constantPoolInfo.m_Info.nameAndTypeInfo);
            break;
        case ConstantPoolInfo::STRING:
            VMCheck::visitStringInfo(classFile, constantPoolInfo.m_Info.stringInfo);
            break;
        case ConstantPoolInfo::METHOD_TYPE:
            VMCheck::visitMethodTypeInfo(classFile, constantPoolInfo.m_Info.methodTypeInfo);
            break;
        case ConstantPoolInfo::METHOD_HANDLE:
            VMCheck::visitMethodHandleInfo(classFile, constantPoolInfo.m_Info.methodHandleInfo);
            break;
        case ConstantPoolInfo::DYNAMIC:
        case ConstantPoolInfo::INVOKE_DYNAMIC:
            VMCheck::visitDynamicInfo(classFile, constantPoolInfo.m_Info.dynamicInfo);
            break;
        case ConstantPoolInfo::MODULE:
        case ConstantPoolInfo::PACKAGE:
            VMCheck::visitModulePackageInfo(classFile, constantPoolInfo.m_Info.modulePackageInfo);
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

void ares::VMCheck::visitClassInfo(ares::ClassInfo &classFile,
                                   ares::ConstantInfo::ClassInfo &info) {
    if (!classFile.isIndexValid(info.m_NameIndex)) {
        std::cout << "The name index is not a valid constant pool index." << std::endl;
        abort();
    }
}

void ares::VMCheck::visitFieldMethodInfo(ares::ClassInfo &classFile,
                                         ares::ConstantInfo::FieldMethodInfo &info) {
    if (!classFile.isIndexValid(info.m_ClassIndex)) {
        std::cerr << "The class index is not a valid constant pool index." << std::endl;
        abort();
    }

    if (!classFile.isIndexValid(info.m_NameAndTypeIndex)) {
        std::cerr << "The name and type index is not a valid constant pool index." << std::endl;
        abort();
    }
}

void ares::VMCheck::visitNameAndTypeInfo(ares::ClassInfo &classFile,
                                         ares::ConstantInfo::NameAndTypeInfo &info) {
    if (!classFile.isIndexValid(info.m_NameIndex)) {
        std::cerr << "The name index is not a valid constant pool index." << std::endl;
        abort();
    }

    if (!classFile.isIndexValid(info.m_DescriptorIndex)) {
        std::cerr << "The descriptor index is not a valid constant pool index." << std::endl;
        abort();
    }
}

void ares::VMCheck::visitStringInfo(ares::ClassInfo &classFile,
                                    ares::ConstantInfo::StringInfo &info) {
    if (!classFile.isIndexValid(info.m_StringIndex)) {
        std::cerr << "The string index is not a valid constant pool index." << std::endl;
        abort();
    }
}

void ares::VMCheck::visitMethodTypeInfo(ares::ClassInfo &classFile,
                                        ares::ConstantInfo::MethodTypeInfo &info) {
    if (!classFile.isIndexValid(info.m_DescriptorIndex)) {
        std::cerr << "The descriptor index is not a valid constant pool index" << std::endl;
        abort();
    }
}

void ares::VMCheck::visitMethodHandleInfo(ares::ClassInfo &classFile,
                                          ares::ConstantInfo::MethodHandleInfo &info) {
    if (info.m_ReferenceKind < 1 || info.m_ReferenceKind > 9) {
        std::cerr << "The reference kind is not in range of 0 to 9." << std::endl;
        abort();
    }

    if (!classFile.isIndexValid(info.m_ReferenceIndex)) {
        std::cerr << "The reference index is not a valid constant pool index." << std::endl;
        abort();
    }

    auto constantPoolInfo = classFile.m_ConstantPool[info.m_ReferenceIndex - 1];
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
        if (classFile.m_ClassVersion < ClassInfo::VERSION_8
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
        auto nameAndType = classFile.m_ConstantPool[
                constantPoolInfo->m_Info.fieldMethodInfo.m_NameAndTypeIndex - 1];
        auto nameUTF8 = classFile.m_ConstantPool[
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
void ares::VMCheck::visitDynamicInfo(ares::ClassInfo &classFile,
                                     ares::ConstantInfo::DynamicInfo &info) {
    if (!classFile.isIndexValid(info.m_NameAndTypeIndex)) {
        std::cerr << "The name and type index is not a valid constant pool index." << std::endl;
        abort();
    }
}

void ares::VMCheck::visitModulePackageInfo(ares::ClassInfo &classFile,
                                           ares::ConstantInfo::ModulePackageInfo &info) {
    if (!classFile.isIndexValid(info.m_NameIndex)) {
        std::cerr << "The name index is not a valid constant pool index." << std::endl;
        abort();
    }
}
