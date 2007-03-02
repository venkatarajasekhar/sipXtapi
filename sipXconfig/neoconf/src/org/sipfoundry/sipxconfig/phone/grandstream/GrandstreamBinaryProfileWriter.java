/*
 * 
 * 
 * Copyright (C) 2006 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2006 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $
 */
package org.sipfoundry.sipxconfig.phone.grandstream;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;

import org.sipfoundry.sipxconfig.phone.Line;

public class GrandstreamBinaryProfileWriter extends GrandstreamProfileWriter {
    private static final int CR = 0x0d;
    private static final int HEXFF = 0xff;
    private static final int OXIOOOO = 0x10000;
    private static final String ET = "&";
    private static final String PARAM_DELIM = "-";
    private int m_lineIndex;
    private boolean m_prefixLineIndex;

    GrandstreamBinaryProfileWriter(GrandstreamPhone phone) {
        super(phone);
    }

    public void write(OutputStream wtr) {
        ByteArrayOutputStream inmemory = new ByteArrayOutputStream();
        setOutputStream(inmemory);
        getPhone().getSettings().acceptVisitor(this);
        for (Line line : getLines()) {
            m_prefixLineIndex = true;
            line.getSettings().acceptVisitor(this);
            m_lineIndex++;
        }
        
        writeBody(inmemory, wtr);
    }
    
    void writeBody(ByteArrayOutputStream inmemory, OutputStream wtr) {
        try {
            finalizeBody(inmemory);
            byte[] body = inmemory.toByteArray();
            writeHeader(wtr, body);
            wtr.write(body);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }        
    }

    void writeLine(String name, String value) {
        String line = getProfileName(name) + '=' + nonNull(value) + ET;
        try {
            getOutputStream().write(line.getBytes());
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    String getProfileName(String pname) {
        if (!m_prefixLineIndex || pname.indexOf(PARAM_DELIM) < 0) {
            return pname;
        }

        String[] ppn = pname.split(PARAM_DELIM);
        return ppn[m_lineIndex];
    }

    void finalizeBody(ByteArrayOutputStream wtr) throws IOException {
        wtr.write("gnkey=0b82".getBytes());
        if (wtr.size() % 2 == 1) {
            wtr.write('\000');
        }
    }

    void writeHeader(OutputStream wtr, byte[] body) throws IOException {
        byte[] gsheader = new byte[] {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, CR, LF, CR,
            LF
        };
        String serial = getPhone().getSerialNumber();
        for (int si = 0; si < 6; si++) {
            gsheader[si + 6] = (byte) Integer.parseInt(serial.substring(si * 2, si * 2 + 2), 16);
        }
        int plen = 8 + body.length / 2;
        gsheader[2] = (byte) ((plen >> 8) & HEXFF);
        gsheader[3] = (byte) (plen & HEXFF);

        int checksum = 0;
        for (int pi = 0; pi < body.length; pi += 2) {
            checksum += (body[pi] & HEXFF) << 8;
            checksum += body[pi + 1] & HEXFF;
        }
        for (int pi = 0; pi < 16; pi += 2) {
            checksum += (gsheader[pi] & HEXFF) << 8;
            checksum += gsheader[pi + 1] & HEXFF;
        }

        checksum = OXIOOOO - (checksum % OXIOOOO);

        gsheader[4] = (byte) ((checksum >> 8) & HEXFF);
        gsheader[5] = (byte) (checksum & HEXFF);

        wtr.write(gsheader);
    }
}