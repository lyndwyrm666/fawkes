
/***************************************************************************
 *  qa_config_change_handler.cpp - QA for configuration change handlers
 *
 *  Created: Mon Nov 12 19:11:06 2007
 *  Copyright  2006-2007  Tim Niemueller [www.niemueller.de]
 *
 *  $Id$
 *
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/// @cond QA

#include <config/sqlite.h>

#include <iostream>

using namespace std;

class QAConfigChangeHandler : public ConfigurationChangeHandler
{
public:
  QAConfigChangeHandler() : ConfigurationChangeHandler("/testing") {}

  virtual void
  config_tag_changed(const char *new_tag)
  {
    printf("CCH: New tag '%s'\n", new_tag);
  }

  virtual void
  config_value_changed(const char *path, int value)
  {
    printf("CCH: Integer '%s' changed to %i\n", path, value);
  }

  virtual void
  config_value_changed(const char *path, unsigned int value)
  {
    printf("CCH: Unsigned Integer '%s' changed to %u\n", path, value);
  }

  virtual void
  config_value_changed(const char *path, float value)
  {
    printf("CCH: Float '%s' changed to %f\n", path, value);
  }

  virtual void
  config_value_changed(const char *path, bool value)
  {
    printf("CCH: Bool '%s' changed to %i\n", path, value);
  }

  virtual void
  config_value_changed(const char *path, const char *value)
  {
    printf("CCH: String '%s' changed to %s\n", path, value);
  }

  virtual void
  config_value_erased(const char *path)
  {
    printf("CCH: Value '%s' erased\n", path);
  }

};

int
main(int argc, char **argv)
{
  SQLiteConfiguration *config = new SQLiteConfiguration(CONFDIR);

  QAConfigChangeHandler qach;
  config->add_change_handler(&qach);

  try {
    cout << "Loading configuration..." << flush;
    config->load("qa.db", "qa_defaults.db");
    cout << "done" << endl;
  } catch (CouldNotOpenConfigException &e) {
    cout << "failed" << endl;
    e.print_trace();
  }

  try {
    float of = 5.234;
    cout << "[FLOAT] set f=" << of << "..." << endl;
    config->set_float("/testing/float", of);
    cout << "[FLOAT] get..." << endl;
    float f = config->get_float("/testing/float");
    printf("done, f=%f\n", f);
  } catch (ConfigurationException &e) {
    cout << "failed" << endl;
    e.print_trace();
  }

  try {
    unsigned int ou = 6;
    cout << "[UINT] set u=" << ou << "..." << endl;
    config->set_uint("/testing/uint", ou);
    cout << "[UINT] get..." << endl;
    unsigned int u = config->get_uint("/testing/uint");
    printf("done, u=%u\n", u);
  } catch (ConfigurationException &e) {
    cout << "failed" << endl;
    e.print_trace();
  }

  try {
    int oi = -7;
    cout << "[INT] set i=" << oi << "..." << endl;
    config->set_int("/testing/int", oi);
    cout << "[INT] get..." << endl;
    int i = config->get_int("/testing/int");
    printf("done, i=%i\n", i);
  } catch (ConfigurationException &e) {
    cout << "failed" << endl;
    e.print_trace();
  }

  try {
    bool ob = true;
    cout << "[BOOL] set b=" << ob << "..." << endl;
    config->set_bool("/testing/bool", ob);
    cout << "[BOOL] get..." << endl;
    bool b = config->get_bool("/testing/bool");
    printf("done, b=%s\n", (b ? "true" : "false"));
  } catch (ConfigurationException &e) {
    cout << "failed" << endl;
    e.print_trace();
  }

  try {
    string os = "This ain't no paradoxon";
    cout << "[STRING] set s='" << os << "'..." << endl;
    config->set_string("/testing/string", os);
    cout << "[STRING] get..." << endl;
    string s = config->get_string("/testing/string");
    printf("done, s='%s'\n", s.c_str());
  } catch (ConfigurationException &e) {
    cout << "failed" << endl;
    e.print_trace();
  }

  try {
    cout << "[EXIST] Checking if test string exists..." << endl;
    if ( config->exists("/testing/string") ) {
      cout << "success";
    } else {
      cout << "failed";
    }
    cout << endl;
  } catch (ConfigurationException &e) {
    cout << "failed" << endl;
    e.print_trace();
  }

  try {
    string os = "This ain't no paradoxon";
    cout << "[LONGSTRING] set s='" << os << "'..." << endl;
    config->set_string("/testing/veryveryveryverylongstring", os);
    cout << "[LONGSTRING] get..." << endl;
    string s = config->get_string("/testing/veryveryveryverylongstring");
    printf("done, s='%s'\n", s.c_str());
  } catch (ConfigurationException &e) {
    cout << "failed" << endl;
    e.print_trace();
  }

  cout << "[ERASE] erasing all values" << endl;
  config->erase("/testing/float");
  config->erase("/testing/uint");
  config->erase("/testing/int");
  config->erase("/testing/bool");
  config->erase("/testing/string");
  config->erase("/testing/veryveryveryverylongstring");

  config->rem_change_handler(&qach);

  delete config;

  return 0;
}



/// @endcond
