package com.github.foobar27.myhtml4j.atoms;

import java.util.Objects;

public final class AttributeKey {
    private final int id;
    private final String name;

    AttributeKey(int id, String name) {
        this.id = id;
        this.name = name;
    }

    public static AttributeKey get(int id, String name) {
        if ((id >= 0) != (name == null)) {
            throw new IllegalArgumentException("Positive ids must not have a name.");
        }
        if (id >= 0) {
            AttributeKey result = AttributeKeys.ALL_ATOMS[id];
            if (result == null) {
                throw new IllegalArgumentException("AttributeKey not found: " + id);
            }
            return result;
        } else {
            return new AttributeKey(id, name);
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
        return "AttributeKey[" + id + "," + name + "]";
    }

    @Override
    public int hashCode() {
        return Objects.hash(id, name);
    }

    @Override
    public boolean equals(Object o) {
        if (!(o instanceof AttributeKey)) {
            return false;
        }
        AttributeKey that = (AttributeKey) o;
        if (id >= 0) {
            return this.id == that.id;
        } else {
            return this.name.equals(that.name);
        }
    }

}
