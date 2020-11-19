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
#include "../visitor/visitor.h"

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

    class ClassReader : Visitor {

    private:
        unsigned int m_Offset{};

    public:
        explicit ClassReader(unsigned int offset = 0u);

        virtual ~ClassReader();

    public:
        void visitClass(ares::ClassFile &classFile) override;

    private:
        void readClassAttributes(ClassFile &classFile);

        void visitClassAttribute(ares::ClassFile &classFile,
                                 ares::AttributeInfo &attributeInfo) override;

        void readMagicNumber(ares::ClassFile &classFile);

        void readClassVersion(ares::ClassFile &classFile);

        void readConstantPool(ares::ClassFile &classFile);

        void visitClassCPInfo(ares::ClassFile &classFile,
                              ares::ConstantPoolInfo &info) override;

        void readClassInfo(ares::ClassFile &classFile,
                           ConstantInfo::ClassInfo &info);

        void readUTF8Info(ares::ClassFile &classFile,
                          ConstantInfo::UTF8Info &info);

        void readFieldMethodInfo(ares::ClassFile &classFile,
                                 ConstantInfo::FieldMethodInfo &info);

        void readNameAndType(ares::ClassFile &classFile,
                             ConstantInfo::NameAndTypeInfo &info);

        void readStringInfo(ares::ClassFile &classFile,
                            ConstantInfo::StringInfo &info);

        void readDoubleLong(ares::ClassFile &classFile,
                            ConstantInfo::DoubleLongInfo &info);

        void readFloatInteger(ares::ClassFile &classFile,
                              ConstantInfo::FloatIntegerInfo &info);

        void readMethodType(ares::ClassFile &classFile,
                            ConstantInfo::MethodTypeInfo &info);

        void readMethodHandle(ares::ClassFile &classFile,
                              ConstantInfo::MethodHandleInfo &info);

        void readDynamic(ares::ClassFile &classFile,
                         ConstantInfo::DynamicInfo &info);

        void readModulePackage(ares::ClassFile &classFile,
                               ConstantInfo::ModulePackageInfo &info);

        void readAccessFlags(ClassFile &classFile);

        void readThisClass(ClassFile &classFile);

        void readSuperClass(ClassFile &classFile);

        void readInterfaces(ClassFile &classFile);

        void visitClassInterface(ClassFile &classFile,
                                 uint16_t interface) override;

        void readFields(ClassFile &classFile);

        void visitClassField(ares::ClassFile &classFile,
                             ares::FieldInfo &fieldInfo) override;

        void readFieldAttributes(ClassFile &classFile, FieldInfo &fieldInfo);

        void visitFieldAttribute(ares::ClassFile &classFile,
                                 ares::FieldInfo &fieldInfo,
                                 ares::AttributeInfo &attributeInfo) override;

        void readMethods(ClassFile &classFile);

        void visitClassMethod(ares::ClassFile &classFile,
                              ares::MethodInfo &methodInfo) override;

        void readMethodAttributes(ClassFile &classFile, MethodInfo &methodInfo);

        void visitMethodAttribute(ares::ClassFile &classFile,
                                  ares::MethodInfo &methodInfo,
                                  ares::AttributeInfo &attributeInfo) override;

    public:
        [[nodiscard]]
        unsigned int getOffset() const;

    };

}