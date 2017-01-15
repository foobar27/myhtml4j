package com.github.foobar27.myhtml4j.example;

import java.util.List;

public abstract class Node {

    private final List<Node> children;

    protected Node(List<Node> children) {
        this.children = children; // FIXME create immutable copy
    }

    public List<Node> getChildren() {
        return children;
    }

    public abstract String toHtml();

}