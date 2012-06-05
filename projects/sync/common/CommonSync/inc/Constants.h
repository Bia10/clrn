#ifndef Constants_h__
#define Constants_h__

//! Kernel module id
const unsigned int		KERNEL_MODULE_ID			= 0;
//! Events module id
const unsigned int		EVENTS_MODULE_ID			= 1;
//! Settings module id
const unsigned int		SETTINGS_MODULE_ID			= 2;
//! DB module id
const unsigned int		DATABASE_MODULE_ID			= 3;
//! Network module id
const unsigned int		NETWORK_MODULE_ID			= 4;
//! Common module id
const unsigned int		COMMON_MODULE_ID			= 5;
//! Network module id
const unsigned int		LAST_MODULE_ID				= COMMON_MODULE_ID;
//! Kernel database name
const char				KERNEL_DATABASE_FILE_NAME[] = "../../../data/kernel.db";
//! Modules table name
const char				MODULES_TABLE_NAME[]		= "plugins";
//! Settings table name
const char				SETTINGS_TABLE_NAME[]		= "settings";
//! Hosts table name
const char				HOSTS_TABLE_NAME[]			= "hosts";
//! Host map table name
const char				HOSTMAP_TABLE_NAME[]		= "host_map";
//! Accounts table name
const char				ACCOUNTS_TABLE_NAME[]		= "accounts";
//! Host map event name
const char				HOSTMAP_EVENT_NAME[]		= "host_map_event";
//! Host status event name
const char				HOST_STATUS_EVENT_NAME[]	= "host_status_event";




#endif // Constants_h__