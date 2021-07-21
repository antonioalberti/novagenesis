/*
 * NGManager.h
 *
 *  Created on: 08/11/2015
 *      Author: danf
 */

#ifndef NGMANAGER_H_
#define NGMANAGER_H_

#include <gio/gio.h>
#include <vector>
#include <string>

using namespace std;

class NGManager {

public:
	static NGManager* getInstance();
	void ifaceComplete(vector<string> *arrHash);
	bool hasLiteral();
	void pullArrLiteral(vector<string> *arrOutput);

public:
	/* Callback's */
	void
	on_name_lost (GDBusConnection *connection,
	              const gchar     *name,
	              gpointer         user_data);
	void
	on_name_acquired (GDBusConnection *connection,
	                  const gchar     *name,
	                  gpointer         user_data);
	void
	on_bus_acquired (GDBusConnection *connection,
	                 const gchar     *name,
	                 gpointer         user_data);
	void
	handle_method_call (GDBusConnection       *connection,
	                    const gchar           *sender,
	                    const gchar           *object_path,
	                    const gchar           *interface_name,
	                    const gchar           *method_name,
	                    GVariant              *parameters,
	                    GDBusMethodInvocation *invocation,
	                    gpointer               user_data);

	bool isAdaptorRegistred();

private:
	static NGManager* instance;
	static GDBusNodeInfo* introspection_data;
	static const gchar introspection_xml[];
	static const GDBusInterfaceVTable interface_vtable;
	guint owner_id;
	GDBusProxy *p;
	bool bHasLiteral;
	bool _isAdaptorRegistred;
	vector<string> arrLiteral;

private:
	NGManager();
	virtual ~NGManager();

	void initDBusAdaptors();
	void initDBusInterfaces();

	/* DBus Method's */
	bool searchByLiteral(const gchar *json);
	bool searchByMurmur(const gchar *json);
};

#endif /* NGMANAGER_H_ */
