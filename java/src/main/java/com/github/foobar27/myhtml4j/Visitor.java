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

import java.util.List;

public interface Visitor {

    void setDoctype(List<String> dt);

    void preOrderVisit();

    void createText(String text);

    void createComment(String text);

    void createElement(Namespace ns, Tag tag, List<Attribute> attributes);

}