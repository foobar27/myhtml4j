package com.github.foobar27.myhtml4j.example;

import com.google.common.html.HtmlEscapers;

import java.util.Collections;

public class Text extends Node {

    private final String text;

    public Text(String text) {
        super(Collections.emptyList());
        this.text = text.trim();
    }

    @Override
    public String toString() {
        return String.format("Text[%s]", text);
    }

    @Override
    public String toHtml() {
        return HtmlEscapers.htmlEscaper().escape(text);
    }

}