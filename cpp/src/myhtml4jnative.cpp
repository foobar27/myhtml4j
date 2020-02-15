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

#include "lexbor/core/base.h"
#include "lexbor/core/types.h"
#include "lexbor/html/parser.h"
#include "lexbor/tag/const.h"
#include "lexbor/dom/interfaces/document_type.h"

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
    JNamespace(Namespace ns)
      : IdJString(ns.id, ns.name)
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
    JClass stringClass;

    Context(JNIEnv *env, jclass callbackClass)
        : m_callbackClass(env, callbackClass)
        , stringClass(JClass::load(env, "java/lang/String").globalRef(env))
    {}

};

jlong JNICALL Java_com_github_foobar27_myhtml4j_Native_newContext(JNIEnv *env, jclass, jclass callbackClass) {
    return (jlong) new Context(env, callbackClass);
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
    NamespaceCache namespaceCache;
    lxb_dom_node_t* tree;
    std::unordered_map<lxb_tag_id_t, int32_t> seenTags;
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

JniNodeAttributes flatten_attributes(WalkContext & wc, lxb_dom_node_t* root) {
    lxb_dom_attr* attr = lxb_dom_element_first_attribute(lxb_dom_interface_element(root));
    if (!attr) {
        return {nullptr, nullptr};
    }
    std::vector<uint32_t> ids;
    std::vector<jstring> strings;
    while (attr) {
      //        auto aNs = lxb_dom_attr_prefix(attr, nullptr);
      int32_t aNs = 0; // TODO
      //std::cerr << "qualified name: " << lxb_dom_attr_qualified_name(attr, nullptr) << std::endl;
        auto aKey = wc.attributeKeyCache.get((const char*) lxb_dom_attr_local_name(attr, nullptr));
        auto aValue = charArrayToJni(wc.env, (const char*) lxb_dom_attr_value(attr, nullptr));

        ids.push_back(aNs);
        ids.push_back(aKey.id);
        strings.push_back(aKey.name);
        strings.push_back(aValue);

        attr = lxb_dom_element_next_attribute(attr);
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

void transferDoctype(WalkContext & wc, lxb_dom_document_t* document) {
    if (!document) {
        return;
    }
    auto doctype = document->doctype;
    if (!doctype) {
        return;
    }
    std::vector<jstring> strings;
    strings.push_back(wc.env->NewStringUTF((const char*) lxb_dom_document_type_name(doctype, nullptr)));
    strings.push_back(wc.env->NewStringUTF((const char*) lxb_dom_document_type_public_id(doctype, nullptr)));
    strings.push_back(wc.env->NewStringUTF((const char*) lxb_dom_document_type_system_id(doctype, nullptr)));
    wc.cb.setDoctype(stringArrayToObjectArray(wc, strings));
}

struct TransferTreeVisitor {
    using Node = lxb_dom_node_t*;

    WalkContext & wc;

    bool pre(Node node) {
        auto tag = lxb_dom_node_tag_id(node);
        switch (tag) {
         case LXB_TAG__END_OF_FILE:
         case LXB_TAG__UNDEF:
 	 case LXB_TAG__EM_DOCTYPE:
	   return false;
 	 case LXB_TAG__DOCUMENT:
	   return true; // post-stage will be skipped
         case LXB_TAG__TEXT:
             {
	       auto txt = lxb_dom_node_text_content(node, nullptr);
                if (txt) {
  		    wc.cb.createText((const char*) txt);
                }
             }
             return false;
         case LXB_TAG__EM_COMMENT:
             {
                auto txt = lxb_dom_node_text_content(node, nullptr);
                if (txt) {
		    wc.cb.createComment((const char*) txt);
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
        auto tag = lxb_dom_node_tag_id(node);
	if (tag == LXB_TAG__DOCUMENT) {
	  return; // skip (was also skipped during pre-stage)
	}
        int32_t signed_tag = tag;
        jstring tag_name = nullptr;
	// The first MAX_TAG_INDEX tag names are predefined, in lexbor as well as in the java counterpart.
	// If we encounter such a tag, we don't need to send any string.
	// If we encounter a tag outside of this range, we will translate to an incremental tag id,
	// and send it to the java side. The second time we will skip the string, because the java side
	// is able to do the lookup itself.
        if (tag > MAX_TAG_INDEX) {
	    auto it = wc.seenTags.find(tag);
	    if (it != wc.seenTags.end()) {
	      // We have already seen this tag, no need to send the string.
	      signed_tag = it->second;
	    } else {
	      // We haven't seen this tag yet.
	      signed_tag = wc.seenTags.size() + MAX_TAG_INDEX + 1;
	      wc.seenTags[tag] = signed_tag;
	      tag_name = charArrayToJni(wc.env, (const char*) lxb_dom_element_tag_name(lxb_dom_interface_element(node), nullptr));
	    }
        }
	auto nsString = lxb_dom_element_prefix(lxb_dom_interface_element(node), nullptr);
	auto ns = nsString
	  ? wc.namespaceCache.get((const char*) nsString)
	  : wc.namespaceCache.get("html"); // TODO optimize // TODO is this even correct?
        auto attributes = flatten_attributes(wc, node);
        wc.cb.createElement(ns, {signed_tag, tag_name}, attributes.ids, attributes.strings);
    }

};

void transferSubTree(WalkContext & wc, lxb_dom_node_t* root) {
    TransferTreeVisitor visitor { wc };
    traverse<MyHtmlAdapter, TransferTreeVisitor>(visitor, root);
}

void add_whitespace(unsigned int tag, std::stringstream & ss) {
    switch (tag) {
    // treat the following as whitespace: http://www.javased.com/?source_dir=ihtika/Sources/Bundles/JerichoBundle/src/main/java/net/htmlparser/jericho/TextExtractor.java
    case LXB_TAG_BR:
    case LXB_TAG_ADDRESS:
    case LXB_TAG_ANNOTATION_XML:
    case LXB_TAG_AREA:
    case LXB_TAG_ARTICLE:
    case LXB_TAG_ASIDE:
    case LXB_TAG_AUDIO:
    case LXB_TAG_BASE:
    case LXB_TAG_BGSOUND:
    case LXB_TAG_BLINK:
    case LXB_TAG_BLOCKQUOTE:
    case LXB_TAG_BODY:
    case LXB_TAG_CANVAS:
    case LXB_TAG_CAPTION:
    case LXB_TAG_CENTER:
    case LXB_TAG_COL:
    case LXB_TAG_COLGROUP:
    case LXB_TAG_DATALIST:
    case LXB_TAG_DD:
    case LXB_TAG_DETAILS:
    case LXB_TAG_DIALOG:
    case LXB_TAG_DIR:
    case LXB_TAG_DIV:
    case LXB_TAG_DL:
    case LXB_TAG_DT:
    case LXB_TAG_EMBED:
    case LXB_TAG_FIELDSET:
    case LXB_TAG_FIGCAPTION:
    case LXB_TAG_FIGURE:
    case LXB_TAG_FOOTER:
    case LXB_TAG_FORM:
    case LXB_TAG_FRAME:
    case LXB_TAG_FRAMESET:
    case LXB_TAG_H1:
    case LXB_TAG_H2:
    case LXB_TAG_H3:
    case LXB_TAG_H4:
    case LXB_TAG_H5:
    case LXB_TAG_H6:
    case LXB_TAG_HEAD:
    case LXB_TAG_HEADER:
    case LXB_TAG_HGROUP:
    case LXB_TAG_HR:
    case LXB_TAG_HTML:
    case LXB_TAG_IMAGE:
    case LXB_TAG_ISINDEX:
    case LXB_TAG_LEGEND:
    case LXB_TAG_LI:
    case LXB_TAG_LINK:
    case LXB_TAG_LISTING:
    case LXB_TAG_MAIN:
    case LXB_TAG_MARQUEE:
    case LXB_TAG_MENU:
    case LXB_TAG_META:
    case LXB_TAG_MTEXT:
    case LXB_TAG_NAV:
    case LXB_TAG_NOBR:
    case LXB_TAG_NOEMBED:
    case LXB_TAG_NOFRAMES:
    case LXB_TAG_NOSCRIPT:
    case LXB_TAG_OL:
    case LXB_TAG_OPTGROUP:
    case LXB_TAG_OPTION:
    case LXB_TAG_P:
    case LXB_TAG_PARAM:
    case LXB_TAG_PLAINTEXT:
    case LXB_TAG_PRE:
    case LXB_TAG_RB:
    case LXB_TAG_RTC:
    case LXB_TAG_SECTION:
    case LXB_TAG_SOURCE:
    case LXB_TAG_STYLE:
    case LXB_TAG_SUMMARY:
    case LXB_TAG_SVG:
    case LXB_TAG_TABLE:
    case LXB_TAG_TBODY:
    case LXB_TAG_TD:
    case LXB_TAG_TEMPLATE:
    case LXB_TAG_TFOOT:
    case LXB_TAG_TH:
    case LXB_TAG_THEAD:
    case LXB_TAG_TITLE:
    case LXB_TAG_TR:
    case LXB_TAG_TRACK:
    case LXB_TAG_UL:
    case LXB_TAG_VIDEO:
    case LXB_TAG_XMP:
    case LXB_TAG_ALTGLYPH:
    case LXB_TAG_ALTGLYPHDEF:
    case LXB_TAG_ALTGLYPHITEM:
    case LXB_TAG_ANIMATECOLOR:
    case LXB_TAG_ANIMATEMOTION:
    case LXB_TAG_ANIMATETRANSFORM:
    case LXB_TAG_CLIPPATH:
    case LXB_TAG_DESC:
    case LXB_TAG_FEBLEND:
    case LXB_TAG_FECOLORMATRIX:
    case LXB_TAG_FECOMPONENTTRANSFER:
    case LXB_TAG_FECOMPOSITE:
    case LXB_TAG_FECONVOLVEMATRIX:
    case LXB_TAG_FEDIFFUSELIGHTING:
    case LXB_TAG_FEDISPLACEMENTMAP:
    case LXB_TAG_FEDISTANTLIGHT:
    case LXB_TAG_FEDROPSHADOW:
    case LXB_TAG_FEFLOOD:
    case LXB_TAG_FEFUNCA:
    case LXB_TAG_FEFUNCB:
    case LXB_TAG_FEFUNCG:
    case LXB_TAG_FEFUNCR:
    case LXB_TAG_FEGAUSSIANBLUR:
    case LXB_TAG_FEIMAGE:
    case LXB_TAG_FEMERGE:
    case LXB_TAG_FEMERGENODE:
    case LXB_TAG_FEMORPHOLOGY:
    case LXB_TAG_FEOFFSET:
    case LXB_TAG_FEPOINTLIGHT:
    case LXB_TAG_FESPECULARLIGHTING:
    case LXB_TAG_FESPOTLIGHT:
    case LXB_TAG_FETILE:
    case LXB_TAG_FETURBULENCE:
    case LXB_TAG_FOREIGNOBJECT:
    case LXB_TAG_GLYPHREF:
    case LXB_TAG_LINEARGRADIENT:
    case LXB_TAG_PATH:
    case LXB_TAG_RADIALGRADIENT:
    case LXB_TAG_TEXTPATH:
    case LXB_TAG_MATH:
    case LXB_TAG_MALIGNMARK:
    case LXB_TAG_MFENCED:
    case LXB_TAG_MGLYPH:
    case LXB_TAG_MI:
    case LXB_TAG_MN:
    case LXB_TAG_MO:
    case LXB_TAG_MS:
        ss << " ";
        break;
    }
}

struct ToTextVisitor {
    std::stringstream ss;

    bool pre(lxb_dom_node_t* node) {
        auto tag = lxb_dom_node_tag_id(node);
        switch (tag) {
        case LXB_TAG__END_OF_FILE:
        case LXB_TAG__UNDEF:
             return false;

        case LXB_TAG_SCRIPT:
        case LXB_TAG_STYLE:
        case LXB_TAG_TEXTAREA:
        case LXB_TAG_OPTION:
        case LXB_TAG_SELECT:
          ss << " ";
          return false;

        case LXB_TAG__TEXT:
             ss << lxb_dom_node_text_content(node, nullptr);
             return false;
        case LXB_TAG__EM_COMMENT:
             return false;
        default:
            add_whitespace(tag, ss);
            return true;
        }
    }

    void post(lxb_dom_node_t* node) {
        auto tag = lxb_dom_node_tag_id(node);
        add_whitespace(tag, ss);
    }
};

std::string html2text(const Context context, lxb_dom_node_t* root) {
    ToTextVisitor visitor;
    traverse<MyHtmlAdapter, ToTextVisitor>(visitor, root);
    return visitor.ss.str();
}

void JNICALL Java_com_github_foobar27_myhtml4j_Native_parseUTF8(JNIEnv *env, jclass, jlong c, jstring i, jobject callback) {
    Context* context = (Context*) c;
    JavaCallbackObject cb(env, context->m_callbackClass, JObject(callback));
    const char *input = env->GetStringUTFChars(i, nullptr);
    lxb_status_t status;
    auto inputLength = strlen(input);
    // init tree
    auto document = lxb_html_document_create();
    if (!document) {
        std::cerr << "lxb_html_document_create() failed" << std::endl;
        env->ReleaseStringUTFChars(i, input);
        cb.internalError();
        return;
    }

    // TODO how do we know it's UTF8?
    status = lxb_html_document_parse(document, (const lxb_char_t*) input, inputLength);
    if (status != LXB_STATUS_OK) {
        std::cerr << "lxb_html_document_parse() failed" << std::endl;
        lxb_html_document_destroy(document);
        env->ReleaseStringUTFChars(i, input);
        cb.internalError();
        return;      
    }

    auto tree = lxb_dom_interface_node(document);

    WalkContext wContext {env, context->stringClass, cb, {env}, {env}, tree, {}};

    transferDoctype(wContext, lxb_dom_interface_document(document));
    transferSubTree(wContext, tree);

    // release resources
    lxb_html_document_destroy(document);
    env->ReleaseStringUTFChars(i, input);
}

jstring JNICALL Java_com_github_foobar27_myhtml4j_Native_html2textUTF8(JNIEnv *env, jclass, jlong c, jstring i) {
    Context* context = (Context*) c;
    const char *input = env->GetStringUTFChars(i, nullptr);
    lxb_status_t status;
    size_t inputLength = strlen(input);
    // TODO how do we know it's UTF8?
    auto document = lxb_html_document_create();
    if (!document) {
        std::cerr << "lxb_html_document_create() failed" << std::endl;
        env->ReleaseStringUTFChars(i, input);
        return nullptr;
    }

    status = lxb_html_document_parse(document, (const lxb_char_t*) input, inputLength);
    if (status != LXB_STATUS_OK) {
        std::cerr << "lxb_html_document_parse() failed" << std::endl;
        lxb_html_document_destroy(document);
        env->ReleaseStringUTFChars(i, input);
        return nullptr; // TODO is this the correct way to handle the error?
    }

    auto tree = lxb_dom_interface_node(document);
    auto text = html2text(*context, tree);

    // release resources
    lxb_html_document_destroy(document);
    env->ReleaseStringUTFChars(i, input);
    return stringToJni(env, text);
}
