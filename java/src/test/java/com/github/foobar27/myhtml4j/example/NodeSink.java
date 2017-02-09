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

import com.github.foobar27.myhtml4j.Attribute;
import com.github.foobar27.myhtml4j.Sink;
import com.github.foobar27.myhtml4j.atoms.Namespace;
import com.github.foobar27.myhtml4j.atoms.Tag;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class NodeSink implements Sink<Node> {

    @Override
    public Node createText(String text) {
        return new Text(text);
    }

    @Override
    public Node createComment(String text) {
        return new Comment(text);
    }

    @Override
    public Node createElement(Namespace ns, Tag tag, List<Attribute> attributes, List<Node> children) {
        return new Element(
                ns.toString(),
                tag.name(),
                convertAttributes(attributes),
                children);
    }

    private static Map<String, List<String>> convertAttributes(List<Attribute> input) {
        // TODO should not ignore ns
        Map<String, List<String>> output = new HashMap<>();
        for (Attribute a : input) {
            List<String> values = output.computeIfAbsent(a.getKey().toString(), k -> new ArrayList<>());
            values.add(a.getValue());
        }
        return output;
    }

}