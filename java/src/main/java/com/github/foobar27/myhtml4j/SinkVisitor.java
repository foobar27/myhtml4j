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