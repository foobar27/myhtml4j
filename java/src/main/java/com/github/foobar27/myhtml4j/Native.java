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
package com.github.foobar27.myhtml4j;

import com.github.foobar27.myhtml4j.atoms.AttributeKey;
import com.github.foobar27.myhtml4j.atoms.Namespace;
import com.github.foobar27.myhtml4j.atoms.Tag;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

class Native {

    private final long contextPointer;

    Native() {
        this.contextPointer = newContext(NativeCallBack.class);
    }

    private static native long newContext(Class callback);

    void parseUTF8(String s, NativeCallBack callBack) {
        parseUTF8(contextPointer, s, callBack);
    }

    static native void parseUTF8(long contextPointer, String s, NativeCallBack callback);

    String html2textUTF8(String s) {
        String result = html2textUTF8(contextPointer, s);
        if (result == null) {
            throw new InternalError("Internal error in html2text");
        }
        return result;
    }

    static native String html2textUTF8(long contextPointer, String s);

    static final class NativeCallBack {

        private final Visitor delegate;
        boolean internalErrorOccurred;
        private final ArrayList<String> knownTagNames = new ArrayList<>();
        private static final int MAX_KNOWN_TAG_ID = 194;

        NativeCallBack(Visitor delegate) {
            if (delegate == null) {
                throw new NullPointerException("Delegate must not be null!");
            }
            this.delegate = delegate;
        }

        void setDoctype(String[] args) {
            delegate.setDoctype(Arrays.asList(args));
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
                for (int i = 0; i < ids.length; i += 2, stringsId += 2) {
                    Namespace ns = Namespace.get(ids[i], null);
                    AttributeKey key = AttributeKey.get(ids[i + 1], strings[stringsId]);
                    String value = strings[stringsId + 1];
                    attributes.add(new Attribute(ns, key, value));
                }
            }
            if (tagId > MAX_KNOWN_TAG_ID) {
                // The tag is not predefined, let's see if it has already been transferred.
                int knownTagIndex = tagId - MAX_KNOWN_TAG_ID - 1;
                if (knownTagIndex < knownTagNames.size() && tagString == null) {
                    tagString = knownTagNames.get(knownTagIndex);
                } else {
                    int nullElements = tagId - MAX_KNOWN_TAG_ID - knownTagNames.size();
                    for (int i = 0; i < nullElements; ++i) {
                        knownTagNames.add(null);
                    }
                    knownTagNames.set(knownTagIndex, tagString);
                }
                tagId = -1; // Do as if it was a new tag.
            }
            delegate.createElement(Namespace.get(nsId, nsString), Tag.get(tagId, tagString), attributes);
        }
    }

}
