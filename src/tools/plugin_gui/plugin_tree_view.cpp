
/***************************************************************************
 *  plugin_tree_view.cpp - Displays a list of Fawkes plugins and allows to
 *                         start/stop them
 *
 *  Created: Fri Sep 26 21:13:48 2008
 *  Copyright  2008  Daniel Beck
 *             2008  Tim Niemueller [www.niemueller.de]
 *
 *  $Id: plugin_gui.h 899 2008-04-10 11:36:58Z tim $
 *
 ****************************************************************************/

/*  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  Read the full text in the LICENSE.GPL file in the doc directory.
 */

#include <tools/plugin_gui/plugin_tree_view.h>
#include <netcomm/fawkes/client.h>
#include <plugin/net/messages.h>
#include <plugin/net/list_message.h>

#include <cstring>
#include <string>

using namespace std;
using namespace fawkes;

/** @class PluginTreeView "plugin_tree_view.h"
 * A TreeView class to list available plugins und trigger their
 * loading/unloading.
 *
 * @author Daniel Beck
 * @author Tim Niemueller
 */

/** @class PluginTreeView::PluginRecord "plugin_tree_view.h"
 * Column record class for the plugin tree view.
 *
 * @author Daniel Beck
 */

/** @var PluginTreeView::m_plugin_list
 * Storage object for the plugin data.
 */

/** @var PluginTreeView::m_plugin_record
 * Column record object.
 */

/** Constructor.
 * @param cobject pointer to base object type
 * @param ref_xml Glade XML file
 */
PluginTreeView::PluginTreeView( BaseObjectType* cobject,
				const Glib::RefPtr<Gnome::Glade::Xml> ref_xml )
  : Gtk::TreeView(cobject),
    m_dispatcher(FAWKES_CID_PLUGINMANAGER)
{
  m_plugin_list = Gtk::ListStore::create(m_plugin_record);
  set_model(m_plugin_list);
  append_column("#", m_plugin_record.index);
  append_column_editable("Status", m_plugin_record.loaded);
  append_column("Plugin", m_plugin_record.name);

  Gtk::CellRendererToggle* renderer;
  renderer = dynamic_cast<Gtk::CellRendererToggle*>( get_column_cell_renderer(1) );
  renderer->signal_toggled().connect( sigc::mem_fun(*this, &PluginTreeView::on_status_toggled));

  m_dispatcher.signal_connected().connect(sigc::mem_fun(*this, &PluginTreeView::on_connected));
  m_dispatcher.signal_disconnected().connect(sigc::mem_fun(*this, &PluginTreeView::on_disconnected));
  m_dispatcher.signal_message_received().connect(sigc::mem_fun(*this, &PluginTreeView::on_message_received));
}


/** Destructor. */
PluginTreeView::~PluginTreeView()
{
  if ( m_dispatcher.get_client()->connected() )
  {
    // unsubscribe
    FawkesNetworkMessage* msg = new FawkesNetworkMessage(FAWKES_CID_PLUGINMANAGER,
							 MSG_PLUGIN_UNSUBSCRIBE_WATCH);
    m_dispatcher.get_client()->enqueue(msg);
    msg->unref();			

    m_dispatcher.get_client()->deregister_handler(FAWKES_CID_PLUGINMANAGER);
  }
}


/** Set the network client.
 * @param client a Fawkes network client
 */
void
PluginTreeView::set_network_client(FawkesNetworkClient* client)
{
  m_dispatcher.set_client(client);
}


void
PluginTreeView::on_connected()
{
  try
  {
    FawkesNetworkClient *client = m_dispatcher.get_client();

    // subscribe for load-/unload messages
    FawkesNetworkMessage* msg = new FawkesNetworkMessage(FAWKES_CID_PLUGINMANAGER,
							 MSG_PLUGIN_SUBSCRIBE_WATCH);
    client->enqueue(msg);
    msg->unref();
      
    // request list of available plugins
    msg = new FawkesNetworkMessage(FAWKES_CID_PLUGINMANAGER,
				   MSG_PLUGIN_LIST_AVAIL);
    client->enqueue(msg);
    msg->unref();
      
    // request list of loaded plugins
    msg = new FawkesNetworkMessage(FAWKES_CID_PLUGINMANAGER,
				   MSG_PLUGIN_LIST_LOADED);
    client->enqueue(msg);
    msg->unref();
  }
  catch (Exception& e)
  {
    e.print_trace();
  }
}


/** Signal handler that is called whenever the connection is terminated. */
void
PluginTreeView::on_disconnected()
{
  m_plugin_list->clear();
}


