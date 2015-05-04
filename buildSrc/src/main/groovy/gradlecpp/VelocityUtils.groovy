package gradlecpp

import org.apache.velocity.Template
import org.apache.velocity.VelocityContext
import org.apache.velocity.app.Velocity
import org.joda.time.format.DateTimeFormat

class VelocityUtils {

    static {
        Properties p = new Properties();

        p.setProperty("resource.loader", "class");
        p.setProperty("class.resource.loader.class", "org.apache.velocity.runtime.resource.loader.FileResourceLoader");
        p.setProperty("class.resource.loader.path", "");

        p.setProperty("input.encoding", "UTF-8");
        p.setProperty("output.encoding", "UTF-8");

        Velocity.init(p);
    }

    static String renderTemplate(File tplFile, Map<String, ? extends Object> ctx) {
        Template tpl = Velocity.getTemplate(tplFile.absolutePath)
        if (!tpl) {
            throw new RuntimeException("Failed to load velocity template ${tplFile.absolutePath}: not found")
        }


        def velocityContext = new VelocityContext(ctx)
        velocityContext.put("_DateTimeFormat", DateTimeFormat)

        def sw = new StringWriter()
        tpl.merge(velocityContext, sw)

        return sw.toString()
    }
}
