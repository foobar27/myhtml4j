package com.github.foobar27.myhtml4j;

import com.github.foobar27.myhtml4j.atoms.Namespace;
import com.github.foobar27.myhtml4j.atoms.Tag;

import java.util.List;

public interface Visitor {

    void preOrderVisit();

    void createText(String text);

    void createComment(String text);

    void createElement(Namespace ns, Tag tag, List<Attribute> attributes);

}