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

import com.github.foobar27.nativeinitializer.DefaultNamingScheme;
import com.github.foobar27.nativeinitializer.NativeInitializer;
import com.github.foobar27.nativeinitializer.NativeLoader;
import com.github.foobar27.nativeinitializer.NativeLoaderFactory;
import com.google.common.base.CharMatcher;

import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;
import java.util.function.Supplier;

public final class Myhtml4j {

    private static final String VERSION;

    static {
        try {
            InputStream input = Myhtml4j.class.getClassLoader().getResourceAsStream("myhtml4j-version.properties");
            if (input == null) {
                throw new IllegalStateException("Resource myhtml4j-version.properties not found");
            }
            Properties prop = System.getProperties();
            prop.load(input);
            VERSION = prop.getProperty("myhtml4j-version");
        } catch (IOException e) {
            throw new RuntimeException("Cannot determine myhtml4j version!", e);
        }
    }

    private static NativeLoaderFactory NATIVE_LOADER_FACTORY =
            new NativeLoaderFactory(new DefaultNamingScheme("myhtml4jnative", VERSION));

    private static final NativeInitializer<Myhtml4j> INITIALIZER =
            new NativeInitializer<>(
                    NATIVE_LOADER_FACTORY
                            .resourceLoaderTempDirectory()
                            .fallbackTo(NATIVE_LOADER_FACTORY.systemLoader()),
                    () -> new Myhtml4j(new Native()));

    public static NativeLoaderFactory getNativeLoaderFactory() {
        return NATIVE_LOADER_FACTORY;
    }

    /**
     * Sets the NativeLoader
     */
    public static void setNativeLoader(NativeLoader loader) {
        INITIALIZER.setNativeLoader(loader);
    }

    public static Myhtml4j getInstance() {
        return INITIALIZER.get();
    }

    private final Native nativeObject;

    private Myhtml4j(Native nativeObject) {
        this.nativeObject = nativeObject;
    }

    /**
     * @param html     The input string, UTF-8 encoded.
     * @param callback To be called on each parsing step.
     * @throws InternalError if an internal error occurred while parsing.
     */
    public void parseUTF8(String html, Visitor callback) {
        Native.NativeCallBack nativeCallBack = new Native.NativeCallBack(callback);
        nativeObject.parseUTF8(html, nativeCallBack);
        if (nativeCallBack.internalErrorOccurred) {
            throw new InternalError("An internal error occurred, maybe OutOfMemory?");
        }
    }

    public <N> Document<N> parseUTF8(String html, Supplier<Sink<N>> sinkFactory) {
        SinkVisitor<N> sink = new SinkVisitor<>(sinkFactory.get());
        parseUTF8(html, sink);
        return new Document<>(sink.getDoctype(), sink.getParsedRoot());
    }

    /**
     * Extracts the text representation of an html string.
     */
    public String html2textUTF8(String html) {
        return CharMatcher.whitespace().trimAndCollapseFrom(nativeObject.html2textUTF8(html), ' ');
    }

}
