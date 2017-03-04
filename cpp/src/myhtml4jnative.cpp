/*
 * This file is part of the myhtml4j distribution.
 *   (https://github.com/foobar27/myhtml4j)
 * Copyright (c) 2017 Sebastien Wagener.
 *
 * myhtml4j is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, version 3.
 *
 * myhtml4j is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include <algorithm>
#include <cstring>
#include <sstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include "myhtml/api.h"

#include "myhtml4jnative.h"
#include "myjni.h"
#include "atoms.h"
#include "traverser.h"
#include "adapter.h"

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
        , m_setDoctype(c.getMethod<void>(env, "setDoctype", "([Ljava/lang/String;)V"))
        , m_preOrderVisit(c.getMethod<void>(env, "preOrderVisit", "()V"))
        , m_createText(c.getMethod<void>(env, "createText", "(Ljava/lang/String;)V"))
        , m_createComment(c.getMethod<void>(env, "createComment", "(Ljava/lang/String;)V"))
        , m_createElement(c.getMethod<void>(env, "createElement", "(ILjava/lang/String;ILjava/lang/String;[I[Ljava/lang/String;)V"))
    {}

private:
    friend class JavaCallbackObject;
    JMethod<void> m_internalError;
    JMethod<void> m_setDoctype;
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

    void setDoctype(jobjectArray attributes) {
        m_class.m_setDoctype(m_env, m_object, attributes);
    }

    void preOrderVisit() {
        m_class.m_preOrderVisit(m_env, m_object);
    }

    void createText(const std::string & text) {
        m_class.m_createText(m_env, m_object, stringToJni(m_env, text));
    }

    void createComment(const std::string & text) {
        m_class.m_createComment(m_env, m_object, stringToJni(m_env, text));
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

jobjectArray stringArrayToObjectArray(WalkContext & wc, const std::vector<jstring> & strings) {
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
    return stringArray;
}

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
        auto aValue = charArrayToJni(wc.env, myhtml_attribute_value(attr, nullptr));

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
    auto stringArray = stringArrayToObjectArray(wc, strings);

    return {idArray, stringArray};
}

void transferDoctype(WalkContext & wc, myhtml_tree_node_t* root) {
    auto child = myhtml_node_child(root);
    if (!child) {
        return;
    }
    auto tag = myhtml_node_tag_id(child);
    if (tag == MyHTML_TAG__DOCTYPE) {
        myhtml_tree_attr_t* attr = myhtml_node_attribute_first(child);
        std::vector<jstring> strings;
        while (attr) {
            auto key = myhtml_attribute_key(attr, nullptr);
            auto value = myhtml_attribute_value(attr, nullptr);
            if (key) {
                strings.push_back(wc.env->NewStringUTF(key));
            }
            if (value) {
                strings.push_back(wc.env->NewStringUTF(value));
            }
            attr = myhtml_attribute_next(attr);
        }
        wc.cb.setDoctype(stringArrayToObjectArray(wc, strings));
    }
}

struct TransferTreeVisitor {
    using Node = myhtml_tree_node_t*;

    WalkContext & wc;

    bool pre(Node node) {
        auto tag = myhtml_node_tag_id(node);
        switch (tag) {
         case MyHTML_TAG__END_OF_FILE:
         case MyHTML_TAG__UNDEF:
             return false;
         case MyHTML_TAG__TEXT:
             {
                auto txt = myhtml_node_text(node, nullptr);
                if (txt) {
                    wc.cb.createText(txt);
                }
             }
             return false;
         case MyHTML_TAG__COMMENT:
             {
                auto txt = myhtml_node_text(node, nullptr);
                if (txt) {
                    wc.cb.createComment(txt);
                }
             }
             return false;
         default:
            // continue
            break;
        }

        wc.cb.preOrderVisit();
        return true;
    }

    void post(Node node) {
        auto tag = myhtml_node_tag_id(node);
        int32_t signed_tag = tag;  // TODO is it possible to exploit this potential overflow?
        jstring tag_name = nullptr;
        if (tag > MAX_TAG_INDEX) {
            auto seenTagIndex = tag - MAX_TAG_INDEX - 1;
            if (seenTagIndex < wc.seenTags.size()) {
              if (wc.seenTags[seenTagIndex])  {
                // already sent, nothing to do. Java side will know how to translate tag id.
              } else {
                wc.seenTags[seenTagIndex] = true;
                tag_name = charArrayToJni(wc.env, myhtml_tag_name_by_id(wc.tree, tag, nullptr));
              }
            } else {
                wc.seenTags.resize(seenTagIndex + 1, false);
                wc.seenTags[seenTagIndex] = true;
                tag_name = charArrayToJni(wc.env, myhtml_tag_name_by_id(wc.tree, tag, nullptr));
            }
        }
        auto ns = myhtml_node_namespace(node);
        auto attributes = flatten_attributes(wc, node);
        wc.cb.createElement({ns, nullptr}, {signed_tag, tag_name}, attributes.ids, attributes.strings);
    }

};

void transferSubTree(WalkContext & wc, myhtml_tree_node_t* root) {
    TransferTreeVisitor visitor { wc };
    traverse<MyHtmlAdapter, TransferTreeVisitor>(visitor, root);
}

void add_whitespace(myhtml_tag_id_t tag, std::stringstream & ss) {
    switch (tag) {
    // treat the following as whitespace: http://www.javased.com/?source_dir=ihtika/Sources/Bundles/JerichoBundle/src/main/java/net/htmlparser/jericho/TextExtractor.java
    case MyHTML_TAG_BR:
    case MyHTML_TAG_ADDRESS:
    case MyHTML_TAG_ANNOTATION_XML:
    case MyHTML_TAG_AREA:
    case MyHTML_TAG_ARTICLE:
    case MyHTML_TAG_ASIDE:
    case MyHTML_TAG_AUDIO:
    case MyHTML_TAG_BASE:
    case MyHTML_TAG_BGSOUND:
    case MyHTML_TAG_BLINK:
    case MyHTML_TAG_BLOCKQUOTE:
    case MyHTML_TAG_BODY:
    case MyHTML_TAG_CANVAS:
    case MyHTML_TAG_CAPTION:
    case MyHTML_TAG_CENTER:
    case MyHTML_TAG_COL:
    case MyHTML_TAG_COLGROUP:
    case MyHTML_TAG_COMMAND:
    case MyHTML_TAG_COMMENT:
    case MyHTML_TAG_DATALIST:
    case MyHTML_TAG_DD:
    case MyHTML_TAG_DETAILS:
    case MyHTML_TAG_DIALOG:
    case MyHTML_TAG_DIR:
    case MyHTML_TAG_DIV:
    case MyHTML_TAG_DL:
    case MyHTML_TAG_DT:
    case MyHTML_TAG_EMBED:
    case MyHTML_TAG_FIELDSET:
    case MyHTML_TAG_FIGCAPTION:
    case MyHTML_TAG_FIGURE:
    case MyHTML_TAG_FOOTER:
    case MyHTML_TAG_FORM:
    case MyHTML_TAG_FRAME:
    case MyHTML_TAG_FRAMESET:
    case MyHTML_TAG_H1:
    case MyHTML_TAG_H2:
    case MyHTML_TAG_H3:
    case MyHTML_TAG_H4:
    case MyHTML_TAG_H5:
    case MyHTML_TAG_H6:
    case MyHTML_TAG_HEAD:
    case MyHTML_TAG_HEADER:
    case MyHTML_TAG_HGROUP:
    case MyHTML_TAG_HR:
    case MyHTML_TAG_HTML:
    case MyHTML_TAG_IMAGE:
    case MyHTML_TAG_ISINDEX:
    case MyHTML_TAG_LEGEND:
    case MyHTML_TAG_LI:
    case MyHTML_TAG_LINK:
    case MyHTML_TAG_LISTING:
    case MyHTML_TAG_MAIN:
    case MyHTML_TAG_MARQUEE:
    case MyHTML_TAG_MENU:
    case MyHTML_TAG_MENUITEM:
    case MyHTML_TAG_META:
    case MyHTML_TAG_MTEXT:
    case MyHTML_TAG_NAV:
    case MyHTML_TAG_NOBR:
    case MyHTML_TAG_NOEMBED:
    case MyHTML_TAG_NOFRAMES:
    case MyHTML_TAG_NOSCRIPT:
    case MyHTML_TAG_OL:
    case MyHTML_TAG_OPTGROUP:
    case MyHTML_TAG_OPTION:
    case MyHTML_TAG_P:
    case MyHTML_TAG_PARAM:
    case MyHTML_TAG_PLAINTEXT:
    case MyHTML_TAG_PRE:
    case MyHTML_TAG_RB:
    case MyHTML_TAG_RTC:
    case MyHTML_TAG_SECTION:
    case MyHTML_TAG_SOURCE:
    case MyHTML_TAG_STYLE:
    case MyHTML_TAG_SUMMARY:
    case MyHTML_TAG_SVG:
    case MyHTML_TAG_TABLE:
    case MyHTML_TAG_TBODY:
    case MyHTML_TAG_TD:
    case MyHTML_TAG_TEMPLATE:
    case MyHTML_TAG_TFOOT:
    case MyHTML_TAG_TH:
    case MyHTML_TAG_THEAD:
    case MyHTML_TAG_TITLE:
    case MyHTML_TAG_TR:
    case MyHTML_TAG_TRACK:
    case MyHTML_TAG_UL:
    case MyHTML_TAG_VIDEO:
    case MyHTML_TAG_XMP:
    case MyHTML_TAG_ALTGLYPH:
    case MyHTML_TAG_ALTGLYPHDEF:
    case MyHTML_TAG_ALTGLYPHITEM:
    case MyHTML_TAG_ANIMATE:
    case MyHTML_TAG_ANIMATECOLOR:
    case MyHTML_TAG_ANIMATEMOTION:
    case MyHTML_TAG_ANIMATETRANSFORM:
    case MyHTML_TAG_CIRCLE:
    case MyHTML_TAG_CLIPPATH:
    case MyHTML_TAG_COLOR_PROFILE:
    case MyHTML_TAG_CURSOR:
    case MyHTML_TAG_DEFS:
    case MyHTML_TAG_DESC:
    case MyHTML_TAG_ELLIPSE:
    case MyHTML_TAG_FEBLEND:
    case MyHTML_TAG_FECOLORMATRIX:
    case MyHTML_TAG_FECOMPONENTTRANSFER:
    case MyHTML_TAG_FECOMPOSITE:
    case MyHTML_TAG_FECONVOLVEMATRIX:
    case MyHTML_TAG_FEDIFFUSELIGHTING:
    case MyHTML_TAG_FEDISPLACEMENTMAP:
    case MyHTML_TAG_FEDISTANTLIGHT:
    case MyHTML_TAG_FEDROPSHADOW:
    case MyHTML_TAG_FEFLOOD:
    case MyHTML_TAG_FEFUNCA:
    case MyHTML_TAG_FEFUNCB:
    case MyHTML_TAG_FEFUNCG:
    case MyHTML_TAG_FEFUNCR:
    case MyHTML_TAG_FEGAUSSIANBLUR:
    case MyHTML_TAG_FEIMAGE:
    case MyHTML_TAG_FEMERGE:
    case MyHTML_TAG_FEMERGENODE:
    case MyHTML_TAG_FEMORPHOLOGY:
    case MyHTML_TAG_FEOFFSET:
    case MyHTML_TAG_FEPOINTLIGHT:
    case MyHTML_TAG_FESPECULARLIGHTING:
    case MyHTML_TAG_FESPOTLIGHT:
    case MyHTML_TAG_FETILE:
    case MyHTML_TAG_FETURBULENCE:
    case MyHTML_TAG_FILTER:
    case MyHTML_TAG_FONT_FACE:
    case MyHTML_TAG_FONT_FACE_FORMAT:
    case MyHTML_TAG_FONT_FACE_NAME:
    case MyHTML_TAG_FONT_FACE_SRC:
    case MyHTML_TAG_FONT_FACE_URI:
    case MyHTML_TAG_FOREIGNOBJECT:
    case MyHTML_TAG_G:
    case MyHTML_TAG_GLYPH:
    case MyHTML_TAG_GLYPHREF:
    case MyHTML_TAG_HKERN:
    case MyHTML_TAG_LINE:
    case MyHTML_TAG_LINEARGRADIENT:
    case MyHTML_TAG_MARKER:
    case MyHTML_TAG_MASK:
    case MyHTML_TAG_METADATA:
    case MyHTML_TAG_MISSING_GLYPH:
    case MyHTML_TAG_MPATH:
    case MyHTML_TAG_PATH:
    case MyHTML_TAG_PATTERN:
    case MyHTML_TAG_POLYGON:
    case MyHTML_TAG_POLYLINE:
    case MyHTML_TAG_RADIALGRADIENT:
    case MyHTML_TAG_RECT:
    case MyHTML_TAG_SET:
    case MyHTML_TAG_STOP:
    case MyHTML_TAG_SWITCH:
    case MyHTML_TAG_SYMBOL:
    case MyHTML_TAG_TEXT:
    case MyHTML_TAG_TEXTPATH:
    case MyHTML_TAG_TREF:
    case MyHTML_TAG_TSPAN:
    case MyHTML_TAG_USE:
    case MyHTML_TAG_VIEW:
    case MyHTML_TAG_VKERN:
    case MyHTML_TAG_MATH:
    case MyHTML_TAG_MACTION:
    case MyHTML_TAG_MALIGNGROUP:
    case MyHTML_TAG_MALIGNMARK:
    case MyHTML_TAG_MENCLOSE:
    case MyHTML_TAG_MERROR:
    case MyHTML_TAG_MFENCED:
    case MyHTML_TAG_MFRAC:
    case MyHTML_TAG_MGLYPH:
    case MyHTML_TAG_MI:
    case MyHTML_TAG_MLABELEDTR:
    case MyHTML_TAG_MLONGDIV:
    case MyHTML_TAG_MMULTISCRIPTS:
    case MyHTML_TAG_MN:
    case MyHTML_TAG_MO:
    case MyHTML_TAG_MOVER:
    case MyHTML_TAG_MPADDED:
    case MyHTML_TAG_MPHANTOM:
    case MyHTML_TAG_MROOT:
    case MyHTML_TAG_MROW:
    case MyHTML_TAG_MS:
    case MyHTML_TAG_MSCARRIES:
    case MyHTML_TAG_MSCARRY:
    case MyHTML_TAG_MSGROUP:
    case MyHTML_TAG_MSLINE:
    case MyHTML_TAG_MSPACE:
    case MyHTML_TAG_MSQRT:
    case MyHTML_TAG_MSROW:
    case MyHTML_TAG_MSTACK:
    case MyHTML_TAG_MSTYLE:
    case MyHTML_TAG_MSUB:
    case MyHTML_TAG_MSUP:
    case MyHTML_TAG_MSUBSUP:
        ss << " ";
        break;
    }
}

struct ToTextVisitor {
    std::stringstream ss;

    bool pre(myhtml_tree_node_t* node) {
        auto tag = myhtml_node_tag_id(node);
        switch (tag) {
        case MyHTML_TAG__END_OF_FILE:
        case MyHTML_TAG__UNDEF:
             return false;

        case MyHTML_TAG_SCRIPT:
        case MyHTML_TAG_STYLE:
        case MyHTML_TAG_TEXTAREA:
        case MyHTML_TAG_OPTION:
        case MyHTML_TAG_SELECT:
          ss << " ";
          return false;

        case MyHTML_TAG__TEXT:
             ss << myhtml_node_text(node, nullptr);
             return false;
        case MyHTML_TAG__COMMENT:
             return false;
        default:
            add_whitespace(tag, ss);
            return true;
        }
    }

    void post(myhtml_tree_node_t* node) {
        auto tag = myhtml_node_tag_id(node);
        add_whitespace(tag, ss);
    }
};

std::string html2text(const Context context, myhtml_tree_node_t* root) {
    ToTextVisitor visitor;
    traverse<MyHtmlAdapter, ToTextVisitor>(visitor, root);
    return visitor.ss.str();
}

void JNICALL Java_com_github_foobar27_myhtml4j_Native_parseUTF8(JNIEnv *env, jclass, jlong c, jstring i, jobject callback) {
    Context* context = (Context*) c;
    JavaCallbackObject cb(env, context->m_callbackClass, JObject(callback));
    const char *input = env->GetStringUTFChars(i, nullptr);
    size_t inputLength = strlen(input);
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

    transferDoctype(wContext, myhtml_tree_get_document(tree));
    transferSubTree(wContext, myhtml_tree_get_node_html(tree));

    // release resources
    myhtml_tree_destroy(tree);
    env->ReleaseStringUTFChars(i, input);
}

jstring JNICALL Java_com_github_foobar27_myhtml4j_Native_html2textUTF8(JNIEnv *env, jclass, jlong c, jstring i) {
    Context* context = (Context*) c;
    const char *input = env->GetStringUTFChars(i, nullptr);
    size_t inputLength = strlen(input);
    // init tree
    myhtml_tree_t* tree = myhtml_tree_create();
    if (!tree) {
        //std::cerr << "myhtml_tree_create failed" << std::endl;
        return nullptr;
    }

    auto res = myhtml_tree_init(tree, context->m_myhtml);
    if (MYHTML_FAILED(res)) {
        std::cerr << "myhtml_tree_init failed with " << res << std::endl;
        myhtml_tree_destroy(tree);
        return nullptr;
    }

    // parse html
    myhtml_parse(tree, MyHTML_ENCODING_UTF_8, input, inputLength);

    auto text = html2text(*context, myhtml_tree_get_node_html(tree));

    // release resources
    myhtml_tree_destroy(tree);
    env->ReleaseStringUTFChars(i, input);
    return stringToJni(env, text);
}
