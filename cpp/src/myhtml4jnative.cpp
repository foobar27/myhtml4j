#include "myhtml/api.h"

#include "myhtml4jnative.h"
#include "myjni.h"

#include "unordered_set"

#include <iostream>
#include <string>
#include <vector>

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
        : m_internalError(c.getMethod<void>(env, "internalErorr", "()V"))
        , m_preOrderVisit(c.getMethod<void>(env, "preOrderVisit", "()V"))
        , m_createText(c.getMethod<void>(env, "createText", "(Ljava/lang/String;)V"))
        , m_createComment(c.getMethod<void>(env, "createComment", "(Ljava/lang/String;)V"))
        , m_createNormalElement(c.getMethod<void>(env, "createNormalElement", "(ILjava/lang/String;ILjava/lang/String;[I[Ljava/lang/String;)V"))
    {}

private:
    friend class JavaCallbackObject;
    JMethod<void> m_internalError;
    JMethod<void> m_preOrderVisit;
    JMethod<void> m_createText;
    JMethod<void> m_createComment;
    JMethod<void> m_createNormalElement;
};

class JavaCallbackObject {
public:
    JavaCallbackObject(JNIEnv *env, JavaCallbackClass& clazz, JObject object)
        : m_env(env)
        , m_class(clazz)
        , m_object(object)
    {}

    void internalError() {
        m_class.m_internalError(m_env, m_object);
    }

    void preOrderVisit() {
        m_class.m_preOrderVisit(m_env, m_object);
    }

    void createText(const std::string & text) {
        m_class.m_createText(m_env, m_object, ToJniType<std::string>::toJni(m_env, text));
    }

    void createComment(const std::string & text) {
        m_class.m_createComment(m_env, m_object, ToJniType<std::string>::toJni(m_env, text));
    }

    void createElement(const JNamespace & ns, const JTag & tag, jintArray ids, jobjectArray strings) {
        m_class.m_createNormalElement(m_env, m_object, ns.id(), ns.string(), tag.id(), tag.string(), ids, strings);
    }

private:
    JNIEnv* m_env;
    JavaCallbackClass& m_class;
    JObject m_object;
};

struct Context {
    JavaCallbackClass m_callbackClass;
    myhtml_t* m_myhtml;
    JClass stringClass;

    Context(JNIEnv *env, jclass callbackClass, myhtml_t* myhtml)
        : m_callbackClass(env, callbackClass)
        , m_myhtml(myhtml)
        , stringClass(JClass::load(env, "java/lang/String").globalRef(env))
    {}

};

jlong newContext(JNIEnv *env, jclass callbackClass) {
    auto myhtml = myhtml_create();
    if (!myhtml) {
        std::cerr << "myhtml_create failed" << std::endl;
        return 0;
    }
    auto res = myhtml_init(myhtml, MyHTML_OPTIONS_DEFAULT, 1, 0);
    if (MYHTML_FAILED(res)) {
        std::cerr << "myhtml_init failed with " << res << std::endl;
        return 0;
    }

    return (jlong) new Context(env, callbackClass, myhtml);
}

void deleteContext(JNIEnv *env, jlong context) {
    delete (Context*) context;
}

struct JniNodeAttributes {
    jintArray ids;
    jobjectArray strings;
};

JniNodeAttributes flatten_attributes(JNIEnv* env, jclass stringClass, myhtml_tree_node_t* root) {
    myhtml_tree_attr_t* attr = myhtml_node_attribute_first(root);
    if (!attr) {
        return {nullptr, nullptr};
    }
    std::vector<uint32_t> ids;
    std::vector<jstring> strings;
    while (attr) {
        auto aNs = myhtml_attribute_namespace(attr);
        auto aKey = env->NewStringUTF(myhtml_attribute_key(attr, nullptr)); // TODO cache
        auto aValue = env->NewStringUTF(myhtml_attribute_value(attr, nullptr));

        ids.push_back(aNs);
        strings.push_back(aKey);
        strings.push_back(aValue);

        attr = myhtml_attribute_next(attr);
    }

    // Convert ids vector to JNI array
    auto idArray = env->NewIntArray(ids.size());
    {
        if (!idArray) {
            throw 0; // TODO
        }
        env->SetIntArrayRegion(idArray, 0, ids.size(), (jint*) ids.data());
    }

    // Convert strings vector to JNI array
    auto stringArray = env->NewObjectArray(strings.size(), stringClass, nullptr);
    {
        if (!stringArray) {
            throw 0; // TODO
        }
        for (int i=0; i<strings.size(); ++i) {
            env->SetObjectArrayElement(stringArray, i, strings[i]);
        }
    }

    return {idArray, stringArray};
}

struct WalkContext {
    JNIEnv * env;
    JClass stringClass;
    JavaCallbackObject & cb;
};

// TODO what happens on StackOverflow?
void transferSubTree(WalkContext & context, myhtml_tree_node_t* root) {
    if (!root) {
        return;
    }

    auto tag = myhtml_node_tag_id(root);

    switch (tag) {
    case MyHTML_TAG__END_OF_FILE:
     case MyHTML_TAG__UNDEF:
         return;
     case MyHTML_TAG__TEXT:
         context.cb.createText(myhtml_node_text(root, nullptr));
         return;
     case MyHTML_TAG__COMMENT:
         context.cb.createComment(myhtml_node_text(root, nullptr));
         return;
     default:
        // continue
        break;
    }

    context.cb.preOrderVisit();

    /* left hand depth-first recoursion */
    myhtml_tree_node_t* child = myhtml_node_child(root);
    while (child != NULL) {
        transferSubTree(context, child);
        child = myhtml_node_next(child);
    }

    // post-order
    auto ns = myhtml_node_namespace(root);
    auto attributes = flatten_attributes(context.env, context.stringClass.id(), root);
    context.cb.createElement({ns, nullptr}, {tag, nullptr}, attributes.ids, attributes.strings);

}


void parseUTF8(JNIEnv *env, jlong c, jstring i, jobject callback) {
    Context* context = (Context*) c;
    JavaCallbackObject cb(env, context->m_callbackClass, JObject(callback));
    const char *input = env->GetStringUTFChars(i, nullptr);
    size_t inputLength = (size_t)env->GetStringLength(i);
    // init tree
    myhtml_tree_t* tree = myhtml_tree_create();
    if (!tree) {
        std::cerr << "myhtml_tree_create failed" << std::endl;
        cb.internalError();
        return;
    }

    auto res = myhtml_tree_init(tree, context->m_myhtml);
    if (MYHTML_FAILED(res)) {
        std::cerr << "myhtml_tree_init failed with " << res << std::endl;
        myhtml_tree_destroy(tree);
        cb.internalError();
        return;
    }

    // parse html
    myhtml_parse(tree, MyHTML_ENCODING_UTF_8, input, inputLength);

    WalkContext wContext {env, context->stringClass, cb};
    transferSubTree(wContext, myhtml_tree_get_node_html(tree));

    // release resources
    myhtml_tree_destroy(tree);
    myhtml_destroy(context->m_myhtml);
}
