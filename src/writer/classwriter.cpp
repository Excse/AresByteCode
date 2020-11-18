//
// Created by timo on 16.11.20.
//

#include "classwriter.h"

#include "../utils/utils.h"

#include "../structure/constantinfo.h"

ares::ClassWriter::ClassWriter(uint8_t *byteCode, unsigned int size, unsigned int offset)
        : m_ByteCode(byteCode), m_Size(size),
          m_Offset(offset) {}

ares::ClassWriter::~ClassWriter() = default;

void ares::ClassWriter::visitClass(ares::ClassFile &classFile) {
    ares::writeU32(classFile.m_MagicNumber, m_ByteCode, m_Size, m_Offset);
    ares::writeU16(classFile.m_MinorVersion, m_ByteCode, m_Size, m_Offset);
    ares::writeU16(classFile.m_MajorVersion, m_ByteCode, m_Size, m_Offset);
    ares::writeU16(classFile.m_ConstantPoolCount, m_ByteCode, m_Size, m_Offset);
}

void ares::ClassWriter::visitClassCPInfo(ares::ClassFile &classFile,
                                         ares::ConstantPoolInfo &constantPoolInfo) {

}

void ares::ClassWriter::visitClassInterface(ares::ClassFile &classFile,
                                            uint16_t interface) {

}

void ares::ClassWriter::visitClassField(ares::ClassFile &classFile,
                                        ares::FieldInfo &fieldInfo) {

}

void ares::ClassWriter::visitClassMethod(ares::ClassFile &classFile,
                                         ares::MethodInfo &methodInfo) {

}

void ares::ClassWriter::visitClassAttribute(ares::ClassFile &classFile,
                                            ares::AttributeInfo &attributeInfo) {

}

void ares::ClassWriter::visitFieldAttribute(ares::ClassFile &classFile,
                                            ares::FieldInfo &fieldInfo,
                                            ares::AttributeInfo &attributeInfo) {

}

void ares::ClassWriter::visitMethodAttribute(ares::ClassFile &classFile,
                                             ares::MethodInfo &methodInfo,
                                             ares::AttributeInfo &attributeInfo) {

}