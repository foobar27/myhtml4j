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

import java.util.List;
import java.util.Optional;

public class Document<N> {

    private final Optional<List<String>> doctype;
    private final N root;

    public Document(Optional<List<String>> doctype, N root) {
        this.doctype = doctype;
        this.root = root;
    }

    public Optional<List<String>> getDoctype() {
        return doctype;
    }

    public N getRoot() {
        return root;
    }

    @Override
    public String toString() {
        return "Document[" + doctype + "," + root + "]";
    }
}
