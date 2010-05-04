/*
* Copyright (C) 2009 Mamadou Diop.
*
* Contact: Mamadou Diop <diopmamadou(at)doubango.org>
*	
* This file is part of Open Source Doubango Framework.
*
* DOUBANGO is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*	
* DOUBANGO is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*	
* You should have received a copy of the GNU General Public License
* along with DOUBANGO.
*
*/

/**@file tsip.h
 * @brief SIP (RFC 3261) and 3GPP IMS/LTE (TS 24.229) implementation.
 *
 * @author Mamadou Diop <diopmamadou(at)doubango.org>
 *
 * @date Created: Sat Nov 8 16:54:58 2009 mdiop
 */
#ifndef TINYSIP_TSIP_H
#define TINYSIP_TSIP_H

#include "tinySIP_config.h"

#include "tinySIP/tsip_ssession.h"
#include "tinySIP/tsip_timers.h"
#include "tinySIP/tsip_event.h"

#include "tinySIP/tsip_uri.h"

#include "tnet_socket.h"
#include "dns/tnet_dns.h"

#include "tsk_runnable.h"

TSIP_BEGIN_DECLS

#define TSIP_IARI_GSMAIS				"urn%3Aurn-7%3A3gpp-application.ims.iari.gsma-is"
#define TSIP_IARI_QUOTED_GSMAIS			"\""TSIP_IARI_GSMAIS"\""
#define TSIP_ICSI_MMTEL_PSVOICE			"urn%3Aurn-7%3A3gpp-service.ims.icsi.mmtel"
#define TSIP_ICSI_QUOTED_MMTEL_PSVOICE	"\""TSIP_ICSI_MMTEL_PSVOICE"\""

#define TSIP_STACK(self)		((tsip_stack_t*)(self))

typedef uint8_t amf_t[2];
typedef uint8_t operator_id_t[16];


typedef enum tsip_stack_param_type_e
{
	pname_null = tsk_null,
	
	/* === Identity === */
	pname_display_name,
	pname_impu,
	pname_preferred_id,
	pname_impi,
	pname_password,

	/* === Network === */
	pname_realm,
	pname_local_ip,
	pname_local_port,
	pname_discovery_naptr,
	pname_discovery_dhcp,
	pname_proxy_cscf,

	
	/* === Security === */
	pname_early_ims,
	pname_secagree_ipsec,
	pname_secagree_tls,
	pname_amf,
	pname_operator_id,
	pname_tls_certs,
	pname_ipsec_params,

	/* === Dummy Headers === */
	pname_header,

}
tsip_stack_param_type_t;

/**@ingroup tsip_stack_group
* @def TSIP_STACK_SET_NULL
* Ends the stack parameters. Mandatory and should be the last one.
*/
#define TSIP_STACK_SET_NULL()							pname_null

/* === Identity === */
/**@ingroup tsip_stack_group
* @def TSIP_STACK_SET_DISPLAY_NAME
* Sets the user's display name. Used in SIP 'From' and 'To' headers.
* @param NAME_STR The user's display name.
* @code
* int ret = tsip_stack_set(stack, 
*              TSIP_STACK_SET_DISPLAY_NAME("john doe"),
*              TSIP_STACK_SET_NULL());
* @endcode
*/
/**@ingroup tsip_stack_group
* @def TSIP_STACK_SET_IMPU
* Sets the user's IMPU (IMS Public Identity).
* @param URI_STR A valid SIP/SIPS/tel URI string.
* @code
* int ret = tsip_stack_set(stack, 
*              TSIP_STACK_SET_IMPU("sip:johndoe@open-ims.test"),
*              TSIP_STACK_SET_NULL());
* @endcode
*/
/**@ingroup tsip_stack_group
* @def TSIP_STACK_SET_PREFERRED_IDENTITY
* Sets the user's Preferred Identity. Used to populate the P-Preferred-Identity header.
* @param URI_STR A valid SIP/SIPS/tel URI string.
* @code
* int ret = tsip_stack_set(stack, 
*              TSIP_STACK_SET_PREFERRED_IDENTITY("sip:johndoe@open-ims.test"),
*              TSIP_STACK_SET_NULL());
* @endcode
*/
/**@ingroup tsip_stack_group
* @def TSIP_STACK_SET_IMPI
* Sets the user's IMPI (IMS Private Identity).
* @param IMPI_STR Any string.
* @code
* int ret = tsip_stack_set(stack, 
*              TSIP_STACK_SET_IMPI("johndoe@open-ims.test"),
*              TSIP_STACK_SET_NULL());
* @endcode
*/
/**@ingroup tsip_stack_group
* @def TSIP_STACK_SET_PASSWORD
* Sets the user's password.
* @param PASSORD_STR Any string.
* @code
* int ret = tsip_stack_set(stack, 
*              TSIP_STACK_SET_PASSWORD("mysecret"),
*              TSIP_STACK_SET_NULL());
* @endcode
*/
#define TSIP_STACK_SET_DISPLAY_NAME(NAME_STR)			pname_display_name, (const char*)NAME_STR
#define TSIP_STACK_SET_IMPU(URI_STR)					pname_impu, (const char*)URI_STR
#define TSIP_STACK_SET_PREFERRED_IDENTITY(URI_STR)		pname_preferred_id, (const char*)URI_STR
#define TSIP_STACK_SET_IMPI(IMPI_STR)					pname_impi, (const char*)IMPI_STR
#define TSIP_STACK_SET_PASSWORD(PASSORD_STR)			pname_password, (const char*)PASSORD_STR

