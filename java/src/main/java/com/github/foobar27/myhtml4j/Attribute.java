package com.github.foobar27.myhtml4j;

import com.github.foobar27.myhtml4j.atoms.AttributeKey;
import com.github.foobar27.myhtml4j.atoms.Namespace;

public final class Attribute {
    private final Namespace ns;
    private final AttributeKey key;
    private final String value;

    public Attribute(Namespace ns, AttributeKey key, String value) {
        this.ns = ns;
        this.key = key;
        this.value = value;
    }

    public Namespace getNamespace() {
        return ns;
    }

    public AttributeKey getKey() {
        return key;
    }

    public String getValue() {
        return value;
    }
}
