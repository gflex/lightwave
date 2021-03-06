/*
 * Copyright © 2018 VMware, Inc.  All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the “License”); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an “AS IS” BASIS, without
 * warranties or conditions of any kind, EITHER EXPRESS OR IMPLIED.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#ifndef __MUTENTCA_POLICY_API_H__
#define __MUTENTCA_POLICY_API_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Available policy types:
 * - LWCA_POLICY_TYPE_CA:
 *     Validate CA policies before creating new intermediate CA
 * - LWCA_POLICY_TYPE_CERTIFICATE:
 *     Validate cert policies before signing a certificate
 */
typedef enum _LWCA_POLICY_TYPE
{
    LWCA_POLICY_TYPE_CA,
    LWCA_POLICY_TYPE_CERTIFICATE
} LWCA_POLICY_TYPE, *PLWCA_POLICY_TYPE;

/*
 * Available policy checks
 * Choose the policies to validate when calling Validate function
 */
typedef enum _LWCA_POLICY_CHECKS
{
    LWCA_POLICY_CHECK_NONE            = 0x0,
    LWCA_POLICY_CHECK_SN              = 0x1,
    LWCA_POLICY_CHECK_SAN             = 0x2,
    LWCA_POLICY_CHECK_KEY_USAGE       = 0x4,
    LWCA_POLICY_CHECK_DURATION        = 0x8,
    LWCA_POLICY_CHECK_ALL             = LWCA_POLICY_CHECK_SN | \
                                        LWCA_POLICY_CHECK_SAN | \
                                        LWCA_POLICY_CHECK_KEY_USAGE | \
                                        LWCA_POLICY_CHECK_DURATION
} LWCA_POLICY_CHECKS, *PLWCA_POLICY_CHECKS;

/*
 * CN/SAN types allowed in policy config file:
 * These will be the "type" values in policy config file.
 * Each allowed type explained below:
 * - "ip"               IP address type
 * - "name"             Name type
 * - "fqdn"             Fully qualified domain type (hostname + DN)
 */
typedef enum _LWCA_POLICY_CFG_TYPE
{
    LWCA_POLICY_CFG_TYPE_IP,
    LWCA_POLICY_CFG_TYPE_NAME,
    LWCA_POLICY_CFG_TYPE_FQDN
} LWCA_POLICY_CFG_TYPE, *PLWCA_POLICY_CFG_TYPE;

/*
 * The matches allowed in policy config file.
 * These will be the "match" values the policy config file.
 *
 * Each allowed match explained below:
 * - "constant":        Checks if CN/SAN matches the given constant value of given type
 *                          type: any
 *                          value: string required
 *
 * - "any":             Allows any CN/SAN of given type
 *                          type: any
 *
 * - "regex":           Checks if CN/SAN matches the given regex of given type
 *                          type: any
 *                          value: valid regex string required
 *
 * - "private":         Checks if CN/SAN is a private IP
 *                          type: ip only
 *
 * - "public":          Checks if CN/SAN is a private IP
 *                          type: ip only
 *
 * - "inzone":          Checks if CN/SAN ip/fqdn is in lw dns records with forward/reverse dns lookup
 *                          type: ip or fqdn
 *
 * - "req.hostname":    Checks if CN/SAN name is requestor's machine account hostname in lw
 *                      Also checks prefix/suffix if they are provided
 *                          type: name only
 *                          prefix: optional
 *                          suffix: optional
 *
 * - "req.fqdn":        Checks if CN/SAN fqdn is requestor's machine account fqdn in lw
 *                      Also checks prefix/sufix if they are provided
 *                          type: fqdn only
 *                          prefix: optional
 *                          suffix: optional
 */
typedef enum _LWCA_POLICY_CFG_MATCH
{
    LWCA_POLICY_CFG_MATCH_CONSTANT,
    LWCA_POLICY_CFG_MATCH_ANY,
    LWCA_POLICY_CFG_MATCH_REGEX,
    LWCA_POLICY_CFG_MATCH_PRIVATE,
    LWCA_POLICY_CFG_MATCH_PUBLIC,
    LWCA_POLICY_CFG_MATCH_INZONE,
    LWCA_POLICY_CFG_MATCH_REQ_HOSTNAME,
    LWCA_POLICY_CFG_MATCH_REQ_FQDN,
} LWCA_POLICY_CFG_MATCH, *PLWCA_POLICY_CFG_MATCH;

/*
 * An object from the policy config file
 * This contains the values corresponding to below json keys
 * - "type":            Required
 * - "match":           Required
 * - "value":           Optional
 * - "prefix":          Optional
 * - "suffix":          Optional
 */
typedef struct _LWCA_POLICY_CFG_OBJ
{
    LWCA_POLICY_CFG_TYPE                type;
    LWCA_POLICY_CFG_MATCH               match;
    PSTR                                pszValue;
    PSTR                                pszPrefix;
    PSTR                                pszSuffix;
    PREGEX                              pRegex;
} LWCA_POLICY_CFG_OBJ, *PLWCA_POLICY_CFG_OBJ;

/*
 * An array of the above mentioned object
 * Example: can be all allowed CN policies or all allowed SAN policies
 */
