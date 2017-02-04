package com.github.foobar27.myhtml4j;

import java.util.List;
import java.util.Optional;

public class Document<N> {

    private final Optional<List<String>> doctype;
    private final N root;

    public Document(Optional<List<String>> doctype, N root) {
        this.doctype = doctype;
        this.root = root;
    }

    public Optional<List<String>> getDoctype() {
        return doctype;
    }

    public N getRoot() {
        return root;
    }

    @Override
    public String toString() {
        return "Document[" + doctype + "," + root + "]";
    }
}
