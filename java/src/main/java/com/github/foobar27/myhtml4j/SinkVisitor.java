package com.github.foobar27.myhtml4j;

import com.github.foobar27.myhtml4j.atoms.Namespace;
import com.github.foobar27.myhtml4j.atoms.Tag;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;
import java.util.Stack;

class SinkVisitor<N> implements Visitor {

    private final Sink<N> sink;
    private final Stack<List<N>> childrenStack = new Stack<>();

    SinkVisitor(Sink<N> sink) {
        this.sink = sink;
        childrenStack.push(new ArrayList<>()); // will contain the root node
    }

    public N getParsedRoot() {
        assert (childrenStack.size() == 1);
        assert (childrenStack.peek().size() == 1);
        return childrenStack.peek().get(0);
    }

    private Optional<List<String>> doctype = Optional.empty();

    public Optional<List<String>> getDoctype() {
        return doctype;
    }

    @Override
    public void setDoctype(List<String> args) {
        this.doctype = Optional.of(args);
    }

    @Override
    public void preOrderVisit() {
        childrenStack.push(new ArrayList<>());
    }

    @Override
    public void createText(String text) {
        childrenStack.peek().add(sink.createText(text));
    }

    @Override
    public void createComment(String text) {
        childrenStack.peek().add(sink.createComment(text));
    }

    @Override
    public void createElement(Namespace ns, Tag tag, List<Attribute> attributes) {
        List<N> children = childrenStack.pop();
        N newElement = sink.createElement(ns, tag, attributes, children);
        childrenStack.peek().add(newElement);
    }

}