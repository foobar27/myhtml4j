package com.github.foobar27.myhtml4j;

import com.github.foobar27.myhtml4j.atoms.AttributeKey;
import com.github.foobar27.myhtml4j.atoms.Namespace;
import com.github.foobar27.myhtml4j.atoms.Tag;

public class Parser {

    public static final class Attribute {
        private final Namespace ns;
        private final AttributeKey key;
        private final String value;

        public Attribute(Namespace ns, AttributeKey key, String value) {
            this.ns = ns;
            this.key = key;
            this.value = value;
        }
    }

    public interface CallBack {
        void preOrderVisit();
        void createText(String s);
        void createComment(String s);
        void createElement(Namespace ns, Tag tag, Attribute[] attributes);
    }

}