void
PluginTreeView::on_message_received(fawkes::FawkesNetworkMessage* msg)
{
  if (msg->cid() != FAWKES_CID_PLUGINMANAGER)  return;

  // loading
  unsigned int msgid = msg->msgid();
  if ( (msgid == MSG_PLUGIN_LOADED) ||
       (msgid == MSG_PLUGIN_LOAD_FAILED) ||
       (msgid == MSG_PLUGIN_UNLOADED) ||
       (msgid == MSG_PLUGIN_UNLOAD_FAILED) )
  {
    Glib::ustring name = "";
    bool loaded = false;

    if ( msgid == MSG_PLUGIN_LOADED)
    {
      if ( msg->payload_size() != sizeof(plugin_loaded_msg_t) ) 
      {
	printf("Invalid message size (load succeeded)\n");
      } 
      else 
      {
	plugin_loaded_msg_t* m = (plugin_loaded_msg_t*) msg->payload();
	name   = m->name;
	loaded = true;
      }
    }
    else if ( msgid == MSG_PLUGIN_LOAD_FAILED ) 
    {
      if ( msg->payload_size() != sizeof(plugin_load_failed_msg_t) ) 
      {
	printf("Invalid message size (load failed)\n");
      } 
      else 
      {
	plugin_load_failed_msg_t* m = (plugin_load_failed_msg_t*) msg->payload();
	name   = m->name;
	loaded = false;
      }
    }
    else if ( msg->msgid() == MSG_PLUGIN_UNLOADED ) 
    {
      if ( msg->payload_size() != sizeof(plugin_unloaded_msg_t) ) 
      {
	printf("Invalid message size (unload succeeded)\n");
      } 
      else 
      {
	plugin_unloaded_msg_t* m = (plugin_unloaded_msg_t*) msg->payload();
	name   = m->name;
	loaded = false;
      }
    }
    else if ( msg->msgid() == MSG_PLUGIN_UNLOAD_FAILED) 
    {
      if ( msg->payload_size() != sizeof(plugin_unload_failed_msg_t) ) 
      {
	printf("Invalid message size (unload failed)\n");
      } 
      else 
      {
	plugin_unload_failed_msg_t* m = (plugin_unload_failed_msg_t*) msg->payload();
	name   = m->name;
	loaded = true;
      }
    }

    Gtk::TreeIter iter;
    for ( iter  = m_plugin_list->children().begin();
	  iter != m_plugin_list->children().end();
	  ++iter )
    {
      Glib::ustring n = (*iter)[m_plugin_record.name];
      if ( n == name )
      {
	(*iter)[m_plugin_record.loaded] = loaded;
	break;
      }
    }
  }
  else if (msgid == MSG_PLUGIN_AVAIL_LIST) 
  {
    PluginListMessage* plm = msg->msgc<PluginListMessage>();
    while ( plm->has_next() ) 
    {
      char* name = plm->next();

      Gtk::TreeModel::Row row = *m_plugin_list->append();
      unsigned int index = m_plugin_list->children().size();
      row[m_plugin_record.index]  = index;
      row[m_plugin_record.name]   = name;
      row[m_plugin_record.loaded] = false;

      free(name);
    }
    delete plm;
  }
  else if ( msg->msgid() == MSG_PLUGIN_AVAIL_LIST_FAILED) 
  {
    printf("Obtaining list of available plugins failed\n");
  }
  else if (msg->msgid() == MSG_PLUGIN_LOADED_LIST ) 
  {
    PluginListMessage* plm = msg->msgc<PluginListMessage>();
    while ( plm->has_next() ) 
    {
      char* name = plm->next();

      Gtk::TreeIter iter;
      for ( iter  = m_plugin_list->children().begin();
	    iter != m_plugin_list->children().end();
	    ++iter )
      {
	Glib::ustring n = (*iter)[m_plugin_record.name];
	if ( n == name )
	{
	  (*iter)[m_plugin_record.loaded] = true;
	  break;
	}
      }
      free(name);
    }
    delete plm;
  }
  else if ( msg->msgid() == MSG_PLUGIN_LOADED_LIST_FAILED) 
  {
    printf("Obtaining list of loaded plugins failed\n");
  }

  // unknown message received
  else
  {
    printf("received message with msg-id %d\n", msg->msgid());
  }
}

/** Signal handler that is called when the loaded checkbox is
 * toggled.
 * @param path the path of the selected row
 */
void
PluginTreeView::on_status_toggled(const Glib::ustring& path)
{
  if ( ! m_dispatcher.get_client()->connected() )  return;

  Gtk::TreeModel::Row row = *m_plugin_list->get_iter(path);
  Glib::ustring plugin_name = row[m_plugin_record.name];
  bool loaded = row[m_plugin_record.loaded];

  if (loaded)
  {
    plugin_load_msg_t* m = (plugin_load_msg_t*) calloc(1, sizeof(plugin_load_msg_t));
    strncpy(m->name, plugin_name.c_str(), PLUGIN_MSG_NAME_LENGTH);
      
    FawkesNetworkMessage *msg = new FawkesNetworkMessage(FAWKES_CID_PLUGINMANAGER,
							 MSG_PLUGIN_LOAD,
							 m, sizeof(plugin_load_msg_t));
    m_dispatcher.get_client()->enqueue(msg);
    msg->unref();
  }
  else
  {
    plugin_unload_msg_t* m = (plugin_unload_msg_t *)calloc(1, sizeof(plugin_unload_msg_t));
    strncpy(m->name, plugin_name.c_str(), PLUGIN_MSG_NAME_LENGTH);
      
    FawkesNetworkMessage *msg = new FawkesNetworkMessage(FAWKES_CID_PLUGINMANAGER,
							 MSG_PLUGIN_UNLOAD,
							 m, sizeof(plugin_unload_msg_t));
    m_dispatcher.get_client()->enqueue(msg);
    msg->unref();
  }
}