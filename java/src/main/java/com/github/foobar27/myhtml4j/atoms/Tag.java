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
package com.github.foobar27.myhtml4j.atoms;

import java.util.Objects;

public final class Tag {
    private final int id;
    private final String name;

    Tag(int id, String name) {
        this.id = id;
        this.name = name;
    }

    public static Tag get(int id, String name) {
        if ((id >= 0) != (name == null)) {
            throw new IllegalArgumentException("Positive ids must not have a name.");
        }
        if (id >= 0) {
            Tag result = Tags.ALL_ATOMS[id];
            if (result == null) {
                throw new IllegalArgumentException("Tag not found: " + id);
            }
            return result;
        } else {
            return new Tag(id, name.toLowerCase());
        }
    }

    public int id() {
        return id;
    }

    public String name() {
        return name;
    }

    @Override
    public String toString() {
        return name;
    }

    @Override
    public int hashCode() {
        return Objects.hash(id, name);
    }

    @Override
    public boolean equals(Object o) {
        if (!(o instanceof Tag)) {
            return false;
        }
        Tag that = (Tag) o;
        if (id >= 0) {
            return this.id == that.id;
        } else {
            return this.name.equals(that.name);
        }
    }

}
