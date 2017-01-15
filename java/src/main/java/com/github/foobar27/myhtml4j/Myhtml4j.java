package com.github.foobar27.myhtml4j;

import java.util.function.Supplier;

public final class Myhtml4j {

    private static final Myhtml4j INSTANCE = new Myhtml4j();

    public static Myhtml4j getInstance() {
        return INSTANCE;
    }

    private final Native nativeObject;

    private Myhtml4j() {
        this.nativeObject = Native.getInstance();
    }

    /**
     *
     * @param html The input string, UTF-8 encoded.
     * @param callback To be called on each parsing step.
     *
     * @throws InternalError if an internal error occurred while parsing.
     */
    public void parseUTF8(String html, Visitor callback) {
        Native.NativeCallBack nativeCallBack = new Native.NativeCallBack(callback);
        nativeObject.parseUTF8(html, nativeCallBack);
        if (nativeCallBack.internalErrorOccurred) {
            throw new InternalError("An internal error occurred, maybe OutOfMemory?");
        }
    }

    public <N> N parseUTF8(String html, Supplier<Sink<N>> sinkFactory) {
        SinkVisitor<N> sink = new SinkVisitor<>(sinkFactory.get());
        parseUTF8(html, sink);
        return sink.getParsedRoot();
    }

}