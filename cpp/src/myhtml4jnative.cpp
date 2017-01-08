#include "myhtml/api.h"

#include "myhtml4jnative.h"
#include "myjni.h"

#include "unordered_set"

// TODO use internal myhtml.tag_id etc

struct IdJString {

    IdJString(uint32_t id, jstring s)
        : m_id(id)
        , m_string(s)
    {}

    uint32_t id() const {
        return m_id;
    }

    jstring string() const {
        return m_string;
    }

private:
    uint32_t m_id;
    jstring m_string;
};

struct JTag : IdJString {
    JTag(uint32_t id, jstring s)
        : IdJString(id, s)
    {}

};

struct JNamespace : IdJString {
    JNamespace(uint32_t id, jstring s)
        : IdJString(id, s)
    {}

};

class JavaCallbackClass {

public:
    JavaCallbackClass(JNIEnv *env, JClass c)
        : m_createText(c.getMethod<void>(env, "createText", "(Ljava.lang.String;)V"))
        , m_createComment(c.getMethod<void>(env, "createComment", "(Ljava.lang.String;)V"))
        , m_createNormalElement(c.getMethod<void>(env, "createNormalElement", "(ILjava.lang.String;)V"))
    {}

private:
    friend class JavaCallbackObject;
    JMethod<void> m_createText;
    JMethod<void> m_createComment;
    JMethod<void> m_createNormalElement;
};

class JavaCallbackObject {
public:
    JavaCallbackObject(JNIEnv *env, JavaCallbackClass clazz, JObject object)
        : m_class(clazz)
        , m_object(object)
    {}

    void createText(JNIEnv *env, const std::string & text) {
        m_class.m_createText(env, m_object, ToJniType<std::string>::toJni(env, text));
    }

    void createComment(JNIEnv *env, const std::string & text) {
        m_class.m_createComment(env, m_object, ToJniType<std::string>::toJni(env, text));
    }

    void createNormalElement(JNIEnv *env, const JNamespace & ns, const JTag & tag) {
        m_class.m_createNormalElement(env, m_object, ns.id(), ns.string(), tag.id(), tag.string());
    }

private:
    JavaCallbackClass m_class;
    JObject m_object;
};

struct Context {
    JavaCallbackClass m_callbackClass;

    Context(JNIEnv *env, jclass callbackClass)
        : m_callbackClass(env, callbackClass)
    {}

};

jlong newContext(JNIEnv *env, jclass callbackClass) {
    return (jlong) new Context(env, callbackClass);
}

void deleteContext(JNIEnv *env, jlong context) {
    delete (Context*) context;
}
