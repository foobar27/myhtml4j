package com.github.foobar27.myhtml4j;

import com.github.foobar27.myhtml4j.example.*;
import org.junit.Test;

import static org.junit.Assert.assertEquals;

public class Myhtml4jTest {

    @Test
    public void parseShouldAddClosingTag() {
        Node root = Myhtml4j.getInstance().parseUTF8("<p id=\"1\" class=\"bold\">foo", NodeSink::new);
        assertEquals("Element[html,html,{},[Element[html,head,{},[]], Element[html,body,{},[Element[html,p,{id=[1], class=[bold]},[Text[foo]]]]]]]",
                root.toString());
    }

    @Test
    public void parseShouldNoMixUpNeighbours() {
        Node root = Myhtml4j.getInstance().parseUTF8("<ul><li>a1<li><li>a2<li></ul><ul><li>b1<li><li>b2<li></ul>", NodeSink::new);
        assertEquals("<html><head></head><body><ul><li>a1</li><li></li><li>a2</li><li></li></ul><ul><li>b1</li><li></li><li>b2</li><li></li></ul></body></html>",
                root.toHtml());
    }

}