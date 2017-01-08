#include "myjni.h"

JClass::JClass(jclass id)
    : m_id(id)
{}

jclass JClass::id() const {
    return m_id;
}
