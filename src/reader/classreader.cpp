//
// Created by timo on 07.11.20.
//

#include "classreader.h"

#include <iostream>

#include "../utils/utils.h"

ares::AresConfiguration::AresConfiguration() = default;

ares::AresConfiguration::~AresConfiguration() = default;


ares::ClassReader::ClassReader(unsigned int offset)
        : m_Offset(offset) {}

ares::ClassReader::~ClassReader() = default;

void ares::ClassReader::visitClass(ares::ClassFile &classFile) {
    readMagicNumber(classFile);
    readClassVersion(classFile);
    readConstantPool(classFile);
    readAccessFlags(classFile);
    readThisClass(classFile);
    readSuperClass(classFile);
    readInterfaces(classFile);
    readFields(classFile);
    readMethods(classFile);
    readClassAttributes(classFile);
}

void ares::ClassReader::readClassAttributes(ares::ClassFile &classFile) {
    if (ares::readU16(classFile.m_AttributesCount, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the attribute count." << std::endl;
        abort();
    }

    classFile.m_Attributes = std::vector<std::shared_ptr<AttributeInfo>>(
            classFile.m_AttributesCount);
    for (auto &attribute : classFile.m_Attributes) {
        auto attributeInfo = std::make_shared<AttributeInfo>();
        ClassReader::visitClassAttribute(classFile, *attributeInfo);

        attribute = attributeInfo;
    }
}

void ares::ClassReader::visitClassAttribute(ares::ClassFile &classFile,
                                            ares::AttributeInfo &attributeInfo) {
    if (ares::readU16(attributeInfo.m_AttributeNameIndex, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name index of the attribute." << std::endl;
        abort();
    }

    if (ares::readU32(attributeInfo.m_AttributeLength, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the length of the attribute." << std::endl;
        abort();
    }

    attributeInfo.m_Info = std::vector<uint8_t>(attributeInfo.m_AttributeLength);
    for (auto &info : attributeInfo.m_Info) {
        if (ares::readU8(info, classFile, m_Offset) == EXIT_FAILURE) {
            std::cerr << "Couldn't read the info of the attribute." << std::endl;
            abort();
        }
    }
}

void ares::ClassReader::readMagicNumber(ClassFile &classFile) {
    if (ares::readU32(classFile.m_MagicNumber, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the magic number of the class file." << std::endl;
        abort();
    }
}

void ares::ClassReader::readClassVersion(ClassFile &classFile) {
    if (ares::readU16(classFile.m_MinorVersion, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the minor version of the class file." << std::endl;
        abort();
    }

    if (ares::readU16(classFile.m_MajorVersion, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the major version of the class file." << std::endl;
        abort();
    }

    classFile.m_ClassVersion = ClassFile::ClassVersion(classFile.m_MajorVersion);
    if (classFile.m_MajorVersion >= ClassFile::VERSION_1_1
        && classFile.m_MajorVersion <= ClassFile::VERSION_15) {
        classFile.m_ClassVersion = ClassFile::ClassVersion(classFile.m_MajorVersion);
    } else {
        classFile.m_ClassVersion = ClassFile::UNDEFINED;
    }
}

void ares::ClassReader::readConstantPool(ClassFile &classFile) {
    if (ares::readU16(classFile.m_ConstantPoolCount, classFile, m_Offset) == EXIT_FAILURE) {
        std::cout << "Couldn't read the constant pool count of this class file" << std::endl;
        abort();
    }

    classFile.m_ConstantPool = std::vector<std::shared_ptr<ConstantPoolInfo>>(
            classFile.m_ConstantPoolCount - 1);
    for (auto index = 0; index < classFile.m_ConstantPoolCount - 1; index++) {
        auto info = std::make_shared<ConstantPoolInfo>();
        ClassReader::visitClassCPInfo(classFile, *info);

        classFile.m_ConstantPool[index] = info;

        if (info->m_Tag == ConstantPoolInfo::DOUBLE || info->m_Tag == ConstantPoolInfo::LONG)
            index++;
    }
}

void ares::ClassReader::visitClassCPInfo(ares::ClassFile &classFile,
                                         ares::ConstantPoolInfo &info) {
    uint8_t infoTag = 0;
    if (ares::readU8(infoTag, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the tag of the constant pool info of this class file"
                  << std::endl;
        abort();
    }

    if (infoTag >= ConstantPoolInfo::UTF_8 && infoTag <= ConstantPoolInfo::PACKAGE
        && infoTag != 13 && infoTag != 14)
        info.m_Tag = ConstantPoolInfo::ConstantTag(infoTag);
    else
        info.m_Tag = ConstantPoolInfo::UNDEFINED;

    switch (info.m_Tag) {
        case ConstantPoolInfo::CLASS:
            readClassInfo(classFile, info.m_Info.classInfo);
            break;
        case ConstantPoolInfo::METHOD_REF:
        case ConstantPoolInfo::FIELD_REF:
        case ConstantPoolInfo::INTERFACE_METHOD_REF:
            readFieldMethodInfo(classFile, info.m_Info.fieldMethodInfo);
            break;
        case ConstantPoolInfo::STRING:
            readStringInfo(classFile, info.m_Info.stringInfo);
            break;
        case ConstantPoolInfo::FLOAT:
        case ConstantPoolInfo::INTEGER:
            readFloatInteger(classFile, info.m_Info.integerFloatInfo);
            break;
        case ConstantPoolInfo::LONG:
        case ConstantPoolInfo::DOUBLE:
            readDoubleLong(classFile, info.m_Info.longDoubleInfo);
            break;
        case ConstantPoolInfo::NAME_AND_TYPE:
            readNameAndType(classFile, info.m_Info.nameAndTypeInfo);
            break;
        case ConstantPoolInfo::UTF_8:
            readUTF8Info(classFile, info.m_Info.utf8Info);
            break;
        case ConstantPoolInfo::METHOD_HANDLE:
            readMethodHandle(classFile, info.m_Info.methodHandleInfo);
            break;
        case ConstantPoolInfo::METHOD_TYPE:
            readMethodType(classFile, info.m_Info.methodTypeInfo);
            break;
        case ConstantPoolInfo::INVOKE_DYNAMIC:
        case ConstantPoolInfo::DYNAMIC:
            readDynamic(classFile, info.m_Info.dynamicInfo);
            break;
        case ConstantPoolInfo::PACKAGE:
        case ConstantPoolInfo::MODULE:
            readModulePackage(classFile, info.m_Info.modulePackageInfo);
            break;
        default:
            break;
    }
}

void ares::ClassReader::readClassInfo(ares::ClassFile &classFile,
                                      ConstantInfo::ClassInfo &info) {
    if (ares::readU16(info.m_NameIndex, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name index of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::readUTF8Info(ares::ClassFile &classFile,
                                     ConstantInfo::UTF8Info &info) {
    if (ares::readU16(info.m_Length, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the length of the class pool info." << std::endl;
        abort();
    }

    info.m_Bytes = new uint8_t[info.m_Length];
    if (ares::readU8Array(info.m_Bytes, info.m_Length, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the bytes of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::readFieldMethodInfo(ares::ClassFile &classFile,
                                            ConstantInfo::FieldMethodInfo &info) {
    if (ares::readU16(info.m_ClassIndex, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the class index of the class pool info." << std::endl;
        abort();
    }

    if (ares::readU16(info.m_NameAndTypeIndex, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name and type index of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::readNameAndType(ares::ClassFile &classFile,
                                        ConstantInfo::NameAndTypeInfo &info) {
    if (ares::readU16(info.m_NameIndex, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name index of the class pool info." << std::endl;
        abort();
    }

    if (ares::readU16(info.m_DescriptorIndex, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the descriptor index of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::readStringInfo(ares::ClassFile &classFile,
                                       ConstantInfo::StringInfo &info) {
    if (ares::readU16(info.m_StringIndex, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the string index of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::readDoubleLong(ares::ClassFile &classFile,
                                       ConstantInfo::DoubleLongInfo &info) {
    if (ares::readU32(info.m_HighBytes, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the high bytes of the class pool info." << std::endl;
        abort();
    }

    if (ares::readU32(info.m_LowBytes, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the low bytes of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::readFloatInteger(ares::ClassFile &classFile,
                                         ConstantInfo::FloatIntegerInfo &info) {
    if (ares::readU32(info.m_Bytes, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the bytes of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::readMethodType(ares::ClassFile &classFile,
                                       ConstantInfo::MethodTypeInfo &info) {
    if (ares::readU16(info.m_DescriptorIndex, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the descriptor index of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::readMethodHandle(ares::ClassFile &classFile,
                                         ConstantInfo::MethodHandleInfo &info) {
    if (ares::readU8(info.m_ReferenceKind, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the reference kind of the class pool info." << std::endl;
        abort();
    }

    if (ares::readU16(info.m_ReferenceIndex, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the reference index of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::readDynamic(ares::ClassFile &classFile,
                                    ConstantInfo::DynamicInfo &info) {
    if (ares::readU16(info.m_BoostrapMethodAttrIndex, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the bootstrap method attribute index of the class pool info."
                  << std::endl;
        abort();
    }

    if (ares::readU16(info.m_NameAndTypeIndex, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name and type index for the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::readModulePackage(ares::ClassFile &classFile,
                                          ConstantInfo::ModulePackageInfo &info) {
    if (ares::readU16(info.m_NameIndex, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name index of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::readAccessFlags(ClassFile &classFile) {
    if (ares::readU16(classFile.m_AccessFlags, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the access flags of the class file." << std::endl;
        abort();
    }
}

void ares::ClassReader::readThisClass(ClassFile &classFile) {
    if (ares::readU16(classFile.m_ThisClass, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the \"this class\" of this class file." << std::endl;
        abort();
    }
}

void ares::ClassReader::readSuperClass(ClassFile &classFile) {
    if (ares::readU16(classFile.m_SuperClass, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the \"super class\" of the class file." << std::endl;
        abort();
    }
}

void ares::ClassReader::readInterfaces(ClassFile &classFile) {
    if (ares::readU16(classFile.m_InterfacesCount, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the interface count of the class file." << std::endl;
        abort();
    }

    classFile.m_Interfaces = std::vector<uint16_t>(classFile.m_InterfacesCount);
    for (auto &interface : classFile.m_Interfaces) {
        if (ares::readU16(interface, classFile, m_Offset) == EXIT_FAILURE) {
            std::cerr << "Couldn't read the interface of this class." << std::endl;
            abort();
        }
    }
}

void ares::ClassReader::visitClassInterface(ares::ClassFile &classFile,
                                            uint16_t interface) {}

void ares::ClassReader::readFields(ClassFile &classFile) {
    if (ares::readU16(classFile.m_FieldsCount, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the field count of the class file." << std::endl;
        abort();
    }

    classFile.m_Fields = std::vector<std::shared_ptr<FieldInfo>>(classFile.m_FieldsCount);
    for (auto &field : classFile.m_Fields) {
        auto fieldInfo = std::make_shared<FieldInfo>();
        ClassReader::visitClassField(classFile, *fieldInfo);

        field = fieldInfo;
    }
}

void ares::ClassReader::visitClassField(ares::ClassFile &classFile,
                                        ares::FieldInfo &fieldInfo) {
    if (ares::readU16(fieldInfo.m_AccessFlags, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the access flags of the field." << std::endl;
        abort();
    }

    if (ares::readU16(fieldInfo.m_NameIndex, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name index of the field." << std::endl;
        abort();
    }

    if (ares::readU16(fieldInfo.m_DescriptorIndex, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the descriptor index of the field." << std::endl;
        abort();
    }

    readFieldAttributes(classFile, fieldInfo);
}

void ares::ClassReader::readFieldAttributes(ares::ClassFile &classFile,
                                            ares::FieldInfo &fieldInfo) {
    if (ares::readU16(fieldInfo.m_AttributesCount, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the attribute count." << std::endl;
        abort();
    }

    fieldInfo.m_Attributes = std::vector<std::shared_ptr<AttributeInfo>>(
            fieldInfo.m_AttributesCount);
    for (auto &attribute : fieldInfo.m_Attributes) {
        auto attributeInfo = std::make_shared<AttributeInfo>();
        ClassReader::visitFieldAttribute(classFile, fieldInfo, *attributeInfo);

        attribute = attributeInfo;
    }
}

void ares::ClassReader::visitFieldAttribute(ares::ClassFile &classFile,
                                            ares::FieldInfo &fieldInfo,
                                            ares::AttributeInfo &attributeInfo) {
    ClassReader::visitClassAttribute(classFile, attributeInfo);
}

void ares::ClassReader::readMethods(ares::ClassFile &classFile) {
    if (ares::readU16(classFile.m_MethodCount, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the method count of the class file." << std::endl;
        abort();
    }

    classFile.m_Methods = std::vector<std::shared_ptr<MethodInfo>>(classFile.m_MethodCount);
    for (auto &method : classFile.m_Methods) {
        auto methodInfo = std::make_shared<MethodInfo>();
        ClassReader::visitClassMethod(classFile, *methodInfo);

        method = methodInfo;
    }
}

void ares::ClassReader::visitClassMethod(ares::ClassFile &classFile,
                                         ares::MethodInfo &methodInfo) {
    if (ares::readU16(methodInfo.m_AccessFlags, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the access flags of the method." << std::endl;
        abort();
    }

    if (ares::readU16(methodInfo.m_NameIndex, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name index of the method." << std::endl;
        abort();
    }

    if (ares::readU16(methodInfo.m_DescriptorIndex, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the descriptor index of the method." << std::endl;
        abort();
    }

    readMethodAttributes(classFile, methodInfo);
}

void ares::ClassReader::readMethodAttributes(ares::ClassFile &classFile,
                                             ares::MethodInfo &methodInfo) {
    if (ares::readU16(methodInfo.m_AttributesCount, classFile, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the attribute count." << std::endl;
        abort();
    }

    methodInfo.m_Attributes = std::vector<std::shared_ptr<AttributeInfo>>(
            methodInfo.m_AttributesCount);
    for (auto &attribute : methodInfo.m_Attributes) {
        auto attributeInfo = std::make_shared<AttributeInfo>();
        ClassReader::visitMethodAttribute(classFile, methodInfo, *attributeInfo);

        attribute = attributeInfo;
    }
}

void ares::ClassReader::visitMethodAttribute(ares::ClassFile &classFile,
                                             ares::MethodInfo &methodInfo,
                                             ares::AttributeInfo &attributeInfo) {
    ClassReader::visitClassAttribute(classFile, attributeInfo);
}

unsigned int ares::ClassReader::getOffset() const {
    return m_Offset;
}