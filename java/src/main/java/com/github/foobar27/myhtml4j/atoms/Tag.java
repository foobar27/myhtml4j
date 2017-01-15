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
            return new Tag(id, name);
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
