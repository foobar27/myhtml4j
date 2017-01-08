package com.github.foobar27.myhtml4j;

public class Native {

    private static final String JNI_LIBRARY_NAME = "myhtml4j";

    private static final Native INSTANCE = new Native();

    private final long contextPointer;

    private Native() {
        // guaranteed to be called once (Singleton pattern)
        System.loadLibrary(JNI_LIBRARY_NAME);
    }

    static Native getInstance() {
        return INSTANCE;
    }

    static native void parse(long contextPointer,
                             String inputHtml,
                             long parseOptions,
                             Parser.CallBack callback);

    void parse(String inputHtml,
               ParseOptions parseOptions,
               Parser.CallBack callback) {
        parse(contextPointer, inputHtml, parseOptions.getNativeStruct().pointer, callback);
    }

    long createTokenizerOptions(TokenizerOptions options) {
        return createTokenizerOptions(contextPointer, options);
    }

    long createTreeBuilderOptions(TreeBuilderOptions options) {
        return createTreeBuilderOptions(contextPointer, options);
    }

    long createParseOptions(ParseOptions options) {
        return createParseOptions(contextPointer, options);
    }

    long createSerializeOptions(SerializeOptions options) {
        return createSerializeOptions(contextPointer, options);
    }


}
