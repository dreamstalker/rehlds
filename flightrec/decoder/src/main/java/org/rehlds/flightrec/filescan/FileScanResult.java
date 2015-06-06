package org.rehlds.flightrec.filescan;

import java.util.ArrayList;
import java.util.List;

public class FileScanResult {
    public List<HeaderScanResult> metaHeaders = new ArrayList<>();
    public List<HeaderScanResult> dataHeaders = new ArrayList<>();
}
