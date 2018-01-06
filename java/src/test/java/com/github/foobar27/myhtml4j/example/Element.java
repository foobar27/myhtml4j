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
package com.github.foobar27.myhtml4j.example;

import com.google.common.escape.Escaper;
import com.google.common.html.HtmlEscapers;

import java.util.List;
import java.util.Map;

public class Element extends Node {

    private final String ns;
    private final String tag;
    private final Map<String, List<String>> attributes;

    public Element(String ns, String tag, Map<String, List<String>> attributes, List<Node> children) {
        super(children);
        this.ns = ns;
        this.tag = tag;
        this.attributes = attributes;
    }

    public String getNs() {
        return ns;
    }

    public String getTag() {
        return tag;
    }

    public Map<String, List<String>> getAttributes() {
        return attributes;
    }

    @Override
    public String toString() {
        return String.format("Element[%s,%s,%s,%s]",
                ns,
                tag,
                attributes,
                getChildren());
    }

    @Override
    public String toHtml() {
        Escaper escaper = HtmlEscapers.htmlEscaper();
        StringBuilder sb = new StringBuilder();
        sb.append("<");
        sb.append(tag);
        for (Map.Entry<String, List<String>> as : attributes.entrySet()) {
            String key = as.getKey();
            for (String value : as.getValue()) {
                sb.append(" ");
                sb.append(key);
                if (value != null) {
                    sb.append("=\"");
                    sb.append(escaper.escape(value));
                    sb.append("\"");
                }
            }
        }
        sb.append(">");

        for (Node child : getChildren()) {
            sb.append(child.toHtml());
        }

        sb.append("</");
        sb.append(tag);
        sb.append(">");
        return sb.toString();
    }

}