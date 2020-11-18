//
// Created by timo on 16.11.20.
//

#include "classwriter.h"

void ares::ClassWriter::visitClass(const ares::ClassFile *classFile) const {

}

void ares::ClassWriter::visitClassCPInfo(const ares::ClassFile *classFile,
                                         const ares::ConstantPoolInfo *constantPoolInfo) const {

}

void
ares::ClassWriter::visitClassInterface(const ares::ClassFile *classFile, uint16_t interface) const {

}

void ares::ClassWriter::visitClassField(const ares::ClassFile *classFile,
                                        const ares::FieldInfo *fieldInfo) const {

}

void ares::ClassWriter::visitClassMethod(const ares::ClassFile *classFile,
                                         const ares::MethodInfo *methodInfo) const {

}

void ares::ClassWriter::visitClassAttribute(const ares::ClassFile *classFile,
                                            const ares::AttributeInfo *attributeInfo) const {

}

void ares::ClassWriter::visitFieldAttribute(const ares::ClassFile *classFile,
                                            const ares::FieldInfo *fieldInfo,
                                            const ares::AttributeInfo *attributeInfo) const {

}

void ares::ClassWriter::visitMethodAttribute(const ares::ClassFile *classFile,
                                             const ares::MethodInfo *methodInfo,
                                             const ares::AttributeInfo *attributeInfo) const {

}
