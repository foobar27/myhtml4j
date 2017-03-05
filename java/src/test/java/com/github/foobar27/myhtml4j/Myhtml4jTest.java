/*
 * This file is part of the myhtml4j distribution.
 *   (https://github.com/foobar27/myhtml4j)
 * Copyright (c) 2017 Sebastien Wagener.
 *
 * myhtml4j is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, version 3.
 *
 * myhtml4j is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
package com.github.foobar27.myhtml4j;

import com.github.foobar27.myhtml4j.atoms.Namespace;
import com.github.foobar27.myhtml4j.atoms.Tag;
import com.github.foobar27.myhtml4j.example.Node;
import org.junit.Test;

import java.util.List;
import java.util.logging.Logger;

import static com.github.foobar27.myhtml4j.TestUtils.parse;
import static com.github.foobar27.myhtml4j.TestUtils.tidy;
import static org.junit.Assert.assertEquals;

public class Myhtml4jTest {

    private static final Logger log = Logger.getLogger(Myhtml4j.class.getName());

    @Test
    public void testParsingCarriageReturn() {
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
    public void nested() {
        int depth = 100; // TODO test for StackOverflow (but would need an iterative
        StringBuilder sb = new StringBuilder();
        sb.append("<html><head></head><body>");
        for (int i = 0; i < depth; ++i) {
            sb.append("<div id=\"").append(i).append("\">");
        }
        for (int i = 0; i < depth; ++i) {
            sb.append("</div>");
        }
        sb.append("</body></html>");
        String html = sb.toString();
        log.info("length: " + html.length());
        assertEquals(html, tidy(html));
    }

    @Test(expected = IllegalStateException.class)
    public void callbackExceptionHandling() {
        String html = "<div>Foo</div>";
        Myhtml4j.getInstance().parseUTF8(html, new Visitor() {
            @Override
            public void setDoctype(List<String> dt) {

            }

            @Override
            public void preOrderVisit() {

            }

            @Override
            public void createText(String text) {

            }

            @Override
            public void createComment(String text) {

            }

            @Override
            public void createElement(Namespace ns, Tag tag, List<Attribute> attributes) {
                throw new IllegalStateException("foo");
            }
        });
    }

}