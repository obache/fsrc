$NetBSD$

* NetBSD and OpenBSD support

--- gatedevice.c.orig	2007-02-07 23:07:27.000000000 +0000
+++ gatedevice.c
@@ -1,4 +1,5 @@
 #include <syslog.h>
+#include <stdio.h>
 #include <stdlib.h>
 #include <upnp/ixml.h>
 #include <string.h>
@@ -10,6 +11,10 @@
 #include "gatedevice.h"
 #include "pmlist.h"
 #include "util.h"
+#if defined(__NetBSD__) || defined(__OpenBSD__)
+#include <sys/ioctl.h>
+#include <sys/socket.h>
+#endif
 
 //Definitions for mapping expiration timer thread
 static TimerThread gExpirationTimerThread;
@@ -326,12 +331,14 @@ int GetCommonLinkProperties(struct Upnp_
 /* get specified statistic from /proc/net/dev */
 int GetTotal(struct Upnp_Action_Request *ca_event, stats_t stat)
 {
-	char dev[IFNAMSIZ], resultStr[RESULT_LEN];
+	char resultStr[RESULT_LEN];
 	const char *methods[STATS_LIMIT] =
 		{ "BytesSent", "BytesReceived", "PacketsSent", "PacketsReceived" };
 	unsigned long stats[STATS_LIMIT];
-	FILE *proc;
 	IXML_Document *result;
+#ifdef linux
+	char dev[IFNAMSIZ];
+	FILE *proc;
 	int read;
 	
 	proc = fopen("/proc/net/dev", "r");
@@ -350,7 +357,53 @@ int GetTotal(struct Upnp_Action_Request 
 	while (read != EOF && (read == 5 && strncmp(dev, g_vars.extInterfaceName, IFNAMSIZ) != 0));
 
 	fclose(proc);
-
+#endif
+#ifdef __NetBSD__
+	struct ifdatareq ifdr;
+	struct if_data *ifi;
+	int fd;
+
+	ifi = &ifdr.ifdr_data;
+	fd = socket(AF_INET, SOCK_DGRAM, 0);
+	if (fd >= 0) {
+		strncpy(ifdr.ifdr_name, g_vars.extInterfaceName, IFNAMSIZ);
+		if (ioctl(fd, SIOCGIFDATA, &ifdr) >= 0) {
+			stats[STATS_RX_PACKETS] = (unsigned long)ifi->ifi_ipackets;
+			stats[STATS_TX_PACKETS] = (unsigned long)ifi->ifi_opackets;
+			stats[STATS_RX_BYTES] = (unsigned long)ifi->ifi_ibytes;
+			stats[STATS_TX_BYTES] = (unsigned long)ifi->ifi_obytes;
+		} else {
+			close(fd);
+			return 0;
+		}
+	} else {
+		return 0;
+	}
+	close(fd);
+#endif
+#ifdef __OpenBSD__
+	struct ifreq ifdr;
+	struct if_data ifi;
+	int fd;
+
+	fd = socket(AF_INET, SOCK_DGRAM, 0);
+	if (fd >= 0) {
+		strncpy(ifdr.ifdr_name, g_vars.extInterfaceName, IFNAMSIZ);
+		ifdr.ifdr_data = (caddr_t) &ifi;
+		if (ioctl(fd, SIOCGIFDATA, &ifdr) >= 0) {
+			stats[STATS_RX_PACKETS] = ifi.ifi_ipackets;
+			stats[STATS_TX_PACKETS] = ifi.ifi_opackets;
+			stats[STATS_RX_BYTES] = ifi.ifi_ibytes;
+			stats[STATS_TX_BYTES] = ifi.ifi_obytes;
+		} else {
+			close(fd);
+			return 0;
+		}
+	} else {
+		return 0:
+	}
+	close(fd);
+#endif
 	snprintf(resultStr, RESULT_LEN,
 		"<u:GetTotal%sResponse xmlns:u=\"urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1\">\n"
 		"<NewTotal%s>%lu</NewTotal%s>\n"
