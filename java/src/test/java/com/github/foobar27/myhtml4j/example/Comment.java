package com.github.foobar27.myhtml4j.example;

import java.util.Collections;

public class Comment extends Node {

    private final String text;

    public Comment(String text) {
        super(Collections.emptyList());
        this.text = text;
    }

    @Override
    public String toString() {
        return String.format("Comment[%s]", text);
    }

    @Override
    public String toHtml() {
        return String.format("<!-- %s -->",
                text.replaceAll("--", "__"));
    }

}