/* === Network === */
/**@ingroup tsip_stack_group
* @def TSIP_STACK_SET_REALM
* Sets the domain name.
* @param URI_STR A valid SIP/SIPS URI string.
* @code
* int ret = tsip_stack_set(stack, 
*              TSIP_STACK_SET_REALM("sip:open-ims.test"),
*              TSIP_STACK_SET_NULL());
* @endcode
*/
/**@ingroup tsip_stack_group
* @def TSIP_STACK_SET_LOCAL_IP
* Sets the user's local IP address.
* @param IP_STR IPv4/IPv6 IP address or FQDN.
* @code
* int ret = tsip_stack_set(stack, 
*              TSIP_STACK_SET_LOCAL_IP("fe80::"),
*              TSIP_STACK_SET_NULL());
* @endcode
*
* @sa @ref  TSIP_STACK_SET_LOCAL_PORT()
*/
/**@ingroup tsip_stack_group
* @def TSIP_STACK_SET_LOCAL_PORT
* Sets the user's local Port to bind to.
* @param PORT_UINT Port number. Should be between 0x0400 and 0xFFFF.
* @code
* int ret = tsip_stack_set(stack, 
*              TSIP_STACK_SET_LOCAL_PORT(5060),
*              TSIP_STACK_SET_NULL());
* @endcode
*
* @sa @ref TSIP_STACK_SET_LOCAL_IP()
*/
/**@ingroup tsip_stack_group
* @def TSIP_STACK_SET_DISCOVERY_NAPTR
* Whether to use DNS NAPTR for Proxy-CSCF discovery. Default is false. However, if the Proxy-CSCF IP address
* is missing, then this option will be enabled by default.
* @param ENABLED_BOOL @a tsk_true (1) or @a tsk_false (0).
* @code
* int ret = tsip_stack_set(stack, 
*              TSIP_STACK_SET_DISCOVERY_NAPTR(tsk_true),
*              TSIP_STACK_SET_NULL());
* @endcode
*
* @sa @ref TSIP_STACK_SET_DISCOVERY_DHCP()
*/
/**@ingroup tsip_stack_group
* @def TSIP_STACK_SET_DISCOVERY_DHCP
* Whether to use DHCPv4/v6 for Proxy-CSCF discovery. Default is false. The IP version (v4/v6) of the DHCP protocol
* to use will depend on the Proxy-CSCF address type.
* @param ENABLED_BOOL @a tsk_true (1) or @a tsk_false (0).
* @code
* int ret = tsip_stack_set(stack, 
*              TSIP_STACK_SET_DISCOVERY_DHCP(tsk_false),
*              TSIP_STACK_SET_NULL());
* @endcode
*
* @sa @ref TSIP_STACK_SET_DISCOVERY_NAPTR()
*/
/**@ingroup tsip_stack_group
* @def TSIP_STACK_SET_PROXY_CSCF
* Configures the Proxy-CSCF. 
* @param FQDN_STR The IP address (v4/v6) or FQDN of the Proxy-CSCF.
* @param PORT_UINT The port.
* @param TRANSPORT_STR The transport type. Should be: @a "udp" or @a "tcp" or @a "tls" or @a "sctp". Default is @a "udp".
* @param IP_VERSION_STR The IP version to use. Should be:  @a "ipv4" or @a "ipv6" or @a "ipv46". This parameter is useful when 
* @a FQDN_STR parameter is a domain name. default is @a "ipv4".
* @code
* int ret = tsip_stack_set(stack, 
*              TSIP_STACK_SET_PROXY_CSCF("pcscf-doubango.org", 4060, "tcp", "ipv6"),
*              TSIP_STACK_SET_NULL());
* @endcode
*
* @sa @ref TSIP_STACK_SET_DISCOVERY_DHCP()<br>@ref TSIP_STACK_SET_DISCOVERY_NAPTR()
*/
#define TSIP_STACK_SET_REALM(URI_STR)					pname_realm, (const char*)URI_STR
#define TSIP_STACK_SET_LOCAL_IP(IP_STR)					pname_local_ip, (const char*)IP_STR
#define TSIP_STACK_SET_LOCAL_PORT(PORT_UINT)			pname_local_port, (unsigned)PORT_UINT
#define TSIP_STACK_SET_DISCOVERY_NAPTR(ENABLED_BOOL)	pname_discovery_naptr, (tsk_bool_t)ENABLED_BOOL
#define TSIP_STACK_SET_DISCOVERY_DHCP(ENABLED_BOOL)		pname_discovery_dhcp, (tsk_bool_t)ENABLED_BOOL
#define TSIP_STACK_SET_PROXY_CSCF(FQDN_STR, PORT_UINT, TRANSPORT_STR, IP_VERSION_STR)			pname_proxy_cscf, (const char*)FQDN_STR, (unsigned)PORT_UINT, (const char*)TRANSPORT_STR, (const char*)IP_VERSION_STR

