package org.rehlds.flightrec.util;

import java.io.File;
import java.net.URL;

public class JarUtils {
    public static File getJarFileOfClass(Class c) {
        String classFileName = c.getName().replace('.', '/') + ".class";
        ClassLoader classLoader = c.getClassLoader();
        if (classLoader == null) {
            classLoader = JarUtils.class.getClassLoader();
        }
        URL url = classLoader.getResource(classFileName);
        if (url == null) {
            return null;
        }

        String strUrl = url.toString();
        if (!strUrl.startsWith("jar:file:/")) {
            return null;
        }

        int jarSeparator = strUrl.indexOf('!');
        if (jarSeparator == -1) {
            return null;
        }

        String jarFilePath = strUrl.substring("jar:file:/".length(), jarSeparator);
        return new File(jarFilePath);
    }
}
