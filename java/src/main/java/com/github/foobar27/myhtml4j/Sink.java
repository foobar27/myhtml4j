package com.github.foobar27.myhtml4j;

import com.github.foobar27.myhtml4j.atoms.Namespace;
import com.github.foobar27.myhtml4j.atoms.Tag;

import java.util.List;

public interface Sink<N> {

    N createText(String text);

    N createComment(String text);

    N createElement(Namespace ns, Tag tag, List<Attribute> attributes, List<N> children);

}