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
            List<String> values = output.get(a.getKey().toString());  // TODO namespace ignored!
            if (values == null) {
                values = new ArrayList<>();
                output.put(a.getKey().toString(), values);
            }
            values.add(a.getValue());
        }
        return output;
    }

}