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
package org.sipfoundry.sipxconfig.site.phone;

import java.util.Iterator;
import java.util.List;

import org.apache.commons.lang.ArrayUtils;
import org.apache.tapestry.contrib.table.model.IBasicTableModel;
import org.apache.tapestry.contrib.table.model.ITableColumn;
import org.sipfoundry.sipxconfig.phone.PhoneContext;

/**
 * Geared towards paging-based data models for tapestry TableViews that only load one page at a
 * time and do server-side sorting.
 */
public class PhoneTableModel implements IBasicTableModel {
    private PhoneContext m_phoneContext;
    private Integer m_groupId;

    public PhoneTableModel(PhoneContext phoneContext, Integer groupId) {
        m_phoneContext = phoneContext;
        m_groupId = groupId;
    }

    public PhoneTableModel() {
        // intentionally empty
    }

    public void setGroupId(Integer groupId) {
        m_groupId = groupId;
    }

    public void setPhoneContext(PhoneContext context) {
        m_phoneContext = context;
    }

    public int getRowCount() {
        return m_phoneContext.getPhonesInGroupCount(m_groupId);
    }

    public Iterator getCurrentPageRows(int firstRow, int pageSize, ITableColumn objSortColumn,
            boolean orderAscending) {
        String[] orderBy = orderByFromSortColum(objSortColumn);
        List page = m_phoneContext.loadPhonesByPage(m_groupId, firstRow, pageSize, orderBy,
                orderAscending);
        return page.iterator();
    }

    /**
     * Translates table column to array of phone properties. It is safe to call with null - emtpy
     * array is returned in such case
     * 
     * HACK: this is dangerously dependend on relation between the table column name and the
     * properties names
     * 
     * @param objSortColumn column object
     * @return array of strings by which we need to sort the table
     */
    public static String[] orderByFromSortColum(ITableColumn objSortColumn) {
        if (objSortColumn == null) {
            return ArrayUtils.EMPTY_STRING_ARRAY;
        }

        String[] orderBy = new String[] {
            objSortColumn.getColumnName()
        };

        // fix for modelId case
        if ("modelId".equals(orderBy[0])) {
            return new String[] {
                "beanId", orderBy[0]
            };
        }

        return orderBy;
    }
}