/* === Security === */
/**@ingroup tsip_stack_group
* @def TSIP_STACK_SET_EARLY_IMS
* Whether to enable 3GPP Early-IMS Security as per 3GPP TS 33.978. 
* @param ENABLED_BOOL @a tsk_true (1) or @a tsk_false (0).
* @code
* int ret = tsip_stack_set(stack, 
*              TSIP_STACK_SET_EARLY_IMS(tsk_false),
*              TSIP_STACK_SET_NULL());
* @endcode
*/
/**@ingroup tsip_stack_group
* @def TSIP_STACK_SET_SECAGREE_IPSEC
* Whether to enable IPSec security agreement as per IETF RFC 3329. 
* @param ENABLED_BOOL @a tsk_true (1) or @a tsk_false (0).
* @code
* int ret = tsip_stack_set(stack, 
*              TSIP_STACK_SET_SECAGREE_IPSEC(tsk_true),
*              TSIP_STACK_SET_NULL());
* @endcode
*
* @sa @ref TSIP_STACK_SET_IPSEC_PARAMS()
*/
/**@ingroup tsip_stack_group
* @def TSIP_STACK_SET_SECAGREE_TLS
* Whether to enable TLS security agreement as per IETF RFC 3329. 
* @param ENABLED_BOOL @a tsk_true (1) or @a tsk_false (0).
* @code
* int ret = tsip_stack_set(stack, 
*              TSIP_STACK_SET_SECAGREE_TLS(tsk_true),
*              TSIP_STACK_SET_NULL());
* @endcode
*
* @sa @ref TSIP_STACK_SET_IPSEC_PARAMS()
*/
/**@ingroup tsip_stack_group
* @def TSIP_STACK_SET_IMS_AKA_AMF
* Sets AMF (Authentication Management Field) value to use for IMS-AKA authentication.
* @param AMF_UINT16 16-bit value.
* @code
* int ret = tsip_stack_set(stack, 
*              TSIP_STACK_SET_IMS_AKA_AMF(0x3FB6),
*              TSIP_STACK_SET_NULL());
* @endcode
*
* @sa @ref TSIP_STACK_SET_IMS_AKA_OPERATOR_ID()
*/
/**@ingroup tsip_stack_group
* @def TSIP_STACK_SET_IMS_AKA_OPERATOR_ID
* Sets the operator identifier value to use for IMS-AKA authentication.
* @param OPID_HEX_STR 128-bit value as hex-string. If the length of the value supplied by the user is less than 128 bits,
* then it will be padded with zeros.
* @code
* int ret = tsip_stack_set(stack, 
*              TSIP_STACK_SET_IMS_AKA_OPERATOR_ID("0x5FABC9"), // Will be padded with zeros
*              TSIP_STACK_SET_NULL());
* @endcode
*
* @sa @ref TSIP_STACK_SET_IMS_AKA_AMF()
*/
/**@ingroup tsip_stack_group
* @def TSIP_STACK_SET_IPSEC_PARAMS
* Sets the IPSec parameters. To enable IPSec security agreement you should use @ref TSIP_STACK_SET_SECAGREE_IPSEC(tsk_true).
* @param ALG_STR Algorithm. Should be: @a "hmac-sha-1-96" or @a "hmac-md5-96". Default is @a "hmac-md5-96".
* @param EALG_STR Encryption Algorithm. Should be: @a "des-ede3-cbc" or @a "aes" or @a "null". Default is @a "null".
* @param MODE_STR Mode. Should be: @a "tun" or @a "trans". Default is @a "trans".
* @param PROTOCOL_STR Protocol. Should be @a "ah" or @a "esp" or @a "ah/esp". Default is @a "esp".
* @code
* int ret = tsip_stack_set(stack, 
*              TSIP_STACK_SET_SECAGREE_IPSEC(tsk_true),
               TSIP_STACK_SET_IPSEC_PARAMS("hmac-md5-96", "des-ede3-cbc", "trans", "esp"),
*              TSIP_STACK_SET_NULL());
* @endcode
*
* @sa @ref TSIP_STACK_SET_TLS_CERTS()
*/
/**@ingroup tsip_stack_group
* @def TSIP_STACK_SET_TLS_CERTS
* Sets TLS certificate files. To enable TLS security agreement you should use @ref TSIP_STACK_SET_SECAGREE_TLS(tsk_true).
* @param CA_FILE_STR CA file.
* @param PUB_FILE_STR Public cert. file.
* @param PRIV_FILE_STR Private cert. file.
* @code
* int ret = tsip_stack_set(stack, 
*              TSIP_STACK_SET_TLS_CERTS("C:\\tls\\ca.pki-crt.pem", "C:\\tls\\pub-crt.pem", "C:\\tls\\priv-key.pem"),
*              TSIP_STACK_SET_NULL());
* @endcode
*
* @sa @ref TSIP_STACK_SET_IPSEC_PARAMS()
*/
#define TSIP_STACK_SET_EARLY_IMS(ENABLED_BOOL)				pname_early_ims, (tsk_bool_t)ENABLED_BOOL
#define TSIP_STACK_SET_SECAGREE_IPSEC(ENABLED_BOOL)			pname_scagree_ipsec, (tsk_bool_t)ENABLED_BOOL
#define TSIP_STACK_SET_SECAGREE_TLS(ENABLED_BOOL)			pname_scagree_tls, (tsk_bool_t)ENABLED_BOOL
#define TSIP_STACK_SET_IMS_AKA_AMF(AMF_UINT16)				pname_amf, (uint16_t)AMF_UINT16
#define TSIP_STACK_SET_IMS_AKA_OPERATOR_ID(OPID_HEX_STR)	pname_operator_id, (const char*)OPID_HEX_STR
#define TSIP_STACK_SET_IPSEC_PARAMS(ALG_STR, EALG_STR, MODE_STR, PROTOCOL_STR)	pname_ipsec_params, (const char*)ALG_STR, (const char*)EALG_STR, (const char*)MODE_STR, (const char*)PROTOCOL_STR
#define TSIP_STACK_SET_TLS_CERTS(CA_FILE_STR, PUB_FILE_STR, PRIV_FILE_STR)			pname_tls_certs, (const char*)CA_FILE_STR, (const char*)PUB_FILE_STR, (const char*)PRIV_FILE_STR


