package com.github.foobar27.myhtml4j;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import java.util.Arrays;
import java.util.Collection;

import static com.github.foobar27.myhtml4j.TestUtils.tidy;
import static org.junit.Assert.assertEquals;

@RunWith(Parameterized.class)
public class TidyTest {
    @Parameters
    public static Collection<Object[]> data() {
        return Arrays.asList(new Object[][]{

                // unicode
                {
                        "<img alt=\"üüüüüüüü\"></img>",
                        "<html><head></head><body><img alt=\"üüüüüüüü\"></img></body></html>"
                },
                // should not mix up neighbours
                {
                        "<ul><li>a1<li><li>a2<li></ul><ul><li>b1<li><li>b2<li></ul>",

                        "<html><head></head><body><ul><li>a1</li><li></li><li>a2</li><li></li></ul><ul><li>b1</li><li></li><li>b2</li><li></li></ul></body></html>"
                },
                {
                        // This tests twice the same attribute.
                        // The second time should be a caches jstring (which we don't test here).
                        "<div foo=\"bar\">Text</div><div foo=\"baz\">",
                        "<html><head></head><body><div foo=\"bar\">Text</div><div foo=\"baz\"></div></body></html>"
                },
                // unknown tag names (sequential)
                {

                        "<foo>Text1</foo><bar>Text2</bar><foo>Text3</foo>",
                        "<html><head></head><body><foo>Text1</foo><bar>Text2</bar><foo>Text3</foo></body></html>"

                },
                // unknown tag names (nested)
                {
                        "<div><li><la><lu></lu></la></li></div>",
                        "<html><head></head><body><div><li><la><lu></lu></la></li></div></body></html>"
                },
                // more nested unknown tag names, which actually broke parser 0.1.2
                {
                        "<x2><div><x0></x0></div><div><x2></x2></div></x2>",
                        "<html><head></head><body><x2><div><x0></x0></div><div><x2></x2></div></x2></body></html>"
                },
                {
                        "<x1><x0></x0><x1></x1></x1>",
                        "<html><head></head><body><x1><x0></x0><x1></x1></x1></body></html>"
                }});
    }

    private final String input;
    private final String expected;

    public TidyTest(String input, String expected) {
        this.input = input;
        this.expected = expected;
    }

    @Test
    public void test() {
        assertEquals(expected, tidy(input));
    }

}
