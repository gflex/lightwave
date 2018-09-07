/*
 *  Copyright (c) 2012-2018 VMware, Inc.  All Rights Reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not
 *  use this file except in compliance with the License.  You may obtain a copy
 *  of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, without
 *  warranties or conditions of any kind, EITHER EXPRESS OR IMPLIED.  See the
 *  License for the specific language governing permissions and limitations
 *  under the License.
 */
package com.vmware.identity.token.impl;

import javax.xml.XMLConstants;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.vmware.vim.sso.client.XmlParserFactory;

/**
 * XML Parser factory that:
 * <ul>
 * <li>disables validation
 * <li>ignores comments and whitespace
 * <li>Supports namespaces
 * <li>disables xInclude syntax parsing
 * <li>doesn't load external DTDs
 * <li>resolves all external XML entities to empty string
 * <li>supports standard XML entities and local "string-substition" XML entities
 * </ul>
 */
public class SecureXmlParserFactory implements XmlParserFactory {

    private final Logger log = LoggerFactory
            .getLogger(SecureXmlParserFactory.class);

    private static final String SAX_VALIDATION = "http://xml.org/sax/features/validation";
    private static final String LOAD_DTD_GRAMMAR = "http://apache.org/xml/features/nonvalidating/load-dtd-grammar";
    private static final String LOAD_EXTERNAL_DTD = "http://apache.org/xml/features/nonvalidating/load-external-dtd";
    private static final String DISALLOW_DOCTYPE_DECL = "http://apache.org/xml/features/disallow-doctype-decl";
    private static final String EXTERNAL_GENERAL_ENTITIES = "http://xml.org/sax/features/external-general-entities";
    private static final String EXTERNAL_PARAMETER_ENTITIES = "http://xml.org/sax/features/external-parameter-entities";


    @Override
    public DocumentBuilder newDocumentBuilder()
            throws ParserConfigurationException {
        DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
        /*
         * IMPORTANT: We disable validation because it might bring synthetic
         * ("non-specified", i.e. generated from the Schema's default values)
         * attributes into the DOM tree; they'll become "specified" after copying
         * and will most probably break the signature.
         */
        dbf.setValidating(false);
        dbf.setIgnoringComments(true);
        dbf.setNamespaceAware(true);
        dbf.setXIncludeAware(false);
        dbf.setExpandEntityReferences(false);

        /*
         * Optional features, recommended by security team. The feature handling
         * depends on what XML parser is on the classpath. Successfully setting
         * the features is not as essential, as dbf.validating = false and the
         * custom entity resolver.
         */
        trySetFeature(dbf, SAX_VALIDATION, false);
        trySetFeature(dbf, XMLConstants.FEATURE_SECURE_PROCESSING, true);
        trySetFeature(dbf, EXTERNAL_GENERAL_ENTITIES, false);
        trySetFeature(dbf, EXTERNAL_PARAMETER_ENTITIES, false);

        // configuration for Xerces XML parser; has not effect if using JDK parser
        trySetFeature(dbf, LOAD_DTD_GRAMMAR, false);
        trySetFeature(dbf, LOAD_EXTERNAL_DTD, false);
        trySetFeature(dbf, DISALLOW_DOCTYPE_DECL, true);

        return dbf.newDocumentBuilder();
    }

    private void trySetFeature(DocumentBuilderFactory dbf, String featureKey,
                               boolean value) {
        try {
            dbf.setFeature(featureKey, value);
        } catch (ParserConfigurationException e) {
            // Note that this may happen on every token parse.
            if (log.isDebugEnabled()) {
                log.debug("Couldn't apply feature " + featureKey +
                        " to DocumentBuilderFactory " + dbf.getClass().getName() +
                        " Can be safely ignored.");
            }
        }
    }
}