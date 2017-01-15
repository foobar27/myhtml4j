package com.github.foobar27.myhtml4j;

import com.github.foobar27.myhtml4j.atoms.AttributeKey;
import com.github.foobar27.myhtml4j.atoms.Namespace;
import com.github.foobar27.myhtml4j.atoms.Tag;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

class Native {

    private static final String JNI_LIBRARY_NAME = "myhtml4jnative";

    private static final Native INSTANCE = new Native();

    private final long contextPointer;

    private Native() {
        // guaranteed to be called once (Singleton pattern)
        System.loadLibrary(JNI_LIBRARY_NAME);
        this.contextPointer = newContext(NativeCallBack.class);
    }

    static Native getInstance() {
        return INSTANCE;
    }

    private static native long newContext(Class callback);

    void parseUTF8(String s, NativeCallBack callBack) {
        parseUTF8(contextPointer, s, callBack);
    }

    static native void parseUTF8(long contextPointer, String s, NativeCallBack callback);

    static final class NativeCallBack {

        private final Visitor delegate;
        boolean internalErrorOccurred;

        NativeCallBack(Visitor delegate) {
            if (delegate == null) {
                throw new NullPointerException("Delegate must not be null!");
            }
            this.delegate = delegate;
        }

        void internalError() {
            this.internalErrorOccurred = true;
        }

        void preOrderVisit() {
            delegate.preOrderVisit();
        }

        void createText(String text) {
            delegate.createText(text);
        }

        void createComment(String text) {
            delegate.createComment(text);
        }

        void createElement(int nsId, String nsString, int tagId, String tagString, int[] ids, String[] strings) {
            List<Attribute> attributes;
            if (ids == null) {
                attributes = Collections.emptyList();
            } else {
                attributes = new ArrayList<>(ids.length);
                int stringsId = 0;
                for (int i=0; i<ids.length; i += 2, stringsId += 2) {
                    Namespace ns = Namespace.get(ids[i], null);
                    AttributeKey key = AttributeKey.get(ids[i+1], strings[stringsId]);
                    String value = strings[stringsId + 1];
                    attributes.add(new Attribute(ns, key, value));
                }
            }
            delegate.createElement(Namespace.get(nsId, nsString), Tag.get(tagId, tagString), attributes);
        }
    }

}
