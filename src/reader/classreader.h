//
// Created by timo on 07.11.20.
//

#pragma once

#include <unordered_map>
#include <sys/stat.h>
#include <unistd.h>
#include <memory>
#include <vector>
#include <list>

#include "../structure/attributeinfo.h"
#include "../structure/constantinfo.h"
#include "../structure/classfile.h"

namespace ares {

    class AresConfiguration {

    public:
        std::unordered_map<std::string, std::shared_ptr<ClassFile>> m_Classes{};
        std::unordered_map<std::string, std::shared_ptr<ClassFile>> m_Others{};
        std::shared_ptr<ClassFile> m_Manifest{};

    public:
        AresConfiguration();

        virtual ~AresConfiguration();

    };

    class ClassReader {

    public:
        static int readClass(ClassFile &classFile, unsigned int &offset);

    private:
        static int readMagicNumber(ClassFile &classFile, unsigned int &offset);

        static int readClassVersion(ClassFile &classFile, unsigned int &offset);

        static int readConstantPool(ClassFile &classFile, unsigned int &offset);

        static int readAccessFlags(ClassFile &classFile, unsigned int &offset);

        static int readThisClass(ClassFile &classFile, unsigned int &offset);

        static int readSuperClass(ClassFile &classFile, unsigned int &offset);

        static int readInterfaces(ClassFile &classFile, unsigned int &offset);

        static int readFields(ClassFile &classFile, unsigned int &offset);

        static int readAttributes(ClassFile &classFile, uint16_t &attributeCount,
                                  std::vector<std::shared_ptr<AttributeInfo>> &attributes,
                                  unsigned int &offset);

        static int readMethods(ClassFile &classFile, unsigned int &offset);

        static int readClassInfo(ClassFile &classFile, ConstantInfo::ClassInfo &info,
                                 unsigned int &offset);

        static int readUTF8Info(ClassFile &classFile, ConstantInfo::UTF8Info &info,
                                unsigned int &offset);

        static int readFieldMethodInfo(ClassFile &classFile, ConstantInfo::FieldMethodInfo &info,
                                       unsigned int &offset);

        static int readNameAndType(ClassFile &classFile, ConstantInfo::NameAndTypeInfo &info,
                                   unsigned int &offset);

        static int readStringInfo(ClassFile &classFile, ConstantInfo::StringInfo &info,
                                  unsigned int &offset);

        static int readDoubleLong(ClassFile &classFile, ConstantInfo::DoubleLongInfo &info,
                                  unsigned int &offset);

        static int readFloatInteger(ClassFile &classFile, ConstantInfo::FloatIntegerInfo &info,
                                    unsigned int &offset);

        static int readMethodType(ClassFile &classFile, ConstantInfo::MethodTypeInfo &info,
                                  unsigned int &offset);

        static int readMethodHandle(ClassFile &classFile, ConstantInfo::MethodHandleInfo &info,
                                    unsigned int &offset);

        static int readDynamic(ClassFile &classFile, ConstantInfo::DynamicInfo &info,
                               unsigned int &offset);

        static int readModulePackage(ClassFile &classFile, ConstantInfo::ModulePackageInfo &info,
                                     unsigned int &offset);

    };

}