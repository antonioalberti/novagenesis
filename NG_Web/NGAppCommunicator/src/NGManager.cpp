/*
 * NGManager.cpp
 *
 *  Created on: 08/11/2015
 *      Author: danf
 */

#include "NGManager.h"
#include <gio/gio.h>
#include <stdlib.h>

#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/writer.h" 		// for stringify JSON
#include <stdio.h>

using namespace rapidjson;

NGManager*
NGManager::instance = NULL;
GDBusNodeInfo*
NGManager::introspection_data = NULL;

/* Introspection data for the service we are exporting */
const gchar
NGManager::introspection_xml[] = "<node>"
		"  <interface name='NGManager.Communication'>"
		"    <method name='SearchByLiteral'>"
		"      <arg type='s' name='jLiteral' direction='in'/>"
		"	     <arg type='b' name='response' direction='out'/>"
		"    </method>"
		"    <method name='SearchByMurmur'>"
		"      <arg type='s' name='jLiteral' direction='in'/>"
		"	     <arg type='b' name='response' direction='out'/>"
		"    </method>"
		"  </interface>"
		"</node>";

static void
handle_method_call (GDBusConnection       *connection,
        const gchar           *sender,
        const gchar           *object_path,
        const gchar           *interface_name,
        const gchar           *method_name,
        GVariant              *parameters,
        GDBusMethodInvocation *invocation,
        gpointer               user_data)
{
	NGManager::getInstance()->handle_method_call(
				connection,sender,object_path,
				interface_name,method_name,parameters,
				invocation,user_data);
}

/* for now */
const GDBusInterfaceVTable
NGManager::interface_vtable = { &::handle_method_call };


static void
on_bus_acquired(GDBusConnection *connection, const gchar *name,
		gpointer user_data)
{
	printf("on_bus_acquired\n");
	NGManager::getInstance()->on_bus_acquired(connection,name,user_data);
}

static void
on_name_acquired(GDBusConnection *connection, const gchar *name,
		gpointer user_data)
{
	printf("on_name_acquired\n");
	NGManager::getInstance()->on_name_acquired(connection,name,user_data);
}

static void
on_name_lost(GDBusConnection *connection, const gchar *name,
		gpointer user_data)
{
	printf("on_name_lost\n");
	NGManager::getInstance()->on_name_lost(connection,name,user_data);
}

void
NGManager::handle_method_call (GDBusConnection       *connection,
        const gchar           *sender,
        const gchar           *object_path,
        const gchar           *interface_name,
        const gchar           *method_name,
        GVariant              *parameters,
        GDBusMethodInvocation *invocation,
        gpointer               user_data)
{

	if (g_strcmp0(method_name, "SearchByLiteral") == 0) {
		const gchar *json;
		g_variant_get(parameters, "(&s)", &json);

		//g_print("SearchByLiteral(%s)\n", json);

		bool isExecuted = searchByLiteral(json);

		g_dbus_method_invocation_return_value(invocation,
				g_variant_new("(b)", isExecuted));

	} else if(g_strcmp0(method_name, "SearchByMurmur") == 0){
		const gchar *json;
		g_variant_get(parameters, "(&s)", &json);

		//g_print("SearchByMurmur(%s)\n", json);

		bool isExecuted = searchByMurmur(json);

		g_dbus_method_invocation_return_value(invocation,
				g_variant_new("(b)", isExecuted));
	}
}

void
NGManager::on_bus_acquired(GDBusConnection *connection, const gchar *name,
		gpointer user_data) {
	guint registration_id;

	if(!_isAdaptorRegistred){
		registration_id = g_dbus_connection_register_object(connection,
				"/NGManagerObject", introspection_data->interfaces[0],
				&interface_vtable,
				NULL, /* user_data */
				NULL, /* user_data_free_func */
				NULL); /* GError** */
		if(registration_id == 0){
			printf("Error on registration.");
		}

		_isAdaptorRegistred = true;
	}
}

bool
NGManager::isAdaptorRegistred()
{
	return _isAdaptorRegistred;
}


void
NGManager::on_name_acquired(GDBusConnection *connection, const gchar *name,
		gpointer user_data)
{

}

void
NGManager::on_name_lost(GDBusConnection *connection, const gchar *name,
		gpointer user_data) {

}

NGManager::NGManager() :
		bHasLiteral(false),
		_isAdaptorRegistred(false)
{
	arrLiteral.clear();
	initDBusAdaptors();
	initDBusInterfaces();
}

NGManager::~NGManager() {
	printf("~NGManager\n");
	g_bus_unown_name(owner_id);
	g_dbus_node_info_unref(introspection_data);
}

