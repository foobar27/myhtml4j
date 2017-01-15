package com.github.foobar27.myhtml4j.atoms;

import java.util.Objects;

public final class Namespace {

    private final int id;
    private final String name;

    Namespace(int id, String name) {
        this.id = id;
        this.name = name;
    }

    public static Namespace get(int id, String name) {
        if ((id >= 0) != (name == null)) {
            throw new IllegalArgumentException("Positive ids must not have a name.");
        }
        if (id >= 0) {
            Namespace result = Namespaces.ALL_ATOMS[id];
            if (result == null) {
                throw new IllegalArgumentException("Namespace not found: " + id);
            }
            return result;
        } else {
            return new Namespace(id, name);
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
        return "Namespace[" + id + "," + name + "]";
    }

    @Override
    public int hashCode() {
        return Objects.hash(id, name);
    }

    @Override
    public boolean equals(Object o) {
        if (!(o instanceof Namespace)) {
            return false;
        }
        Namespace that = (Namespace) o;
        if (id >= 0) {
            return this.id == that.id;
        } else {
            return this.name.equals(that.name);
        }
    }
}