/* === Headers === */
/**@ingroup tsip_stack_group
* @def TSIP_STACK_SET_HEADER
* Adds or updates a stack-level header.
* @param NAME_STR The name of the header to add or update.
* @param VALUE_STR The value of the header to add or update.
* @code
* int ret = tsip_stack_set(stack, 
*              TSIP_STACK_SET_HEADER("User-Agent", "IM-client/OMA1.0 doubango/v1.0.0"),
*              TSIP_STACK_SET_HEADER("Allow", "INVITE, ACK, CANCEL, BYE, MESSAGE, OPTIONS, NOTIFY, PRACK, UPDATE, REFER"),
*              TSIP_STACK_SET_NULL());
* @endcode
*
* @sa @ref TSIP_STACK_UNSET_HEADER()
*/
/**@ingroup tsip_stack_group
* @def TSIP_STACK_SET_HEADER
* Removes a stack-level header.
* @param NAME_STR The name of the header to remove.
* @code
* int ret = tsip_stack_set(stack, 
*              TSIP_STACK_UNSET_HEADER("User-Agent""),
*              TSIP_STACK_UNSET_HEADER("Allow"),
*              TSIP_STACK_UNSET_HEADER("My_Header"),
*              TSIP_STACK_SET_NULL());
* @endcode
*
* @sa @ref TSIP_STACK_SET_HEADER()
*/
#define TSIP_STACK_SET_HEADER(NAME_STR, VALUE_STR)		pname_header, (const char*)NAME_STR, (const char*)VALUE_STR
#define TSIP_STACK_UNSET_HEADER(NAME_STR)				TSIP_STACK_SET_HEADER(NAME_STR, ((const char*)-1))

