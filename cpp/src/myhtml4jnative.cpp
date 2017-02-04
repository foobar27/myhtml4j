#include "myhtml/api.h"

#include "myhtml4jnative.h"
#include "myjni.h"
#include "atoms.h"

#include <iostream>
#include <string>
#include <vector>

#define MAX_TAG_INDEX 251 // TODO magic value, where can we get it from?

struct IdJString {

    IdJString(int32_t id, jstring s)
        : m_id(id)
        , m_string(s)
    {}

    int32_t id() const {
        return m_id;
    }

    jstring string() const {
        return m_string;
    }

private:
    int32_t m_id;
    jstring m_string;
};

struct JTag : IdJString {
    JTag(int32_t id, jstring s)
        : IdJString(id, s)
    {}

};

struct JNamespace : IdJString {
    JNamespace(int32_t id, jstring s)
        : IdJString(id, s)
    {}

};

class JavaCallbackClass {

public:
    JavaCallbackClass(JNIEnv *env, JClass c)
        : m_internalError(c.getMethod<void>(env, "internalError", "()V"))
        , m_preOrderVisit(c.getMethod<void>(env, "preOrderVisit", "()V"))
        , m_createText(c.getMethod<void>(env, "createText", "(Ljava/lang/String;)V"))
        , m_createComment(c.getMethod<void>(env, "createComment", "(Ljava/lang/String;)V"))
        , m_createElement(c.getMethod<void>(env, "createElement", "(ILjava/lang/String;ILjava/lang/String;[I[Ljava/lang/String;)V"))
    {}

private:
    friend class JavaCallbackObject;
    JMethod<void> m_internalError;
    JMethod<void> m_preOrderVisit;
    JMethod<void> m_createText;
    JMethod<void> m_createComment;
    JMethod<void> m_createElement;
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
        m_class.m_createElement(m_env, m_object, ns.id(), ns.string(), tag.id(), tag.string(), ids, strings);
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

jlong JNICALL Java_com_github_foobar27_myhtml4j_Native_newContext(JNIEnv *env, jclass, jclass callbackClass) {
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

void JNICALL Java_com_github_foobar27_myhtml4j_Native_deleteContext(JNIEnv *env, jclass, jlong context) {
    delete (Context*) context;
}

struct JniNodeAttributes {
    jintArray ids;
    jobjectArray strings;
};

struct WalkContext {
    JNIEnv * env;
    JClass stringClass;
    JavaCallbackObject & cb;
    AttributeKeyCache attributeKeyCache;
    myhtml_tree_t* tree;
    std::vector<bool> seenTags;
};

JniNodeAttributes flatten_attributes(WalkContext & wc, myhtml_tree_node_t* root) {
    myhtml_tree_attr_t* attr = myhtml_node_attribute_first(root);
    if (!attr) {
        return {nullptr, nullptr};
    }
    std::vector<uint32_t> ids;
    std::vector<jstring> strings;
    while (attr) {
        auto aNs = myhtml_attribute_namespace(attr);
        auto aKey = wc.attributeKeyCache.get(myhtml_attribute_key(attr, nullptr));
        auto aValue = wc.env->NewStringUTF(myhtml_attribute_value(attr, nullptr));

        ids.push_back(aNs);
        ids.push_back(aKey.id);
        strings.push_back(aKey.name);
        strings.push_back(aValue);

        attr = myhtml_attribute_next(attr);
    }

    // Convert ids vector to JNI array
    auto idArray = wc.env->NewIntArray(ids.size());
    {
        if (!idArray) {
            throw 0; // TODO
        }
        wc.env->SetIntArrayRegion(idArray, 0, ids.size(), (jint*) ids.data());
    }

    // Convert strings vector to JNI array
    auto stringArray = wc.env->NewObjectArray(strings.size(), wc.stringClass.id(), nullptr);
    {
        if (!stringArray) {
            throw 0; // TODO
        }
        for (size_t i=0; i<strings.size(); ++i) {
            auto string = strings[i];
            if (string) {
                wc.env->SetObjectArrayElement(stringArray, i, string);
            } // else take pre-initialized value of null
        }
    }

    return {idArray, stringArray};
}

// TODO what happens on StackOverflow?
void transferSubTree(WalkContext & wc, myhtml_tree_node_t* root) {
    if (!root) {
        return;
    }

    auto tag = myhtml_node_tag_id(root);
    switch (tag) {
    case MyHTML_TAG__END_OF_FILE:
     case MyHTML_TAG__UNDEF:
         return;
     case MyHTML_TAG__TEXT:
         wc.cb.createText(myhtml_node_text(root, nullptr));
         return;
     case MyHTML_TAG__COMMENT:
         wc.cb.createComment(myhtml_node_text(root, nullptr));
         return;
     default:
        // continue
        break;
    }

    wc.cb.preOrderVisit();

    /* left hand depth-first recursion */
    myhtml_tree_node_t* child = myhtml_node_child(root);
    while (child != NULL) {
        transferSubTree(wc, child);
        child = myhtml_node_next(child);
    }

    // post-order
    int32_t signed_tag = tag;  // TODO is it possible to exploit this potential overflow?
    jstring tag_name = nullptr;
    if (tag > MAX_TAG_INDEX) {
        auto seenTagIndex = tag - MAX_TAG_INDEX - 1;
        if (seenTagIndex < wc.seenTags.size()) {
            // already sent, nothing to do. Java side will know how to translate tag id.
        } else {
            wc.seenTags.resize(seenTagIndex + 1, false);
            wc.seenTags[seenTagIndex] = true;
            tag_name = ToJniType<std::string>::toJni(wc.env, myhtml_tag_name_by_id(wc.tree, tag, nullptr));
        }
    }
    auto ns = myhtml_node_namespace(root);
    auto attributes = flatten_attributes(wc, root);
    wc.cb.createElement({ns, nullptr}, {signed_tag, tag_name}, attributes.ids, attributes.strings);

}

void JNICALL Java_com_github_foobar27_myhtml4j_Native_parseUTF8(JNIEnv *env, jclass, jlong c, jstring i, jobject callback) {
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

    WalkContext wContext {env, context->stringClass, cb, {env}, tree, {}};
    transferSubTree(wContext, myhtml_tree_get_node_html(tree));

    // release resources
    myhtml_tree_destroy(tree);
}