NGManager*
NGManager::getInstance() {
	if (instance == NULL) {
		instance = new NGManager;
	}
	return instance;
}

void
NGManager::initDBusAdaptors() {
	printf("=== NGManager::initDBusAdaptors\n");

	/* We are lazy here - we don't want to manually provide
	 * the introspection data structures - so we just build
	 * them from XML.
	 */
	introspection_data = g_dbus_node_info_new_for_xml(introspection_xml, NULL);
	g_assert(introspection_data != NULL);

	owner_id = g_bus_own_name (G_BUS_TYPE_SESSION,
							 "com.NGManager.Communicator",
							 G_BUS_NAME_OWNER_FLAGS_NONE,
							 &::on_bus_acquired,
							 &::on_name_acquired,
							 &::on_name_lost,
							 NULL,
							 NULL);

}

void
NGManager::initDBusInterfaces(){
	  printf("=== NGManager::initDBusInterfaces\n");

	  GDBusConnection *c;
	  GError *error = NULL;

	  c = g_bus_get_sync (G_BUS_TYPE_SESSION, NULL, &error);
	  error = NULL;
	  p = g_dbus_proxy_new_sync (c,
	                             G_DBUS_PROXY_FLAGS_NONE,
	                             NULL,                      	/* GDBusInterfaceInfo* */
	                             "com.NGManager.NGBrowser", 	/* name */
	                             "/NGConnectorObject", 			/* object path */
	                             "NGConnector.Connection",     /* interface name */
	                             NULL,                      	/* GCancellable */
	                             &error);
	  g_assert_no_error (error);
}

void
NGManager::ifaceComplete(vector<string> *arrHash)
{
	/* Init JSON Object */
	Document doc;
	doc.SetObject();

	/* Populate Json */
	Value arrCompleteHash;
	arrCompleteHash.SetArray();
	for (unsigned int i = 0; i < arrHash->size(); ++i) {
		Value chash;
		chash.SetString(StringRef(arrHash->at(i).c_str()));
		arrCompleteHash.PushBack(chash,doc.GetAllocator());
	}

	/* Insert */
	doc.AddMember("wordlist",arrCompleteHash,doc.GetAllocator());

	/* Convert Json */
	StringBuffer sb;
	Writer<StringBuffer> writer(sb);
	doc.Accept(writer);    // Accept() traverses the DOM and generates Handler events.

	/* DBus Connection */
	GError *error = NULL;
	GVariant *value = g_variant_new("(&s)",(char*)sb.GetString());
	GVariant *tpResult = g_dbus_proxy_call_sync(p,"complete",value,G_DBUS_CALL_FLAGS_NONE,500,NULL,&error);
	g_variant_unref(tpResult);
/*
	GVariant *result = g_variant_get_child_value(tpResult,0);
	g_variant_unref(tpResult);

	g_assert_no_error (error);
	bool res = g_variant_get_boolean(result);
	g_variant_unref(result);

	g_print("Result: %d\n",res);
*/

}

bool
NGManager::hasLiteral()
{
	return bHasLiteral;
}

void
NGManager::pullArrLiteral(vector<string> *arrOutput)
{
	/*
	for (unsigned int i = 0; i < arrLiteral.size(); ++i) {
		arrOutput->push_back(arrLiteral.at(i));
	}
	arrLiteral.clear();
	bHasLiteral = false;
	*/
	arrOutput->push_back(arrLiteral.at(0));
	arrLiteral.erase(arrLiteral.begin());
	if( arrLiteral.size() == 0 ){
		bHasLiteral = false;
	}
}

/**
 * JSON param format:
 * 		{ "wordlist" : [ "abcd", "cde" ] }
 */
bool
NGManager::searchByLiteral(const gchar* json) {

	Document doc;
	doc.Parse(json);

	if( doc.IsObject() && doc.FindMember("wordlist") != doc.MemberEnd() ){
		Value& wordlist = doc["wordlist"];
		for (Value::ConstValueIterator itr = wordlist.Begin();
				itr != wordlist.End(); ++itr)
		{
//			g_print("[%s]\n",itr->GetString());
			arrLiteral.push_back(itr->GetString());
		}
		if( arrLiteral.size() > 0){
			bHasLiteral = true;
		}
		return true;
	}
	return false;
}

/**
 * JSON param format:
 * 		{ "wordlist" : [ "abcd", "cde" ] }
 */
bool
NGManager::searchByMurmur(const gchar* json) {

	Document doc;
	doc.Parse(json);

	if( doc.IsObject() && doc.FindMember("word") != doc.MemberEnd() ){
		Value& word = doc["word"];
		arrLiteral.push_back(word.GetString());
		bHasLiteral = true;
		return true;
	}
	return false;
}