/* 3GPP IMS/LTE stack (for internal use). only tsip_stack_handle_t should be visible. */
typedef struct tsip_stack_s
{
	TSK_DECLARE_RUNNABLE;
	
	tsk_bool_t started;
	tsip_stack_callback_f callback;

	/* === Identity === */
	struct{
		char* display_name;
		tsip_uri_t *impu;
		tsip_uri_t *preferred;
		char *impi;
		char *password;
	} identity;

	/* === Network === */
	struct{
		char *local_ip;
		tnet_port_t local_port;

		char *proxy_cscf;
		tnet_port_t proxy_cscf_port;
		tnet_socket_type_t proxy_cscf_type;
		
		tsip_uri_t *realm;

		tsk_bool_t discovery_naptr;
		tsk_bool_t discovery_dhcp;
	} network;

	/* === Security === */
	struct{
		char* secagree_mech;
		tsk_bool_t earlyIMS;
		operator_id_t operator_id;
		amf_t amf;
		
		/* IPSec */
		struct{
			char* alg;
			char* ealg;
			char* mode;
			char* protocol;
		} ipsec;
		tsk_bool_t enable_secagree_ipsec;
		
		/* TLS */
		struct {
			char* ca;
			char* pbk;
			char* pvk;
		}tls;
		tsk_bool_t enable_secagree_tls;
	} security;
	
	
	
	
	
	
	
	
	tsip_uris_L_t* paths;
	tsip_uris_L_t* service_routes;
	tsip_uris_L_t* associated_uris;

	/* DNS context */
	tnet_dns_ctx_t *dns_ctx;

	/* DHCP context */

	

	/* QoS */

	/* Internals. */
	tsk_timer_manager_handle_t* timer_mgr;
	tsip_timers_t timers;
	tsip_ssessions_L_t *ssessions;
	tsk_params_L_t *headers;

	/* Layers */
	struct tsip_dialog_layer_s *layer_dialog;
	struct tsip_transac_layer_s *layer_transac;
	struct tsip_transport_layer_s *layer_transport;
}
tsip_stack_t;




TINYSIP_API tsip_stack_handle_t *tsip_stack_create(tsip_stack_callback_f callback, const char* realm_uri, const char* impi_uri, const char* impu_uri, ...);
TINYSIP_API int tsip_stack_start(tsip_stack_handle_t *self);
TINYSIP_API int tsip_stack_set(tsip_stack_handle_t *self, ...);
TINYSIP_API int tsip_stack_stop(tsip_stack_handle_t *self);

#define TSIP_STACK_EVENT_RAISE(stack, status_code, reason_phrase, incoming, type) \
	TSK_RUNNABLE_ENQUEUE(TSK_RUNNABLE(stack), (const tsip_stack_handle_t*)stack, (short)status_code, (const char*)reason_phrase, (unsigned)incoming, (tsip_event_type_t)type);


TINYSIP_GEXTERN const tsk_object_def_t *tsip_stack_def_t;

TSIP_END_DECLS

#endif /* TINYSIP_TSIP_H */