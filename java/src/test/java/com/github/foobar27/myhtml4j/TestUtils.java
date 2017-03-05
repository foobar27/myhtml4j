package com.github.foobar27.myhtml4j;

import com.github.foobar27.myhtml4j.example.Node;
import com.github.foobar27.myhtml4j.example.NodeSink;

final class TestUtils {

    static String html2text(String html) {
        return Myhtml4j.getInstance().html2textUTF8(html);
    }

    static Document<Node> parse(String html) {
        return Myhtml4j.getInstance().parseUTF8(html, NodeSink::new);
    }

    static String tidy(String input) {
        return parse(input).getRoot().toHtml();
    }

}
