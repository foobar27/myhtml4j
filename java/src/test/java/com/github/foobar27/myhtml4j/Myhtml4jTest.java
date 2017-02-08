package com.github.foobar27.myhtml4j;

import com.github.foobar27.myhtml4j.example.*;
import org.junit.Test;

import static org.junit.Assert.assertEquals;

public class Myhtml4jTest {

    @Test
    public void html2textList() {
        assertEquals("Foo Bar Baz",
                html2text("<ul><li>Foo</li><li>Bar</li><li>Baz</li></ul>"));
    }

    @Test
    public void html2textEntities() {
        assertEquals("Sébastien Wagener",
                html2text("<div>S&eacute;bastien&nbsp;Wagener</div>"));
    }

    @Test
    public void html2textWhitespace() {
        assertEquals("a b c",
                html2text("a<br>b<div>c</div>"));
    }

    @Test
    public void parsingTwice() {
        Node root1 = parse("<p id=\"1\" class=\"bold\">foo").getRoot();
        Node root2 = parse("<p id=\"1\" class=\"bold\">foo").getRoot();

        assertEquals(root1.toString(), root2.toString());
        assertEquals("Element[html,html,{},[Element[html,head,{},[]], Element[html,body,{},[Element[html,p,{id=[1], class=[bold]},[Text[foo]]]]]]]",
                root1.toString());
    }

    @Test
    public void unicodeParasing() {
        Node root = parse("<img alt=\"üüüüüüüü\"></img>").getRoot();
        assertEquals("<html><head></head><body><img alt=\"üüüüüüüü\"></img></body></html>",
                root.toHtml());
    }

    @Test
    public void parseShouldAddClosingTag() {
        Node root = parse("<p id=\"1\" class=\"bold\">foo").getRoot();
        assertEquals("Element[html,html,{},[Element[html,head,{},[]], Element[html,body,{},[Element[html,p,{id=[1], class=[bold]},[Text[foo]]]]]]]",
                root.toString());
    }

    @Test
    public void parseShouldNoMixUpNeighbours() {
        Node root = parse("<ul><li>a1<li><li>a2<li></ul><ul><li>b1<li><li>b2<li></ul>").getRoot();
        assertEquals("<html><head></head><body><ul><li>a1</li><li></li><li>a2</li><li></li></ul><ul><li>b1</li><li></li><li>b2</li><li></li></ul></body></html>",
                root.toHtml());
    }

    @Test
    public void unknownAttributeKey() {
        // This tests twice the same attribute.
        // The second time should be a caches jstring (which we don't test here).
        Node root = parse("<div foo=\"bar\">Text</div><div foo=\"baz\">").getRoot();
        assertEquals("<html><head></head><body><div foo=\"bar\">Text</div><div foo=\"baz\"></div></body></html>",
                root.toHtml());
    }

    @Test
    public void unknownTagName() {
        Node root = parse("<foo>Text1</foo><bar>Text2</bar><foo>Text3</foo/").getRoot();
        assertEquals("<html><head></head><body><foo>Text1</foo><bar>Text2</bar><foo>Text3</foo></body></html>",
                root.toHtml());
    }
    @Test
    public void customNamespaces() {
        String html = "<html xmlns:tpl=\"http://example.com/\"><body><h1 tpl:bar=\"baz\">World!</h1></body></html>";
        Node root = parse(html).getRoot();
        assertEquals("Element[html,html,{xmlns:tpl=[http://example.com/]},[Element[html,head,{},[]], Element[html,body,{},[Element[html,h1,{tpl:bar=[baz]},[Text[World!]]]]]]]",
                root.toString());
    }

    @Test
    public void html5Doctype() {
        String html = "<!DOCTYPE html><html><head><title>Hello world</title></head></body></html>";
        Document root = parse(html);
        assertEquals("Document[Optional[[html]],Element[html,html,{},[Element[html,head,{},[Element[html,title,{},[Text[Hello world]]]]], Element[html,body,{},[]]]]]",
                root.toString());
    }

    @Test
    public void html4Doctype() {
        String html = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Frameset//EN\" \"http://www.w3.org/TR/html4/frameset.dtd\"><html><head><title>Hello world</title></head></body></html>";
        Document root = parse(html);
        assertEquals("Document[Optional[[html, PUBLIC, -//W3C//DTD HTML 4.01 Frameset//EN, http://www.w3.org/TR/html4/frameset.dtd]],Element[html,html,{},[Element[html,head,{},[Element[html,title,{},[Text[Hello world]]]]], Element[html,body,{},[]]]]]",
                root.toString());
    }

    private static String html2text(String html) {
        return Myhtml4j.getInstance().html2textUTF8(html);
    }

    private static Document<Node> parse(String html) {
        return Myhtml4j.getInstance().parseUTF8(html, NodeSink::new);
    }

}