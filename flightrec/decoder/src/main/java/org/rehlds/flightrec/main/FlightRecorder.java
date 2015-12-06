package org.rehlds.flightrec.main;

import org.rehlds.flightrec.api.DecoderModule;
import org.rehlds.flightrec.api.FlightrecMessage;
import org.rehlds.flightrec.decoders.rehlds.RehldsDecodersModule;
import org.rehlds.flightrec.filescan.FileScanResult;
import org.rehlds.flightrec.filescan.FlightRecFileScanner;
import org.rehlds.flightrec.filescan.HeaderScanResult;
import org.rehlds.flightrec.logtree.FlightLogTreeBuilder;
import org.rehlds.flightrec.logtree.LogTreeNodeComplex;
import org.rehlds.flightrec.logparser.FlightLogParser;
import org.rehlds.flightrec.textlogwriter.TextLogWriter;
import org.rehlds.flightrec.util.JarUtils;

import java.io.File;
import java.io.FileFilter;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.*;

public class FlightRecorder {
    RunConfig cfg;
    List<DecoderModule> decoderModules = new ArrayList<>();

    public FlightRecorder(RunConfig cfg) {
        this.cfg = cfg;
    }

    private boolean checkConfig() {
        if (cfg.dumpFile == null) {
            System.out.println("Dump file is not selected, please use --dump-file <filename> parameter to specify it");
            return false;
        }

        if (cfg.outFile == null) {
            cfg.outFile = new File(cfg.dumpFile.getAbsolutePath() + ".flog");
        }

        return true;
    }

    private List<FlightrecMessage> scanFile(RandomAccessFile f) throws IOException {
        FileScanResult scanResult = FlightRecFileScanner.scan(f);

        System.out.println("Dump file scan results: ");
        for (HeaderScanResult hdr : scanResult.metaHeaders) {
            System.out.print(String.format("\tMeta header @ 0x%08X; len=%d; version=%d; valid=%s", hdr.pos, hdr.len, hdr.version, "" + (hdr.error == null)));
            if (hdr.error != null) {
                System.out.print("; error: " + hdr.error);
            }
            System.out.println();
        }
        for (HeaderScanResult hdr : scanResult.dataHeaders) {
            System.out.print(String.format("\tData header @ 0x%08X; len=%d; version=%d; valid=%s", hdr.pos, hdr.len, hdr.version, "" + (hdr.error == null)));
            if (hdr.error != null) {
                System.out.print("; error: " + hdr.error);
            }
            System.out.println();
        }

        HeaderScanResult validMetaHeader = null;
        HeaderScanResult validDataHeader = null;

        for (HeaderScanResult metaHeader : scanResult.metaHeaders) {
            if (metaHeader.error != null) {
                continue;
            }

            if (validMetaHeader != null) {
                System.out.println("Multiple meta headers found, exiting");
                return null;
            }

            validMetaHeader = metaHeader;
        }

        for (HeaderScanResult dataHeader : scanResult.dataHeaders) {
            if (dataHeader.error != null) {
                continue;
            }

            if (validDataHeader != null) {
                System.out.println("Multiple data headers found, exiting");
                return null;
            }

            validDataHeader = dataHeader;
        }

        if (validMetaHeader == null) {
            System.out.println("Meta header not found, exiting");
            return null;
        }

        if (validDataHeader == null) {
            System.out.println("Data header not found, exiting");
            return null;
        }

        return FlightLogParser.doParse(f, validMetaHeader, validDataHeader);
    }

    private LogTreeNodeComplex buildTree(List<FlightrecMessage> messages) {
        return FlightLogTreeBuilder.buildTree(messages);
    }

    private boolean writeOutputFile(LogTreeNodeComplex logTreeRoot) {
        TextLogWriter.decodeAndWrite(logTreeRoot, cfg.outFile, decoderModules);
        System.out.println("Written decoded log to '" + cfg.outFile.getAbsolutePath() + ";");
        return true;
    }

    public boolean run() {
        registerBuiltinDecoders();
        loadExternalDecoders();

        if (!checkConfig()) {
            return false;
        }

        List<FlightrecMessage> messages;
        try(RandomAccessFile f = new RandomAccessFile(cfg.dumpFile, "r")) {
            messages = scanFile(f);
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }

        if (messages == null) {
            return false;
        }

        System.out.println("Read " + messages.size() + " messages from '" + cfg.dumpFile.getAbsolutePath() + "'");
        LogTreeNodeComplex treeRootNode = buildTree(messages);
        if (treeRootNode == null) {
            return false;
        }

        if (!writeOutputFile(treeRootNode)) {
            return false;
        }

        return true;
    }

    private void loadExternalDecoders() {
        File f = JarUtils.getJarFileOfClass(FlightRecorder.class);
        if (f == null) {
            System.out.println("Could not locate main JAR, external decoders will not be loaded");
            return;
        }

        File extDir = new File(f.getParentFile(), "extDecoders");
        if (!extDir.exists() || !extDir.isDirectory()) {
            System.out.println("Directory '" + extDir.getAbsolutePath() + "' doesn't exist");
        }

        File[] jarFiles = extDir.listFiles(new FileFilter() {
            @Override
            public boolean accept(File pathname) {
                return pathname.getName().toLowerCase().endsWith(".jar");
            }
        });

        ArrayList<URL> jarUrls = new ArrayList<>();
        for (File jf : jarFiles) {
            try {
                jarUrls.add(jf.toURI().toURL());
            } catch (MalformedURLException e) {
                throw new RuntimeException(e);
            }
        }

        URLClassLoader extDecodersClassloader = new URLClassLoader(jarUrls.toArray(new URL[jarUrls.size()]), this.getClass().getClassLoader());
        ServiceLoader<DecoderModule> srvLoader = ServiceLoader.load(DecoderModule.class, extDecodersClassloader);
        for (DecoderModule decoderModule : srvLoader) {
            System.out.println("Loaded external decoder module " + decoderModule.getDescription() + " version " + decoderModule.getVersion());
            decoderModules.add(decoderModule);
        }
    }

    private void registerBuiltinDecoders() {
        decoderModules.add(new RehldsDecodersModule());
    }

    public static void main(String args[]) {
        RunConfig cfg;
        try {
            cfg = parseArgs(args);
        } catch (IllegalArgumentException e) {
            System.out.println(e.getMessage());
            return;
        }

        new FlightRecorder(cfg).run();
    }

    private static RunConfig parseArgs(String args[])  {
        RunConfig cfg = new RunConfig();
        for (int i = 0; i < args.length; i++) {
            String a = args[i];

            if ("--dump-file".equals(a)) {
                if (i + 1 >= args.length) {
                    throw new IllegalArgumentException("--dump-file should be followed by file name");
                }
                i++;
                cfg.dumpFile = new File(args[i]);
                continue;
            }

            if ("--out-file".equals(a)) {
                if (i + 1 >= args.length) {
                    throw new IllegalArgumentException("--out-file should be followed by file name");
                }
                i++;
                cfg.outFile = new File(args[i]);
                continue;
            }

            throw new IllegalArgumentException("Invalid command line parameter: '" + a + "'");
        }

        return cfg;
    }

}
