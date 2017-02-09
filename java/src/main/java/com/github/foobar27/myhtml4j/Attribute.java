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

import com.github.foobar27.myhtml4j.atoms.AttributeKey;
import com.github.foobar27.myhtml4j.atoms.Namespace;

public final class Attribute {
    private final Namespace ns;
    private final AttributeKey key;
    private final String value;

    public Attribute(Namespace ns, AttributeKey key, String value) {
        this.ns = ns;
        this.key = key;
        this.value = value;
    }

    public Namespace getNamespace() {
        return ns;
    }

    public AttributeKey getKey() {
        return key;
    }

    public String getValue() {
        return value;
    }
}
