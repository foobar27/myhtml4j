package com.github.foobar27.myhtml4j;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import java.util.Arrays;
import java.util.Collection;

import static com.github.foobar27.myhtml4j.TestUtils.html2text;
import static org.junit.Assert.assertEquals;

@RunWith(Parameterized.class)
public class Html2textTest {

    @Parameters
    public static Collection<Object[]> data() {
        return Arrays.asList(new Object[][]{
                {
                        "<ul><li>Foo</li><li>Bar</li><li>Baz</li></ul>",
                        "Foo Bar Baz"
                },
                // entities
                {
                        "<div>S&eacute;bastien&nbsp;Wagener</div>",
                        "Sébastien Wagener"
                },
                // whitespace handling
                {
                        "a<br>b<div>c</div>",
                        "a b c"
                },
                // whitespace handling for inline elements
                {
                        "Hello w<b>orl</b>d",
                        "Hello world",
                },
                {
                        "Hello w<b></b>orld",
                        "Hello world"
                },
                // whitespace should also be added for removed block element
                {
                        "a<script></script>b",
                        "a b"
                },
                // <select> should be removed
                {
                        "a<select><option>Foo</option></select>b",
                        "a b"
                },
                // final carriage return (used to trigger a bug in myhtml)
                {
                        "<div>Foo\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r</div>a",
                        "Foo a",
                },
                {
                        "<div>Foo\rBar</div>",
                        "Foo Bar"
                }});
    }

    private final String input;
    private final String expected;

    public Html2textTest(String input, String expected) {
        this.input = input;
        this.expected = expected;
    }

    @Test
    public void test() {
        assertEquals(expected, html2text(input));
    }

}
