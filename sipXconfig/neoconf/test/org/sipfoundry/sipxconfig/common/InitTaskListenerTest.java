/*
 * 
 * 
 * Copyright (C) 2007 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2007 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $
 */
package org.sipfoundry.sipxconfig.common;

import junit.framework.TestCase;

public class InitTaskListenerTest extends TestCase {
    
    public void testEventType() {
        InitTaskListenerDummy itl = new InitTaskListenerDummy();
        itl.setTaskName("bongo");
        ApplicationInitializedEvent event = new ApplicationInitializedEvent(this);
        itl.onApplicationEvent(event);
        assertFalse(itl.isTriggered());
    }

    public void testEmptyTaskName() {
        InitTaskListenerDummy itl = new InitTaskListenerDummy();
        InitializationTask event = new InitializationTask("bongo");
        itl.onApplicationEvent(event);
        assertTrue(itl.isTriggered());
    }
    
    public void testTaskName() {
        InitTaskListenerDummy itl = new InitTaskListenerDummy();
        itl.setTaskName("kuku");
        InitializationTask event = new InitializationTask("bongo");
        itl.onApplicationEvent(event);
        assertFalse(itl.isTriggered());
    }

    public void testOnInitTask() {
        InitTaskListenerDummy itl = new InitTaskListenerDummy();
        itl.setTaskName("bongo");
        InitializationTask event = new InitializationTask("bongo");
        itl.onApplicationEvent(event);
        assertTrue(itl.isTriggered());
    }

    class InitTaskListenerDummy extends InitTaskListener {
        private boolean triggered;

        public boolean isTriggered() {
            return triggered;
        }

        public void onInitTask(String task) {
            triggered = true;
        }
    }
}
