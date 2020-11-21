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
#include "../structure/classinfo.h"
#include "../visitor/visitor.h"

namespace ares {

    class ClassReader : Visitor {

    private:
        unsigned int m_Offset{};

    public:
        explicit ClassReader(unsigned int offset = 0u);

        virtual ~ClassReader();

    public:
        void visitClass(ares::ClassInfo &classFile) override;

    private:
        void readClassAttributes(ClassInfo &classFile);

        void visitClassAttribute(ares::ClassInfo &classFile,
                                 ares::AttributeInfo &attributeInfo) override;

        void readMagicNumber(ares::ClassInfo &classFile);

        void readClassVersion(ares::ClassInfo &classFile);

        void readConstantPool(ares::ClassInfo &classFile);

        void visitClassCPInfo(ares::ClassInfo &classFile,
                              ares::ConstantPoolInfo &info) override;

        void readClassInfo(ares::ClassInfo &classFile,
                           ConstantInfo::ClassInfo &info);

        void readUTF8Info(ares::ClassInfo &classFile,
                          ConstantInfo::UTF8Info &info);

        void readFieldMethodInfo(ares::ClassInfo &classFile,
                                 ConstantInfo::FieldMethodInfo &info);

        void readNameAndType(ares::ClassInfo &classFile,
                             ConstantInfo::NameAndTypeInfo &info);

        void readStringInfo(ares::ClassInfo &classFile,
                            ConstantInfo::StringInfo &info);

        void readDoubleLong(ares::ClassInfo &classFile,
                            ConstantInfo::DoubleLongInfo &info);

        void readFloatInteger(ares::ClassInfo &classFile,
                              ConstantInfo::FloatIntegerInfo &info);

        void readMethodType(ares::ClassInfo &classFile,
                            ConstantInfo::MethodTypeInfo &info);

        void readMethodHandle(ares::ClassInfo &classFile,
                              ConstantInfo::MethodHandleInfo &info);

        void readDynamic(ares::ClassInfo &classFile,
                         ConstantInfo::DynamicInfo &info);

        void readModulePackage(ares::ClassInfo &classFile,
                               ConstantInfo::ModulePackageInfo &info);

        void readAccessFlags(ClassInfo &classFile);

        void readThisClass(ClassInfo &classFile);

        void readSuperClass(ClassInfo &classFile);

        void readInterfaces(ClassInfo &classFile);

        void visitClassInterface(ClassInfo &classFile,
                                 uint16_t interface) override;

        void readFields(ClassInfo &classFile);

        void visitClassField(ares::ClassInfo &classFile,
                             ares::FieldInfo &fieldInfo) override;

        void readFieldAttributes(ClassInfo &classFile, FieldInfo &fieldInfo);

        void visitFieldAttribute(ares::ClassInfo &classFile,
                                 ares::FieldInfo &fieldInfo,
                                 ares::AttributeInfo &attributeInfo) override;

        void readMethods(ClassInfo &classFile);

        void visitClassMethod(ares::ClassInfo &classFile,
                              ares::MethodInfo &methodInfo) override;

        void readMethodAttributes(ClassInfo &classFile, MethodInfo &methodInfo);

        void visitMethodAttribute(ares::ClassInfo &classFile,
                                  ares::MethodInfo &methodInfo,
                                  ares::AttributeInfo &attributeInfo) override;

    public:
        [[nodiscard]]
        unsigned int getOffset() const;

    };

}