typedef struct _LWCA_POLICY_CFG_OBJ_ARRAY
{
    PLWCA_POLICY_CFG_OBJ                *ppObj;
    DWORD                               dwCount;
} LWCA_POLICY_CFG_OBJ_ARRAY, *PLWCA_POLICY_CFG_OBJ_ARRAY;

/*
 * All the allowed policies stored after parsing the policy config file
 */
typedef struct _LWCA_POLICIES
{
    BOOLEAN                             bMultiSANEnabled;
    PLWCA_POLICY_CFG_OBJ_ARRAY          pSNs;
    PLWCA_POLICY_CFG_OBJ_ARRAY          pSANs;
    DWORD                               dwKeyUsage;
    DWORD                               dwCertDuration;
} LWCA_POLICIES, *PLWCA_POLICIES;

/*
 * Policy Context containing the policy values to be validated against
 */
typedef struct _LWCA_POLICY_CONTEXT
{
    PLWCA_POLICIES                      pCAPoliciesAllowed;
    PLWCA_POLICIES                      pCertPoliciesAllowed;
} LWCA_POLICY_CONTEXT, *PLWCA_POLICY_CONTEXT;

extern PLWCA_POLICY_CONTEXT gpPolicyCtx;

/*
 * MutentCA Policy requires a json policy config file to initialize. The contents of this
 * json file should be passed to the InitCtx method
 * This function will parse the given config json and store the allowed certificates in
 * the policy context which will be returned
 * This context needs to be passed to Validate function for validating policies and needs
 * to be freed when no longer required
 */
DWORD
LwCAPolicyInitCtx(
    PLWCA_JSON_OBJECT                   pJson,              // IN
    PLWCA_POLICY_CONTEXT                *ppPolicyCtx        // OUT
    );

/*
 * Validate all the specified policy types for the given request
 * This will internally call each policy validate function for the specified policy types
 * For any policy violation, corresponding error code will be returned
 */
DWORD
LwCAPolicyValidate(
    PLWCA_POLICY_CONTEXT                pPolicyCtx,         // IN
    PLWCA_REQ_CONTEXT                   pReqContext,        // IN
    X509_REQ                            *pRequest,          // IN
    PLWCA_CERT_VALIDITY                 pValidity,          // IN (Optional)
    LWCA_POLICY_TYPE                    policyType,         // IN
    LWCA_POLICY_CHECKS                  policyChecks        // IN
    );

/*
 * Get the certificate duration defined for the specified policy type
 * If CertDuration policy is not defined, return duration 0 with error
 */
DWORD
LwCAPolicyGetCertDuration(
    PLWCA_POLICY_CONTEXT                pPolicyCtx,         // IN
    LWCA_POLICY_TYPE                    policyType,         // IN
    DWORD                               *pdwDuration        // OUT
    );

/*
 * Free the policy context when you no longer need it
 */
VOID
LwCAPolicyFreeCtx(
    PLWCA_POLICY_CONTEXT                pPolicyCtx          // IN
    );


/*
 * Util function prototypes for the structures defined above
 * Implemented in policy/util.c
 */

DWORD
LwCAPolicyCfgObjInit(
    LWCA_POLICY_CFG_TYPE            type,
    LWCA_POLICY_CFG_MATCH           match,
    PCSTR                           pcszValue,
    PCSTR                           pcszPrefix,
    PCSTR                           pcszSuffix,
    PLWCA_POLICY_CFG_OBJ            *ppObj
    );

DWORD
LwCAPolicyCfgObjArrayAllocate(
    DWORD                           dwCount,
    PLWCA_POLICY_CFG_OBJ_ARRAY      *ppObjArray
    );

DWORD
LwCAPolicyCfgObjArrayInit(
    PLWCA_POLICY_CFG_OBJ            *ppObj,
    DWORD                           dwCount,
    PLWCA_POLICY_CFG_OBJ_ARRAY      *ppObjArray
    );

DWORD
LwCAPoliciesInit(
    BOOLEAN                         bMultiSANEnabled,
    PLWCA_POLICY_CFG_OBJ_ARRAY      pSNs,
    PLWCA_POLICY_CFG_OBJ_ARRAY      pSANs,
    DWORD                           dwKeyUsage,
    DWORD                           dwCertDuration,
    PLWCA_POLICIES                  *ppPolicies
    );

DWORD
LwCAPolicyCfgObjCopy(
    PLWCA_POLICY_CFG_OBJ            pObjIn,
    PLWCA_POLICY_CFG_OBJ            *ppObjOut
    );

DWORD
LwCAPolicyCfgObjArrayCopy(
    PLWCA_POLICY_CFG_OBJ_ARRAY      pObjArrayIn,
    PLWCA_POLICY_CFG_OBJ_ARRAY      *ppObjArrayOut
    );

VOID
LwCAPolicyCfgObjFree(
    PLWCA_POLICY_CFG_OBJ            pObj
    );

VOID
LwCAPolicyCfgObjArrayFree(
    PLWCA_POLICY_CFG_OBJ_ARRAY      pObjArray
    );

VOID
LwCAPoliciesFree(
    PLWCA_POLICIES                  pPolicies
    );

#ifdef __cplusplus
}
#endif

#endif // __MUTENTCA_POLICY_API_H__
