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

import org.springframework.context.ApplicationEvent;
import org.springframework.context.ApplicationListener;

/**
 * Special version of application listener triggered only by the tast with a specified name.
 * InitTaskListener
 */
public abstract class InitTaskListener implements ApplicationListener {
    private String m_taskName;

    /**
     * If task name is not set any task will trigger listener
     */
    public void setTaskName(String taskName) {
        m_taskName = taskName;
    }

    public void onApplicationEvent(ApplicationEvent event) {
        if (event instanceof InitializationTask) {
            InitializationTask task = (InitializationTask) event;
            String taskName = task.getTask();
            if (m_taskName == null || taskName.equals(m_taskName)) {
                onInitTask(taskName);
            }
        }
    }

    public abstract void onInitTask(String task);
}
