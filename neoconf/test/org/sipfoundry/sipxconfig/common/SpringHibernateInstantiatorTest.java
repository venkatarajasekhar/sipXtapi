/*
 * 
 * 
 * Copyright (C) 2005 SIPfoundry Inc.
 * Licensed by SIPfoundry under the LGPL license.
 * 
 * Copyright (C) 2005 Pingtel Corp.
 * Licensed to SIPfoundry under a Contributor Agreement.
 * 
 * $
 */
package org.sipfoundry.sipxconfig.common;

import junit.framework.TestCase;

import org.apache.commons.lang.StringUtils;
import org.sipfoundry.sipxconfig.TestHelper;
import org.sipfoundry.sipxconfig.gateway.Gateway;
import org.sipfoundry.sipxconfig.gateway.audiocodes.MediantGateway;
import org.springframework.beans.factory.ListableBeanFactory;

public class SpringHibernateInstantiatorTest extends TestCase {

    private SpringHibernateInstantiator m_instantiator;

    protected void setUp() throws Exception {
        ListableBeanFactory context = TestHelper.getApplicationContext();
        m_instantiator = new SpringHibernateInstantiator();
        m_instantiator.setBeanFactory(context);
        // to make sure that test are valid
        assertTrue(context.getBeanNamesForType(Gateway.class).length > 1);
    }

    public void testInstantiate() {
        Integer id = new Integer(5);
        BeanWithId bean = (BeanWithId) m_instantiator.instantiate(Gateway.class, id);
        assertSame(Gateway.class, bean.getClass());
        assertSame(id, bean.getId());
    }

    public void testInstantiateSubclass() {
        Integer id = new Integer(5);
        BeanWithId bean = (BeanWithId) m_instantiator.instantiate(MediantGateway.class, id);
        assertSame(MediantGateway.class, bean.getClass());
        assertSame(id, bean.getId());
    }

    public void testInstantiateUnknown() {
        Integer id = new Integer(5);
        // there is a good chance we will not have StringUtils in beanFactory
        Object bean = m_instantiator.instantiate(StringUtils.class, id);
        assertNull(bean);
    }

//    public void testOnSaveAndOnDelete() {
//        Object entity = new Object();
//
//        MockControl listenerControl = MockControl.createStrictControl(DaoEventListener.class);
//        DaoEventListener listener = (DaoEventListener) listenerControl.getMock();
//        listenerControl.expectAndReturn(listener.onSave(entity, null, null, null, null), false);
//        listener.onDelete(entity, null, null, null, null);
//        listenerControl.setDefaultVoidCallable();
//        listenerControl.replay();
//        
//        MockControl bfControl = MockControl.createStrictControl(ListableBeanFactory.class);
//        ListableBeanFactory bf = (ListableBeanFactory) bfControl.getMock();
//        Map beanMap = new HashMap();
//        beanMap.put(listener.getClass().getName(), listener);
//        bfControl.expectAndReturn(bf.getBeansOfType(DaoEventListener.class, true, true), beanMap, 2);
//        bfControl.replay();
//        
//        SpringHibernateInstantiator instantiator = new SpringHibernateInstantiator();
//        instantiator.setBeanFactory(bf);
//        instantiator.onSave(entity, null, null, null, null);
//        instantiator.onDelete(entity, null, null, null, null);
//        
//        listenerControl.verify();
//        bfControl.verify();
//    }
    
}
