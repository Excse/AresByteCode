//
// Created by timo on 07.11.20.
//

#include "classreader.h"

#include <iostream>

#include "../structure/methodinfo.h"
#include "../structure/fieldinfo.h"
#include "../utils/utils.h"

ares::AresConfiguration::AresConfiguration() = default;

ares::AresConfiguration::~AresConfiguration() = default;

int ares::ClassReader::readClass(ClassFile &classFile, unsigned int &offset) {
    if (readMagicNumber(classFile, offset) == EXIT_FAILURE)
        return EXIT_FAILURE;

    if (readClassVersion(classFile, offset) == EXIT_FAILURE)
        return EXIT_FAILURE;

    if (readConstantPool(classFile, offset) == EXIT_FAILURE)
        return EXIT_FAILURE;

    if (readAccessFlags(classFile, offset) == EXIT_FAILURE)
        return EXIT_FAILURE;

    if (readThisClass(classFile, offset) == EXIT_FAILURE)
        return EXIT_FAILURE;

    if (readSuperClass(classFile, offset) == EXIT_FAILURE)
        return EXIT_FAILURE;

    if (readInterfaces(classFile, offset) == EXIT_FAILURE)
        return EXIT_FAILURE;

    if (readFields(classFile, offset) == EXIT_FAILURE)
        return EXIT_FAILURE;

    if (readMethods(classFile, offset) == EXIT_FAILURE)
        return EXIT_FAILURE;

    if (readAttributes(classFile, classFile.m_AttributesCount,
                       classFile.m_Attributes, offset) == EXIT_FAILURE)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int ares::ClassReader::readMagicNumber(ClassFile &classFile, unsigned int &offset) {
    if (ares::readU32(classFile.m_MagicNumber, classFile, offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the magic number of the class file." << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int ares::ClassReader::readClassVersion(ClassFile &classFile, unsigned int &offset) {
    uint16_t minorVersion = 0;
    if (ares::readU16(minorVersion, classFile, offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the minor version of the class file." << std::endl;
        return EXIT_FAILURE;
    }

    uint16_t majorVersion = 0;
    if (ares::readU16(majorVersion, classFile, offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the major version of the class file." << std::endl;
        return EXIT_FAILURE;
    }

    switch (majorVersion) {
        case ClassFile::VERSION_1_1:
            classFile.m_ClassVersion = ClassFile::VERSION_1_1;
            break;
        case ClassFile::VERSION_1_2:
            classFile.m_ClassVersion = ClassFile::VERSION_1_2;
            break;
        case ClassFile::VERSION_1_3:
            classFile.m_ClassVersion = ClassFile::VERSION_1_3;
            break;
        case ClassFile::VERSION_1_4:
            classFile.m_ClassVersion = ClassFile::VERSION_1_4;
            break;
        case ClassFile::VERSION_5:
            classFile.m_ClassVersion = ClassFile::VERSION_5;
            break;
        case ClassFile::VERSION_6:
            classFile.m_ClassVersion = ClassFile::VERSION_6;
            break;
        case ClassFile::VERSION_7:
            classFile.m_ClassVersion = ClassFile::VERSION_7;
            break;
        case ClassFile::VERSION_8:
            classFile.m_ClassVersion = ClassFile::VERSION_8;
            break;
        case ClassFile::VERSION_9:
            classFile.m_ClassVersion = ClassFile::VERSION_9;
            break;
        case ClassFile::VERSION_10:
            classFile.m_ClassVersion = ClassFile::VERSION_10;
            break;
        case ClassFile::VERSION_11:
            classFile.m_ClassVersion = ClassFile::VERSION_11;
            break;
        case ClassFile::VERSION_12:
            classFile.m_ClassVersion = ClassFile::VERSION_12;
            break;
        case ClassFile::VERSION_13:
            classFile.m_ClassVersion = ClassFile::VERSION_13;
            break;
        case ClassFile::VERSION_14:
            classFile.m_ClassVersion = ClassFile::VERSION_14;
            break;
        case ClassFile::VERSION_15:
            classFile.m_ClassVersion = ClassFile::VERSION_15;
            break;
        default:
            classFile.m_ClassVersion = ClassFile::UNDEFINED;
            break;
    }

    if (classFile.m_ClassVersion > ClassFile::VERSION_12
        && (minorVersion != 0 && minorVersion != 65535)) {
        std::cerr << "All Java 12 class files need a minor version of 0 or 65535." << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int ares::ClassReader::readConstantPool(ClassFile &classFile, unsigned int &offset) {
    if (ares::readU16(classFile.m_ConstantPoolCount, classFile, offset) == EXIT_FAILURE) {
        std::cout << "Couldn't read the constant pool count of this class file" << std::endl;
        return EXIT_FAILURE;
    }

    classFile.m_ConstantPool = std::vector<std::shared_ptr<ConstantPoolInfo>>(
            classFile.m_ConstantPoolCount - 1);
    for (auto index = 0; index < classFile.m_ConstantPoolCount - 1; index++) {
        auto info = std::make_shared<ConstantPoolInfo>();

        if (ares::readU8(info->m_Tag, classFile, offset) == EXIT_FAILURE) {
            std::cerr << "Couldn't read the tag of the constant pool info of this class file"
                      << std::endl;
            return EXIT_FAILURE;
        }

        switch (info->m_Tag) {
            case ConstantPoolInfo::CLASS:
                if (readClassInfo(classFile, info->m_Info.classInfo,
                                  offset) == EXIT_FAILURE)
                    return EXIT_FAILURE;
                break;
            case ConstantPoolInfo::METHOD_REF:
            case ConstantPoolInfo::FIELD_REF:
            case ConstantPoolInfo::INTERFACE_METHOD_REF:
                if (readFieldMethodInfo(classFile, info->m_Info.fieldMethodInfo,
                                        offset) == EXIT_FAILURE)
                    return EXIT_FAILURE;
                break;
            case ConstantPoolInfo::STRING:
                if (readStringInfo(classFile, info->m_Info.stringInfo,
                                   offset) == EXIT_FAILURE)
                    return EXIT_FAILURE;
                break;
            case ConstantPoolInfo::FLOAT:
            case ConstantPoolInfo::INTEGER:
                if (readFloatInteger(classFile, info->m_Info.integerFloatInfo,
                                     offset) == EXIT_FAILURE)
                    return EXIT_FAILURE;
                break;
            case ConstantPoolInfo::LONG:
            case ConstantPoolInfo::DOUBLE:
                if (readDoubleLong(classFile, info->m_Info.longDoubleInfo,
                                   offset) == EXIT_FAILURE)
                    return EXIT_FAILURE;
                break;
            case ConstantPoolInfo::NAME_AND_TYPE:
                if (readNameAndType(classFile, info->m_Info.nameAndTypeInfo,
                                    offset) == EXIT_FAILURE)
                    return EXIT_FAILURE;
                break;
            case ConstantPoolInfo::UTF_8:
                if (readUTF8Info(classFile, info->m_Info.utf8Info,
                                 offset) == EXIT_FAILURE)
                    return EXIT_FAILURE;
                break;
            case ConstantPoolInfo::METHOD_HANDLE:
                if (readMethodHandle(classFile, info->m_Info.methodHandleInfo,
                                     offset) == EXIT_FAILURE)
                    return EXIT_FAILURE;
                break;
            case ConstantPoolInfo::METHOD_TYPE:
                if (readMethodType(classFile, info->m_Info.methodTypeInfo,
                                   offset) == EXIT_FAILURE)
                    return EXIT_FAILURE;
                break;
            case ConstantPoolInfo::INVOKE_DYNAMIC:
            case ConstantPoolInfo::DYNAMIC:
                if (readDynamic(classFile, info->m_Info.dynamicInfo,
                                offset) == EXIT_FAILURE)
                    return EXIT_FAILURE;
                break;
            case ConstantPoolInfo::PACKAGE:
            case ConstantPoolInfo::MODULE:
                if (readModulePackage(classFile, info->m_Info.modulePackageInfo,
                                      offset) == EXIT_FAILURE)
                    return EXIT_FAILURE;
                break;
            default:
                std::cerr << "Case for constant pool tag \"" << (int) info->m_Tag
                          << "\" not implemented yet." << std::endl;
                return EXIT_FAILURE;
        }

        classFile.m_ConstantPool[index] = info;

        if (info->m_Tag == ConstantPoolInfo::DOUBLE
            || info->m_Tag == ConstantPoolInfo::LONG)
            index++;
    }

    return EXIT_SUCCESS;
}

int ares::ClassReader::readAccessFlags(ClassFile &classFile, unsigned int &offset) {
    if (ares::readU16(classFile.m_AccessFlags, classFile, offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the access flags of the class file." << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int ares::ClassReader::readThisClass(ClassFile &classFile, unsigned int &offset) {
    if (ares::readU16(classFile.m_ThisClass, classFile, offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the \"this class\" of this class file." << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int ares::ClassReader::readSuperClass(ClassFile &classFile, unsigned int &offset) {
    if (ares::readU16(classFile.m_SuperClass, classFile, offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the \"super class\" of the class file." << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int ares::ClassReader::readInterfaces(ClassFile &classFile, unsigned int &offset) {
    if (ares::readU16(classFile.m_InterfacesCount, classFile, offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the interface count of the class file." << std::endl;
        return EXIT_FAILURE;
    }

    classFile.m_Interfaces = std::vector<uint16_t>(classFile.m_InterfacesCount);
    for (auto &interface : classFile.m_Interfaces) {
        if (ares::readU16(interface, classFile, offset) == EXIT_FAILURE) {
            std::cerr << "Couldn't read the interface of this class." << std::endl;
            return EXIT_SUCCESS;
        }
    }

    return EXIT_SUCCESS;
}

int ares::ClassReader::readAttributes(ClassFile &classFile, uint16_t &attributeCount,
                                      std::vector<std::shared_ptr<AttributeInfo>> &attributes,
                                      unsigned int &offset) {
    if (ares::readU16(attributeCount, classFile, offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the attribute count." << std::endl;
        return EXIT_FAILURE;
    }

    attributes = std::vector<std::shared_ptr<AttributeInfo>>(attributeCount);
    for (auto &attribute : attributes) {
        auto attributeInfo = std::make_shared<AttributeInfo>();

        if (ares::readU16(attributeInfo->m_AttributeNameIndex, classFile, offset) == EXIT_FAILURE) {
            std::cerr << "Couldn't read the name index of the attribute." << std::endl;
            return EXIT_FAILURE;
        }

        if (ares::readU32(attributeInfo->m_AttributeLength, classFile, offset) == EXIT_FAILURE) {
            std::cerr << "Couldn't read the length of the attribute." << std::endl;
            return EXIT_FAILURE;
        }

        attributeInfo->m_Info = std::vector<uint8_t>(attributeInfo->m_AttributeLength);
        for (auto &info : attributeInfo->m_Info) {
            if (ares::readU8(info, classFile, offset) == EXIT_FAILURE) {
                std::cerr << "Couldn't read the info of the attribute." << std::endl;
                return EXIT_FAILURE;
            }
        }

        attribute = attributeInfo;
    }

    return EXIT_SUCCESS;
}

int ares::ClassReader::readMethods(ClassFile &classFile, unsigned int &offset) {
    if (ares::readU16(classFile.m_MethodCount, classFile, offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the method count of the class file." << std::endl;
        return EXIT_FAILURE;
    }

    classFile.m_Methods = std::vector<std::shared_ptr<MethodInfo>>(classFile.m_MethodCount);
    for (auto &method : classFile.m_Methods) {
        auto methodInfo = std::make_shared<MethodInfo>();

        if (ares::readU16(methodInfo->m_AccessFlags, classFile, offset) == EXIT_FAILURE) {
            std::cerr << "Couldn't read the access flags of the method." << std::endl;
            return EXIT_FAILURE;
        }

        if (ares::readU16(methodInfo->m_NameIndex, classFile, offset) == EXIT_FAILURE) {
            std::cerr << "Couldn't read the name index of the method." << std::endl;
            return EXIT_FAILURE;
        }

        if (ares::readU16(methodInfo->m_DescriptorIndex, classFile, offset) == EXIT_FAILURE) {
            std::cerr << "Couldn't read the descriptor index of the method." << std::endl;
            return EXIT_FAILURE;
        }

        if (readAttributes(classFile, methodInfo->m_AttributesCount,
                           methodInfo->m_Attributes, offset) == EXIT_FAILURE)
            return EXIT_FAILURE;

        method = methodInfo;
    }

    return EXIT_SUCCESS;
}

int ares::ClassReader::readFields(ClassFile &classFile, unsigned int &offset) {
    if (ares::readU16(classFile.m_FieldsCount, classFile, offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the field count of the class file." << std::endl;
        return EXIT_FAILURE;
    }

    classFile.m_Fields = std::vector<std::shared_ptr<FieldInfo>>(classFile.m_FieldsCount);
    for (auto &field : classFile.m_Fields) {
        auto fieldInfo = std::make_shared<FieldInfo>();

        if (ares::readU16(fieldInfo->m_AccessFlags, classFile, offset) == EXIT_FAILURE) {
            std::cerr << "Couldn't read the access flags of the field." << std::endl;
            return EXIT_FAILURE;
        }

        if (ares::readU16(fieldInfo->m_NameIndex, classFile, offset) == EXIT_FAILURE) {
            std::cerr << "Couldn't read the name index of the field." << std::endl;
            return EXIT_FAILURE;
        }

        if (ares::readU16(fieldInfo->m_DescriptorIndex, classFile, offset) == EXIT_FAILURE) {
            std::cerr << "Couldn't read the descriptor index of the field." << std::endl;
            return EXIT_FAILURE;
        }

        if (readAttributes(classFile, fieldInfo->m_AttributesCount,
                           fieldInfo->m_Attributes, offset) == EXIT_FAILURE)
            return EXIT_FAILURE;

        field = fieldInfo;
    }

    return EXIT_SUCCESS;
}


int ares::ClassReader::readClassInfo(ares::ClassFile &classFile,
                                     ConstantInfo::ClassInfo &info,
                                     unsigned int &offset) {
    if (ares::readU16(info.m_NameIndex, classFile, offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name index of the class pool info." << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int ares::ClassReader::readUTF8Info(ares::ClassFile &classFile,
                                    ConstantInfo::UTF8Info &info,
                                    unsigned int &offset) {
    if (ares::readU16(info.m_Length, classFile, offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the length of the class pool info." << std::endl;
        return EXIT_FAILURE;
    }

    info.m_Bytes = new uint8_t[info.m_Length];
    if (ares::readU8Array(info.m_Bytes, info.m_Length, classFile, offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the bytes of the class pool info." << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int ares::ClassReader::readFieldMethodInfo(ares::ClassFile &classFile,
                                           ConstantInfo::FieldMethodInfo &info,
                                           unsigned int &offset) {
    if (ares::readU16(info.m_ClassIndex, classFile, offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the class index of the class pool info." << std::endl;
        return EXIT_FAILURE;
    }

    if (ares::readU16(info.m_NameAndTypeIndex, classFile, offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name and type index of the class pool info." << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int ares::ClassReader::readNameAndType(ares::ClassFile &classFile,
                                       ConstantInfo::NameAndTypeInfo &info,
                                       unsigned int &offset) {
    if (ares::readU16(info.m_NameIndex, classFile, offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name index of the class pool info." << std::endl;
        return EXIT_FAILURE;
    }

    if (ares::readU16(info.m_DescriptorIndex, classFile, offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the descriptor index of the class pool info." << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int ares::ClassReader::readStringInfo(ares::ClassFile &classFile,
                                      ConstantInfo::StringInfo &info,
                                      unsigned int &offset) {
    if (ares::readU16(info.m_StringIndex, classFile, offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the string index of the class pool info." << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int ares::ClassReader::readDoubleLong(ares::ClassFile &classFile,
                                      ConstantInfo::DoubleLongInfo &info,
                                      unsigned int &offset) {
    if (ares::readU32(info.m_HighBytes, classFile, offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the high bytes of the class pool info." << std::endl;
        return EXIT_FAILURE;
    }

    if (ares::readU32(info.m_LowBytes, classFile, offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the low bytes of the class pool info." << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int ares::ClassReader::readFloatInteger(ares::ClassFile &classFile,
                                        ConstantInfo::FloatIntegerInfo &info,
                                        unsigned int &offset) {
    if (ares::readU32(info.m_Bytes, classFile, offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the bytes of the class pool info." << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int ares::ClassReader::readMethodType(ares::ClassFile &classFile,
                                      ConstantInfo::MethodTypeInfo &info,
                                      unsigned int &offset) {
    if (ares::readU16(info.m_DescriptorIndex, classFile, offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the descriptor index of the class pool info." << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int ares::ClassReader::readMethodHandle(ares::ClassFile &classFile,
                                        ConstantInfo::MethodHandleInfo &info,
                                        unsigned int &offset) {
    if (ares::readU8(info.m_ReferenceKind, classFile, offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the reference kind of the class pool info." << std::endl;
        return EXIT_FAILURE;
    }

    if (ares::readU16(info.m_ReferenceIndex, classFile, offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the reference index of the class pool info." << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int ares::ClassReader::readDynamic(ares::ClassFile &classFile,
                                   ConstantInfo::DynamicInfo &info,
                                   unsigned int &offset) {
    if (ares::readU16(info.m_BoostrapMethodAttrIndex, classFile, offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the bootstrap method attribute index of the class pool info."
                  << std::endl;
        return EXIT_FAILURE;
    }

    if (ares::readU16(info.m_NameAndTypeIndex, classFile, offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name and type index for the class pool info." << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int ares::ClassReader::readModulePackage(ares::ClassFile &classFile,
                                         ConstantInfo::ModulePackageInfo &info,
                                         unsigned int &offset) {
    if (ares::readU16(info.m_NameIndex, classFile, offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name index of the class pool info." << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}