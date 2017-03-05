package com.github.foobar27.myhtml4j;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import java.util.Arrays;
import java.util.Collection;

import static com.github.foobar27.myhtml4j.TestUtils.parse;
import static org.junit.Assert.assertEquals;

@RunWith(Parameterized.class)
public class ParserTest {

    @Parameters
    public static Collection<Object[]> data() {
        return Arrays.asList(new Object[][]{
                // carriage return
                {
                        "<div>Foo\r</div>",
                        "Document[Optional.empty,Element[html,html,{},[Element[html,head,{},[]], Element[html,body,{},[Element[html,div,{},[Text[Foo]]]]]]]]"
                },
                // should add closing tag
                {
                        "<p id=\"1\" class=\"bold\">foo",
                        "Document[Optional.empty,Element[html,html,{},[Element[html,head,{},[]], Element[html,body,{},[Element[html,p,{id=[1], class=[bold]},[Text[foo]]]]]]]]"
                },
                // unknown namespace
                {
                        "<html xmlns:tpl=\"http://example.com/\"><body><h1 tpl:bar=\"baz\">World!</h1></body></html>",
                        "Document[Optional.empty,Element[html,html,{xmlns:tpl=[http://example.com/]},[Element[html,head,{},[]], Element[html,body,{},[Element[html,h1,{tpl:bar=[baz]},[Text[World!]]]]]]]]"
                },
                // html5 doctype
                {
                        "<!DOCTYPE html><html><head><title>Hello world</title></head></body></html>",
                        "Document[Optional[[html]],Element[html,html,{},[Element[html,head,{},[Element[html,title,{},[Text[Hello world]]]]], Element[html,body,{},[]]]]]"
                },
                // html4 doctype
                {
                        "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Frameset//EN\" \"http://www.w3.org/TR/html4/frameset.dtd\"><html><head><title>Hello world</title></head></body></html>",
                        "Document[Optional[[html, PUBLIC, -//W3C//DTD HTML 4.01 Frameset//EN, http://www.w3.org/TR/html4/frameset.dtd]],Element[html,html,{},[Element[html,head,{},[Element[html,title,{},[Text[Hello world]]]]], Element[html,body,{},[]]]]]"
                }});
    }

    private final String input;
    private final String expected;

    public ParserTest(String input, String expected) {
        this.input = input;
        this.expected = expected;
    }

    @Test
    public void test() {
        assertEquals(expected, parse(input).toString());
    }

}
