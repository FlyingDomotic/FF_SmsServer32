#include <Arduino.h>

String unaccentuate(String originalString, bool toLowercase = false) {
    String replacement = originalString;
    replacement.replace("À", "A");
    replacement.replace("Á", "A");
    replacement.replace("Â", "A");
    replacement.replace("Ã", "A");
    replacement.replace("Ä", "A");
    replacement.replace("Å", "A");
    replacement.replace("Ç", "C");
    replacement.replace("È", "E");
    replacement.replace("É", "E");
    replacement.replace("Ê", "E");
    replacement.replace("Ë", "E");
    replacement.replace("Ì", "I");
    replacement.replace("Í", "I");
    replacement.replace("Î", "I");
    replacement.replace("Ï", "I");
    replacement.replace("Ñ", "N");
    replacement.replace("Ò", "O");
    replacement.replace("Ó", "O");
    replacement.replace("Ô", "O");
    replacement.replace("Õ", "O");
    replacement.replace("Ö", "O");
    replacement.replace("Ø", "O");
    replacement.replace("Ù", "U");
    replacement.replace("Ú", "U");
    replacement.replace("Û", "U");
    replacement.replace("Ü", "U");
    replacement.replace("à", "a");
    replacement.replace("á", "a");
    replacement.replace("â", "a");
    replacement.replace("ã", "a");
    replacement.replace("ä", "a");
    replacement.replace("å", "a");
    replacement.replace("ç", "c");
    replacement.replace("è", "e");
    replacement.replace("é", "e");
    replacement.replace("ê", "e");
    replacement.replace("ë", "e");
    replacement.replace("ì", "i");
    replacement.replace("í", "i");
    replacement.replace("î", "i");
    replacement.replace("ï", "i");
    replacement.replace("ð", "o");
    replacement.replace("ñ", "n");
    replacement.replace("ò", "o");
    replacement.replace("ó", "o");
    replacement.replace("ô", "o");
    replacement.replace("õ", "o");
    replacement.replace("ö", "o");
    replacement.replace("ø", "o");
    replacement.replace("ù", "u");
    replacement.replace("ú", "u");
    replacement.replace("û", "u");
    replacement.replace("ü", "u");
    replacement.replace("ý", "y");
    replacement.replace("ÿ", "y");
    if (toLowercase) {
        replacement.toLowerCase();
    }
    return replacement;
}
